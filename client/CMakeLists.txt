# OBJECT LIBRARY
set(LIB_NAME client_library)

set(LIB_HEADERS include/client_actor.hpp include/shutdown.hpp)

set(LIB_SOURCES src/client_actor.cpp src/shutdown.cpp)

add_library(${LIB_NAME} OBJECT ${LIB_HEADERS} ${LIB_SOURCES})

target_link_libraries(${LIB_NAME} PUBLIC shared_library)

target_include_directories(${LIB_NAME}
                           PUBLIC ${caf_tracing_SOURCE_DIR}/client/include)

# APPLICATION
set(APP_NAME client_application)

add_executable(${APP_NAME} src/main.cpp)

target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME} shared_library)
