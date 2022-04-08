project(libtdi_psa VERSION 0.1 LANGUAGES CXX)

set(TDI_PSA_SRCS
  c_frontend/psa_table_key_c.cpp
  #psa_target.cpp
)

add_library(tdi_psa_o OBJECT ${TDI_PSA_SRCS})
add_library(tdi_psa SHARED $<TARGET_OBJECTS:tdi_psa_o>)
target_link_libraries(tdi_psa PUBLIC tdi)
