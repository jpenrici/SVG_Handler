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

# ========== PySide6 GUI (Python frontend) ===========
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(PYSIDE_SRC_DIR "${PROJECT_SOURCE_DIR}/python")
set(PYSIDE_STAGE_DIR "${CMAKE_BINARY_DIR}/gui_pyside6")
set(PYSIDE_LIB_DIR "${PYSIDE_STAGE_DIR}/lib")
set(PYSIDE_RES_DIR "${PYSIDE_STAGE_DIR}/resources")
set(PYSIDE_VENV_DIR "${PYSIDE_STAGE_DIR}/.venv")

if(WIN32)
    set(PYSIDE_VENV_PYTHON "${PYSIDE_VENV_DIR}/Scripts/python.exe")
else()
    set(PYSIDE_VENV_PYTHON "${PYSIDE_VENV_DIR}/bin/python3")
endif()

add_custom_target(copy_pyside ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PYSIDE_STAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PYSIDE_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PYSIDE_RES_DIR}

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/lib/libsvg_handler_py.so
        ${PYSIDE_LIB_DIR}/

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PYSIDE_RES_DIR}/

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PYSIDE_SRC_DIR}/cli_py3/svg_handler.py
        ${PYSIDE_STAGE_DIR}

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PYSIDE_SRC_DIR}/gui_pyside6/main.py
        ${PYSIDE_STAGE_DIR}

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PYSIDE_SRC_DIR}/gui_pyside6/mainwindow.py
        ${PYSIDE_STAGE_DIR}

    COMMENT "Preparing PySide6 runtime directory (gui_pyside6)..."
)

# --------- Create the virtual environment ---------
add_custom_target(create_pyside_venv
    COMMAND ${Python3_EXECUTABLE} -m venv ${PYSIDE_VENV_DIR}
    COMMENT "Creating Python virtual environment (.venv) for PySide6..."
)

# --------- Install PySide6 inside .venv ---------
add_custom_target(install_pyside6
    COMMAND ${PYSIDE_VENV_PYTHON} -m pip install --upgrade pip
    COMMAND ${PYSIDE_VENV_PYTHON} -m pip install PySide6
    DEPENDS create_pyside_venv
    COMMENT "Installing PySide6 inside the virtual environment..."
)

# --------- Auxiliary script for running the GUI ---------
set(PYSIDE_RUNNER_SCRIPT ${CMAKE_BINARY_DIR}/run_pyside.py)

file(WRITE ${PYSIDE_RUNNER_SCRIPT} "
import os
import sys
import subprocess

stage_dir = os.path.abspath(r\"${PYSIDE_STAGE_DIR}\")
venv_dir = os.path.abspath(r\"${PYSIDE_VENV_DIR}\")
lib_dir  = os.path.join(stage_dir, 'lib')

if os.name == 'nt':
    python_exec = os.path.join(venv_dir, 'Scripts', 'python.exe')
else:
    python_exec = os.path.join(venv_dir, 'bin', 'python3')

os.environ['LD_LIBRARY_PATH'] = lib_dir + os.pathsep + os.environ.get('LD_LIBRARY_PATH', '')

main_py = os.path.join(stage_dir, 'main.py')
if not os.path.exists(main_py):
    print(f'[ERROR] main.py not found at {main_py}')
    sys.exit(1)

print(f'[INFO] Running PySide6 GUI using virtual environment: {venv_dir}')
print(f'[INFO] Executing: {python_exec} {main_py}')

subprocess.run([python_exec, main_py], cwd=stage_dir)
")

# --------- Main execution target ---------
add_custom_target(run_pyside6
    COMMAND ${PYSIDE_VENV_PYTHON} ${PYSIDE_RUNNER_SCRIPT}
    DEPENDS copy_pyside install_pyside6
    WORKING_DIRECTORY ${PYSIDE_STAGE_DIR}
    COMMENT "Running PySide6 GUI inside virtual environment..."
)

# ========== Copy Resources ==========
file(GLOB RESOURCE_FILES "resources/*")
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/cli/resources)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/tests/resources)

# ========== Default Build Type ==========
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type" FORCE)
endif()

# ========== Custom Clean Target ==========
add_custom_target(deep_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT ">>> Completely cleaning the build directory: ${CMAKE_BINARY_DIR}"
)

# ========== Construction and execution information ==========
set(BUILD_DIR "${CMAKE_BINARY_DIR}")
set(BUILD_TYPE_LOWERCASE ${CMAKE_BUILD_TYPE})
string(TOLOWER ${BUILD_TYPE_LOWERCASE} BUILD_TYPE_LOWERCASE)

message(STATUS "")
message(STATUS "======================================================================")
message(STATUS "--- CMAKE CONFIGURATION SUCCESSFUL ---")
message(STATUS "Build Type set to: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "--- BUILD INSTRUCTIONS ---")

message(STATUS "1. To build using CMake (Recommended for cross-platform):")
message(STATUS "   cmake --build ${BUILD_DIR}")

if(CMAKE_GENERATOR MATCHES "Makefiles" OR CMAKE_GENERATOR MATCHES "Ninja")
    message(STATUS "")
    message(STATUS "2. Alternatively, use the native build command (Generator: ${CMAKE_GENERATOR}):")
    message(STATUS "   cd ${BUILD_DIR}")

    if(CMAKE_GENERATOR MATCHES "Makefiles")
        message(STATUS "   make")
    elseif(CMAKE_GENERATOR MATCHES "Ninja")
        message(STATUS "   ninja")
    endif()

    message(STATUS "   (Note: Use 'make clean' or 'ninja clean' for standard cleanup)")
endif()

message(STATUS "--------------------------")
message(STATUS "")
message(STATUS "--- RUN COMMANDS ---")

# 1. CLI Executable
message(STATUS "-> CLI Application (svg_handler_cli):")
message(STATUS "   Run: ${BUILD_DIR}/cli/${PROGRAM_NAME}")
message(STATUS "")

# 2. Qt6 GUI Executable
message(STATUS "-> Qt6 GUI (svg_handler_qt):")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target run_qtgui")
message(STATUS "")

# 3. GTKmm4 GUI Executable
message(STATUS "-> GTKmm4 GUI (svg_handler_gtk):")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target run_gtkmm")
message(STATUS "")

# 4. PySide6 GUI Executable
message(STATUS "-> PySide6 GUI (Python/PySide6):")
message(STATUS "   Setup & Run: cmake --build ${BUILD_DIR} --target run_pyside6")
message(STATUS "")

# 5. Tests
message(STATUS "-> Unit Tests:")
message(STATUS "   Run all main tests: cmake --build ${BUILD_DIR} --target test_run")
message(STATUS "")
message(STATUS "-> Internal Module Test [ string_utils ]:")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target test_string_utils")
message(STATUS "")
message(STATUS "-> Internal Module Test [ svg_reader ]:")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target test_svg_reader")
message(STATUS "")
message(STATUS "-> Internal Module Test [ tree_utils ]:")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target test_tree_utils")
message(STATUS "")
message(STATUS "-> Internal Module Test [ csv_exporter ]:")
message(STATUS "   Build & Run: cmake --build ${BUILD_DIR} --target test_csv_exporter")
message(STATUS "")

# 6. Clean
message(STATUS "-> Full Cleanup:")
message(STATUS "   Remove ALL build files/cache: cmake --build ${BUILD_DIR} --target deep_clean")

message(STATUS "======================================================================")
message(STATUS "")
