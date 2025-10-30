cmake_minimum_required(VERSION 3.16)
project(AdvancedCSVContactManager C)

set(CMAKE_C_STANDARD 11)

# --- Your executable ---
add_executable(AdvancedCSVContactManager
        ContactManager.c
        main.c
        Validation.c
        FileIO.c
        Sorting.c
        StringHandaling.c
        WebServer.c
        Contact.c
)

# --- CivetWeb include and library ---
include_directories(${CMAKE_SOURCE_DIR}/CivetWeb/include)

add_library(civetweb STATIC ${CMAKE_SOURCE_DIR}/CivetWeb/src/civetweb.c)

# Disable SSL
target_compile_definitions(civetweb PRIVATE NO_SSL)

# Link the CivetWeb static library to your main executable
target_link_libraries(AdvancedCSVContactManager PRIVATE civetweb ws2_32)








