#pragma once

#include <filesystem>

#include "Window.h"

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path);

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
