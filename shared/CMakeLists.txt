find_package(Threads REQUIRED)

### OBJECT LIBRARY
set(LIB_NAME e3SharedLib)

set(LIB_HEADERS
    include/ip_address.hpp
    include/atoms.hpp
    include/aprint.hpp
    include/client_actor_type.hpp
    include/server_actor_type.hpp)
    
set(LIB_SOURCES
    src/ip_address.cpp)

add_library(
    ${LIB_NAME}
    OBJECT
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    Threads::Threads
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    fmt::fmt
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO}
)

target_include_directories(
    ${LIB_NAME}
    PUBLIC
    ${CAF_INCLUDE_DIRS}
)

target_include_directories(
    ${LIB_NAME}
    PUBLIC
    ${exercise3_SOURCE_DIR}/shared/include
)