cmake_minimum_required(VERSION 3.16)
project(spectogram)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RESOURCE_PATH_DEST ${CMAKE_CURRENT_BINARY_DIR}/resources)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(dependencies)

add_executable(spectogram 
src/main.cpp)

target_include_directories(spectogram PRIVATE include)
target_link_libraries(spectogram PRIVATE rreav)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_SOURCE_DIR}/resources
    ${RESOURCE_PATH_DEST}
    DEPENDS ${MY_TARGET}
)