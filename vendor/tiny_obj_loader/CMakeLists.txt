# https://github.com/tinyobjloader/tinyobjloader/releases/tag/v1.0.6
add_library(tinyobjloader tiny_obj_loader.h tiny_obj_loader.cpp)
target_include_directories(tinyobjloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})