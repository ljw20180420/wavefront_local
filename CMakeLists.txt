cmake_minimum_required(VERSION 3.15...4.2)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

file(GLOB CPP_SOURCES CONFIGURE_DEPENDS "cpp/src/*.cpp")

pybind11_add_module(
    _core MODULE
    src/wavefront_local/bindings.cpp
    ${CPP_SOURCES}
)
target_include_directories(_core PRIVATE cpp/src)

install(TARGETS _core DESTINATION ${SKBUILD_PROJECT_NAME})
