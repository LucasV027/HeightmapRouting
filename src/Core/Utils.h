#pragma once

#include <filesystem>

#include "Window.h"

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path);

    struct Model {
        enum Attribs {
            POSITION = 1 << 1,
            NORMAL =   1 << 2,
            TEXCOORD = 1 << 3,
        };

        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        uint8_t attribs = 0;

        static Model FromFile(const std::filesystem::path& path);
    };

    namespace GL {
        void GLAPIENTRY OpenGLMessageCallback(GLenum source,
                                              GLenum type,
                                              GLuint id,
                                              GLenum severity,
                                              GLsizei length,
                                              const GLchar* message,
                                              const void* userParam);

    } // namespace GL
} // namespace Utils
