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

file(GLOB RESOURCE_FILES "resources/*")
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Main Executable (CLI)
add_executable(${PROGRAM_NAME} cli/main.cpp)
target_link_libraries(${PROGRAM_NAME} svg_handler_lib)

add_custom_target(run
    COMMAND ${PROGRAM_NAME} ${CMAKE_SOURCE_DIR}/resources/sample.svg
    DEPENDS ${PROGRAM_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cli
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
    tree_utils
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

# Qt GUI - Linux
set(QT_MIN_VERSION "6.5.0")
find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QTGUI_SOURCES
  gui_qt6/main.cpp
  gui_qt6/mainwindow.cpp
  gui_qt6/mainwindow.h
  gui_qt6/mainwindow.ui
)

set(QTGUI_EXEC_NAME svg_handler_qt)

qt_add_executable(${QTGUI_EXEC_NAME} ${QTGUI_SOURCES})

target_link_libraries(${QTGUI_EXEC_NAME}
    PRIVATE
        svg_handler_lib
        Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS ${QTGUI_EXEC_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_custom_target(run_qtgui
    COMMAND ${QTGUI_EXEC_NAME}
    DEPENDS ${QTGUI_EXEC_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(${QTGUI_EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gui
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type" FORCE)
endif()
