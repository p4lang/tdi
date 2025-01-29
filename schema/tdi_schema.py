#!/usr/bin/env python3

# This script generates a JSON schema for the TDI schema file. It can also
# validate the TDI schema produced by the compiler (from the P4Info message)
# against the JSON schema.

import argparse
import jsl
import json
import jsonschema
import sys

########################################################
#   Schema Version
########################################################

"""
Version Notes:
1.0.0:
- Initial version
"""

major_version = 1
medium_version = 0
minor_version = 0

def get_schema_version():
    return "%s.%s.%s" % (str(major_version), str(medium_version), str(minor_version))

########################################################

class Annotation(jsl.Document):
    class Options(object):
        title = "Annotation"
        description = "Annotation on an object, propagated from P4 source"
    name = jsl.StringField(required=False)
    value = jsl.StringField(required=False)

Annotations = jsl.ArrayField(required=False, items=jsl.DocumentField(Annotation, as_ref=True))

class NamedObject(jsl.Document):
    id_ = jsl.IntField(name="id", required=True, description="ID of the key field")
    name = jsl.StringField(required=True, decription="Name")
    annotations = Annotations

MatchType = jsl.StringField(
    required=True,
    enum=["Exact", "Ternary", "LPM", "Range", "ATCAM", "Optional"],
    description = "Type of the match key field")

class PrimitiveTypeBase(jsl.Document):
    pass

PrimitiveIntTypes = [
    "int8", "int16", "int32", "int64",
    "uint8", "uint16", "uint32", "uint64",
]

class PrimitiveIntType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveIntType"
        description = "A fixed-width integer type"
    type_ = jsl.StringField(name="type", required=True, enum=PrimitiveIntTypes)
    default_value = jsl.IntField(required=False)
    mask = jsl.StringField(required=False)

class PrimitiveBoolType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveBoolType"
        description = "Boolean type"
    type_ = jsl.StringField(name="type", required=True, enum=["bool"])
    default_value = jsl.BooleanField(required=False)

class PrimitiveBytesType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveBytesType"
        description = "A fixed-width bitvector type"
    type_ = jsl.StringField(name="type", required=True, enum=["bytes"])
    width = jsl.IntField(required=False)
    default_value = jsl.NumberField(required=False)
    mask = jsl.StringField(required=False)

class PrimitiveFloatType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveFloatType"
        description = "A fixed-width floating-point type"
    type_ = jsl.StringField(name="type", required=True,
                            enum=["float", "double"])
    default_value = jsl.NumberField(required=False)

class PrimitiveEnumType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveEnumType"
        description = "A list of string choices"
    type_ = jsl.StringField(name="type", required=True, enum=["enum", "string"])
    choices = jsl.ArrayField(required=True, items=jsl.StringField())
    default_value = jsl.StringField(required=False)

class PrimitiveStringType(PrimitiveTypeBase):
    class Options(object):
        title = "PrimitiveStringType"
        description = "An arbitrary string"
    type_ = jsl.StringField(name="type", required=True, enum=["string"])
    default_value = jsl.StringField(required=False)

PrimitiveType = jsl.OneOfField(
    name="type",
    required=True,
    fields=[jsl.DocumentField(PrimitiveIntType, as_ref=True),
            jsl.DocumentField(PrimitiveBoolType, as_ref=True),
            jsl.DocumentField(PrimitiveBytesType, as_ref=True),
            jsl.DocumentField(PrimitiveFloatType, as_ref=True),
            jsl.DocumentField(PrimitiveEnumType, as_ref=True),
            jsl.DocumentField(PrimitiveStringType, as_ref=True)]
)

class BaseDataField(NamedObject):
    annotations = Annotations
    type_ = PrimitiveType
    repeated = jsl.BooleanField(required=True)

class ContainerDataField(NamedObject):
    class Options(object):
        title = "ContainerDataField"
        description = "A data field which itself contains several data fields"
    annotations = Annotations
    container = jsl.ArrayField(
        required=True,
        items=jsl.OneOfField(fields=[
            # have to use strings because of cyclic dependency
            jsl.DocumentField("SingletonData", as_ref=True),
            jsl.DocumentField("OneOfData", as_ref=True)]),
        description="All data fields in the container")
    repeated = jsl.BooleanField(required=True)

