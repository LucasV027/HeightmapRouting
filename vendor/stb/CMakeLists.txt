add_library(stb
        stb/stb_image_write.h
        stb/stb_image_write.cpp
        stb/stb_image.h
        stb/stb_image.cpp
)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})