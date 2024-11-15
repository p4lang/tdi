cmake_minimum_required(VERSION 3.12)
project(libtdi_cli VERSION 0.1 LANGUAGES C)

find_package(Python3 COMPONENTS Interpreter Development)

set(TDI_CLI_SRCS
  tdi_cli.c
)

set(TDI_CLI_EXAMPLES_SRCS
  cli_example.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/target-syslibs/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/target-utils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/target-utils/third-party/klish)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(tdi_cli_o OBJECT ${TDI_CLI_SRCS})
target_include_directories(tdi_cli_o PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(tdi_cli_o PUBLIC target_sys tdi clish python3.11)

add_library(bfshell_plugin_tdi SHARED $<TARGET_OBJECTS:tdi_cli_o>)
target_link_libraries(bfshell_plugin_tdi PUBLIC target_sys tdi clish python3.11)

add_executable(tdi_cli_example ${TDI_CLI_EXAMPLES_SRCS})
target_link_libraries(tdi_cli_example bfshell_plugin_tdi)

install(TARGETS tdi_cli_example DESTINATION bin)
install(FILES xml/startup.xml xml/tdi.xml DESTINATION share/cli/xml)
