cmake_minimum_required(VERSION 3.0.0)
project(mocker VERSION 0.1.0)

find_program(CCACHE ccache)
if (CCACHE)
  message(STATUS "Using ccache")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
  message(STATUS "Ccache not found")
endif ()

add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_17)
target_compile_options(project_options INTERFACE
-Wall -Wextra -Wpedantic -Werror -Wstrict-aliasing -Wuninitialized
  -Wunused-parameter -Wsign-compare -Wsign-conversion -Wunreachable-code
  -Wcast-align -Wno-implicit-fallthrough -Wno-uninitialized
  -Wno-missing-field-initializers -Wno-ignored-qualifiers -Wswitch-enum
  -Wredundant-decls -Wno-strict-overflow -Wshadow
  $<$<COMPILE_LANGUAGE:CXX>:
      -Woverloaded-virtual -Wreorder -Wzero-as-null-pointer-constant -Wnon-virtual-dtor>)

add_subdirectory(base)
add_subdirectory(3rd_party)

add_executable(mocker main.cpp
  mocker.cpp
  filesystem_browser.cpp
  image.cpp)

target_link_libraries(mocker PRIVATE project_options base 3rd_parties)