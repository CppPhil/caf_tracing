### OBJECT LIBRARY
set(LIB_NAME e3ClientLib)

set(LIB_HEADERS
    include/client_actor.hpp
    include/shutdown.hpp)

set(LIB_SOURCES
    src/client_actor.cpp
    src/shutdown.cpp)

add_library(
    ${LIB_NAME}
    OBJECT
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    e3SharedLib
)

target_include_directories(
    ${LIB_NAME}
    PUBLIC
    ${exercise3_SOURCE_DIR}/client/include
)

### APPLICATION
set(APP_NAME e3ClientApp)

add_executable(
    ${APP_NAME}
    src/main.cpp
) 
               
target_link_libraries(
    ${APP_NAME}
    PRIVATE
    ${LIB_NAME}
)
