# OBJECT LIBRARY
set(LIB_NAME shared_library)

set(LIB_HEADERS
    include/tracer/get.hpp
    include/tracer/put.hpp
    include/actor_profiler.hpp
    include/actor_system_config.hpp
    include/aprint.hpp
    include/create_span.hpp
    include/create_tracer.hpp
    include/error.hpp
    include/get_tracing_data.hpp
    include/hostname.hpp
    include/ip_address.hpp
    include/message.hpp
    include/server_actor_type.hpp
    include/source_location.hpp
    include/span_context.hpp
    include/tracing_data.hpp
    include/tracing_data_factory.hpp
    include/types.hpp)

set(LIB_SOURCES
    src/tracer/get.cpp
    src/tracer/put.cpp
    src/actor_profiler.cpp
    src/actor_system_config.cpp
    src/create_span.cpp
    src/create_tracer.cpp
    src/error.cpp
    src/hostname.cpp
    src/ip_address.cpp
    src/span_context.cpp
    src/tracing_data.cpp
    src/tracing_data_factory.cpp)

add_library(${LIB_NAME} OBJECT ${LIB_HEADERS} ${LIB_SOURCES})

target_link_libraries(
  ${LIB_NAME} PUBLIC Threads::Threads jaegertracing ${CMAKE_DL_LIBS}
                     ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO})

target_include_directories(
  ${LIB_NAME}
  PUBLIC ${CAF_INCLUDE_DIRS}
         ${caf_tracing_SOURCE_DIR}/shared/include
         ${caf_tracing_SOURCE_DIR}/external/expected/include
         ${caf_tracing_SOURCE_DIR}/external/optional/include
         ${caf_tracing_SOURCE_DIR}/external/fmtlib/include
         ${caf_tracing_SOURCE_DIR}/external/gsl/include
         ${caf_tracing_SOURCE_DIR}/external/philslib/include)

target_compile_definitions(${LIB_NAME} PUBLIC CAF_ENABLE_ACTOR_PROFILER
                                              FMT_HEADER_ONLY __USE_POSIX)
