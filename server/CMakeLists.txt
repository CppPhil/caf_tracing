### OBJECT LIBRARY
set(LIB_NAME server_library)

set(LIB_HEADERS
    include/server_chat_actor.hpp
    include/participant.hpp)

set(LIB_SOURCES
    src/server_chat_actor.cpp
    src/participant.cpp)

add_library(
    ${LIB_NAME}
    OBJECT
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    shared_library
)

target_include_directories(
    ${LIB_NAME}
    PUBLIC
    ${caf_tracing_SOURCE_DIR}/server/include
)

### APPLICATION
set(APP_NAME server_application)

add_executable(
    ${APP_NAME}
    src/main.cpp
) 
               
target_link_libraries(
    ${APP_NAME}
    PRIVATE
    ${LIB_NAME}
    shared_library
)
