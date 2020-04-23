cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HunterGate)

huntergate(URL "https://github.com/cpp-pm/hunter/archive/v0.23.249.tar.gz" SHA1
           "d45d77d8bba9da13e9290a180e0477e90accd89b")

project(caf_tracing VERSION 1.0.0)

string(
  APPEND
  CMAKE_CXX_FLAGS
  " -Wall -Wcast-align -Wcast-qual -Wmissing-field-initializers -Wmissing-include-dirs -Wpointer-arith -Wstack-protector -Wstrict-aliasing=2 -Wunreachable-code -Wlogical-op -Wduplicated-cond -Wnull-dereference -Wno-unused-parameter"
)
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0")
string(APPEND CMAKE_CXX_FLAGS_RELEASE
       " -O3 -march=native -funroll-loops -flto -DNDEBUG")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Threads REQUIRED)

set(CAF_ENABLE_ACTOR_PROFILER
    ON
    CACHE BOOL "Enable experimental profiler API" FORCE)
set(CAF_ENABLE_EXAMPLES
    OFF
    CACHE BOOL "Build small programs showcasing CAF features" FORCE)
set(CAF_ENABLE_TESTING
    OFF
    CACHE BOOL "Build unit test suites" FORCE)
set(CAF_LOG_LEVEL
    "TRACE"
    CACHE STRING "Set log verbosity of CAF components" FORCE)
add_subdirectory(external/actor-framework)

set(BUILD_TESTING
    OFF
    CACHE BOOL "Build jaeger tests" FORCE)
set(JAEGERTRACING_BUILD_EXAMPLES
    OFF
    CACHE BOOL "Build examples" FORCE)
add_subdirectory(external/jaeger-client-cpp)

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(shared)
