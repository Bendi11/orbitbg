cmake_minimum_required(VERSION 3.4)

project(
    orbitbg
    LANGUAGES C
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(
    SRC
    "src/main.c"
    "src/render.c"
    "src/gfx.c"
    "src/orbit.c"
)

find_package(Cairo REQUIRED)
find_package(X11 REQUIRED)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Building in ${CMAKE_BUILD_TYPE} mode")

target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_TYPE="${CMAKE_BUILD_TYPE}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CAIRO_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CAIRO_LIBRARIES} ${X11_LIBRARIES} m)