DataField = jsl.OneOfField(required=True, fields=[
    jsl.DocumentField(document_cls=BaseDataField, as_ref=True),
    jsl.DocumentField(document_cls=ContainerDataField, as_ref=True)])

class KeyField(BaseDataField):
    class Options(object):
        title = "KeyField"
        description = "Table key specification"
    match_type = MatchType
    mandatory = jsl.BooleanField(required=True)
    repeated = jsl.BooleanField(required=True, enum=[False])

class CommonDataIface(jsl.Document):
    mandatory = jsl.BooleanField(required=True)
    read_only = jsl.BooleanField(required=True)

class SingletonData(CommonDataIface):
    class Options(object):
        title = "SingletonData"
        description = "A single action data field"
    singleton = DataField

class OneOfData(CommonDataIface):
    class Options(object):
        title = "OneOfData"
        description = "Union of possible action data fields"
    oneof = jsl.ArrayField(required=True, items=DataField)

# we could use a "singleton" field as well, but I believe that the increased
# uniformity is not required and would not make parsing easier in the drivers.
class ActionDataField(BaseDataField):
    class Options(object):
        title = "ActionDataField"
        description = "Action parameter specification"
    mandatory = jsl.BooleanField(required=True)
    repeated = jsl.BooleanField(required=True, enum=[False])
    read_only = jsl.BooleanField(required=True)

class ActionSpec(NamedObject):
    class Options(object):
        title = "ActionSpec"
        description = "List of action specifications; for each action we have a description of all action parameters"
    data = jsl.ArrayField(
        required=True, items=jsl.DocumentField(ActionDataField, as_ref=True))
    action_scope = jsl.StringField(
        required=True, enum=["TableAndDefault", "TableOnly", "DefaultOnly"])

class TableCommon(NamedObject):
    class Options(object):
        title = "Table"
        description = "Table object which can be controlled through TDI; can be either a P4 table or a fixed table"
    table_type = jsl.StringField(required=True)
    size = jsl.IntField(required=True)
    key = jsl.ArrayField(required=True,
                         items=jsl.DocumentField(KeyField, as_ref=True))
    data = jsl.ArrayField(
        required=True,
        items=jsl.OneOfField(fields=[
            jsl.DocumentField(SingletonData, as_ref=True),
            jsl.DocumentField(OneOfData, as_ref=True)]),
        description="All 'common' action data fields for the table")
    read_only = jsl.BooleanField(required=False)
    attributes = jsl.ArrayField(required=False, items=jsl.StringField())
    supported_operations = jsl.ArrayField(required=False, items=jsl.StringField())
    depends_on = jsl.ArrayField(
        required=False, items=jsl.IntField(),
        description="List of table ids which this table depends on; entries in this table may "
                    "include references to entries in the tables listed here."
                    "The depends on tables can be attached tables like action "
                    "data, action profile, action selector, meters etc.")

class TableGeneric(TableCommon):
    class Options(object):
        title = "TableGeneric"
        description = "Generic tables which can be controlled through TDI"

class TdiSchema(jsl.Document):
    class Options(object):
        description = "Schema for TDI"
    schema_version = jsl.StringField(required=True,
                                     description="Schema version used to produce this JSON.")
    tables = jsl.ArrayField(
        required=True,
        items=jsl.AnyOfField(fields=[
            jsl.DocumentField(TableGeneric, as_ref=True)]),
        description="All table objects")

def main():
    parser = argparse.ArgumentParser(
        description='TDI Json schema generator and validator')
    parser.add_argument('--dump-schema', action='store', required=False,
                        help='Dump the schema to this file')
    parser.add_argument('input_json', nargs='?',
                        help='Json file to validate')
    args = parser.parse_args()

    schema = TdiSchema.get_schema()
    if args.dump_schema:
        with open(args.dump_schema, 'w') as schema_f:
            json.dump(schema, schema_f, indent=4)
    if args.input_json:
        with open(args.input_json, 'r') as input_f:
            tdi_json = json.load(input_f)
            try:
                jsonschema.validate(tdi_json, schema)
            except jsonschema.ValidationError as e:
                print("Error when validating TDI JSON", file=sys.stderr)
                print(e, file=sys.stderr)
                sys.exit(1)
    sys.exit(0)

if __name__ == '__main__':
    main()
