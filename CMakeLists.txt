cmake_minimum_required(VERSION 3.10)
project(TerrainDescriptors)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
list(APPEND SOURCES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\")

add_subdirectory(vendor)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm stb glfw glad imgui tinyobjloader)
