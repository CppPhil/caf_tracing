# OBJECT LIBRARY
set(LIB_NAME shared_library)

set(LIB_HEADERS
    include/aprint.hpp
    include/atoms.hpp
    include/client_actor_type.hpp
    include/create_span.hpp
    include/error.hpp
    include/extract.hpp
    include/inject.hpp
    include/ip_address.hpp
    include/server_actor_type.hpp
    include/setup_tracer.hpp
    include/source_location.hpp)

set(LIB_SOURCES src/create_span.cpp src/error.cpp src/extract.cpp
                src/inject.cpp src/ip_address.cpp src/setup_tracer.cpp)

add_library(${LIB_NAME} OBJECT ${LIB_HEADERS} ${LIB_SOURCES})

target_link_libraries(
  ${LIB_NAME} PUBLIC Threads::Threads ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE}
                     ${CAF_LIBRARY_IO} jaegertracing GSL)

target_include_directories(
  ${LIB_NAME}
  PUBLIC ${CAF_INCLUDE_DIRS} ${caf_tracing_SOURCE_DIR}/shared/include
         ${caf_tracing_SOURCE_DIR}/external/expected/include
         ${caf_tracing_SOURCE_DIR}/external/fmtlib/include)

target_compile_definitions(${LIB_NAME} PUBLIC CAF_ENABLE_ACTOR_PROFILER
                                              FMT_HEADER_ONLY)
