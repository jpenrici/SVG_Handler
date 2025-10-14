cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(SVG_Handler LANGUAGES CXX)

set(PROGRAM_NAME svg_handler_cli)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

include_directories(include)

add_library(svg_handler_lib
  src/svg_handler.cpp
  src/svg_reader.cpp
  src/string_utils.cpp
  src/tree_utils.cpp
  src/csv_exporter.cpp
)

set(RESOURCE_FILES
  resources/sample.svg
)

foreach(FILE ${RESOURCE_FILES})
  file(COPY ${FILE} DESTINATION ${CMAKE_BINARY_DIR}/resources)
endforeach()

# Main Executable (CLI)
add_executable(${PROGRAM_NAME} cli/main.cpp)
target_link_libraries(${PROGRAM_NAME} svg_handler_lib)

add_custom_target(run
    COMMAND ${PROGRAM_NAME} ${CMAKE_SOURCE_DIR}/resources/sample.svg
    DEPENDS ${PROGRAM_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Test All
add_executable(${PROGRAM_NAME}_test tests/test.cpp)
target_link_libraries(${PROGRAM_NAME}_test svg_handler_lib)

add_custom_target(test_run
    COMMAND ${PROGRAM_NAME}_test
    DEPENDS ${PROGRAM_NAME}_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Test using internal modules
set(INTERNAL_TEST_MODULES
    string_utils
    svg_reader
    #tree_utils
    csv_exporter
)

foreach(MODULE_NAME IN LISTS INTERNAL_TEST_MODULES)
    set(EXEC_NAME test_run_${MODULE_NAME})
    add_executable(${EXEC_NAME} src/${MODULE_NAME}.cpp)
    target_compile_definitions(${EXEC_NAME} PRIVATE BUILD_TEST_EXE)
    add_custom_target(test_${MODULE_NAME}
        COMMAND ${EXEC_NAME}
        DEPENDS ${EXEC_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endforeach()
