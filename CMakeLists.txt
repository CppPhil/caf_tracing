cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HunterGate)

HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.249.tar.gz"
    SHA1 "d45d77d8bba9da13e9290a180e0477e90accd89b"
)

project(caf_tracing VERSION 1.0.0)

set(CAF_URL https://github.com/actor-framework/actor-framework/archive/0.17.4.tar.gz)

set(TAR_PATH ${caf_tracing_SOURCE_DIR}/external/actor-framework-0.17.4.tar.gz)

file(
  DOWNLOAD ${CAF_URL}
  ${TAR_PATH}
  STATUS status
)

list(GET status 0 status_code)

if (NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading '${CAF_URL}' failed")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar xfz ${TAR_PATH}
  WORKING_DIRECTORY ${caf_tracing_SOURCE_DIR}/external
  RESULT_VARIABLE rv
)

if (NOT rv EQUAL 0)
  message(FATAL_ERROR "error: could not extract actor-framework-0.17.4.tar.gz!")
endif()

execute_process(
  COMMAND bash ${caf_tracing_SOURCE_DIR}/scripts/build_caf.sh
)

set(CAF_ROOT_DIR ${caf_tracing_SOURCE_DIR}/build/caf)
find_package(CAF COMPONENTS core io test)

string(APPEND CMAKE_CXX_FLAGS " -Werror -Wall -Wextra -Wcast-align -Wcast-qual -Wmissing-field-initializers -Wmissing-include-dirs -Wpointer-arith -Wstack-protector -Wstrict-aliasing=2 -Wunreachable-code -Wlogical-op -Wduplicated-cond -Wnull-dereference -Wno-unused-parameter")
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native -funroll-loops -flto -fwhole-program -DNDEBUG")

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

find_package(Threads REQUIRED)

set(BUILD_TESTING false)
set(JAEGERTRACING_BUILD_EXAMPLES false)
add_subdirectory(external/jaeger-client-cpp)

add_subdirectory(external/fmtlib)

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)

