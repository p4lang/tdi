
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TDI_SRCS
  tdi_init.cpp
  tdi_info.cpp
  tdi_target.cpp
  tdi_table.cpp
  tdi_table_key.cpp
  tdi_learn.cpp
)

set(TDI_C_FRONTEND_SRCS
  c_frontend/tdi_init_c.cpp
)

add_library(tdi_o OBJECT ${TDI_SRCS} ${TDI_C_FRONTEND_SRCS})
add_library(tdi SHARED $<TARGET_OBJECTS:tdi_o>)

target_link_libraries(tdi PUBLIC target_utils targetsys)

add_subdirectory(arch/tna)
add_subdirectory(targets/dummy)
