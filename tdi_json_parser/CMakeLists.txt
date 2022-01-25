
set(TDI_JSON_PARSING_SRCS
  tdi_cjson.cpp
  tdi_table_info.cpp
)

add_library(tdi_json_parser_o OBJECT ${TDI_JSON_PARSING_SRCS})
add_library(tdi_json_parser SHARED $<TARGET_OBJECTS:tdi_json_parser_o>)
target_link_libraries(tdi_json_parser PUBLIC target_sys target_utils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/target-utils/third-party/cJSON)
