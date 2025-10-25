cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(SVG_Handler LANGUAGES CXX)

set(PROGRAM_NAME svg_handler_cli)
set(LIBRARY_NAME svg_handler)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

# ========== Core Library ==========
add_library(${LIBRARY_NAME} SHARED
  src/svg_handler.cpp
  src/svg_reader.cpp
  src/string_utils.cpp
  src/tree_utils.cpp
  src/csv_exporter.cpp
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_NAME}
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)

# ========== CLI Executable ==========
add_executable(${PROGRAM_NAME} cli/main.cpp)
target_link_libraries(${PROGRAM_NAME} PRIVATE ${LIBRARY_NAME})
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cli
)

# ========== Tests ==========
add_executable(${PROGRAM_NAME}_test tests/test.cpp)
target_link_libraries(${PROGRAM_NAME}_test ${LIBRARY_NAME})
add_custom_target(test_run
    COMMAND ${PROGRAM_NAME}_test
    DEPENDS ${PROGRAM_NAME}_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_target_properties(${PROGRAM_NAME}_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
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
    set_target_properties(${EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
endforeach()

# ========== Qt6 GUI ==========
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
        ${LIBRARY_NAME}
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
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gui_qt
)

# ========== GTKmm4 GUI ==========
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)

set(GTKMM_SOURCES
    gui_gtkmm4/main.cpp
    gui_gtkmm4/mainwindow.cpp
    gui_gtkmm4/mainwindow.hpp
)

set(GTKMM_EXEC_NAME svg_handler_gtk)
add_executable(${GTKMM_EXEC_NAME} ${GTKMM_SOURCES})

target_include_directories(${GTKMM_EXEC_NAME} PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(${GTKMM_EXEC_NAME}
    PRIVATE
        ${LIBRARY_NAME}
        ${GTKMM_LIBRARIES}
)

target_compile_options(${GTKMM_EXEC_NAME} PRIVATE ${GTKMM_CFLAGS_OTHER})

add_custom_target(run_gtkmm
    COMMAND ${GTKMM_EXEC_NAME}
    DEPENDS ${GTKMM_EXEC_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(${GTKMM_EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gui_gtk
)

# ========== Python Interoperability ==========
option(BUILD_PYTHON_BINDINGS "Build Python shared library for ctypes or pybind11" ON)

if(BUILD_PYTHON_BINDINGS)
  message(STATUS "Building shared library for Python interoperability")

  # Shared library with C bindings
  add_library(svg_handler_python SHARED
    src/svg_handler.cpp
    src/svg_reader.cpp
    src/string_utils.cpp
    src/tree_utils.cpp
    src/csv_exporter.cpp
  )

  target_include_directories(svg_handler_python PRIVATE include)
  set_target_properties(svg_handler_python PROPERTIES
      OUTPUT_NAME "svg_handler_py"
      LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
  )
endif()

# ========== Copy Resources ==========
file(GLOB RESOURCE_FILES "resources/*")
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/cli/resources)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/tests/resources)

# ========== Default Build Type ==========
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type" FORCE)
endif()
