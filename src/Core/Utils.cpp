#include "Utils.h"

#include <fstream>
#include <iostream>

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path) {
        std::ifstream in(path);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    }
} // namespace Utils

namespace Utils::GL {
    void OpenGLMessageCallback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam) {

        // clang-format off
        const char* severityStr = [severity]() {
            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "INFO";
            default: return "UNKNOWN";
            }
        }();

        const char* typeStr = [type]() {
            switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "UNKNOWN";
            }
        }();
        // clang-format on

        auto& stream = (severity == GL_DEBUG_SEVERITY_NOTIFICATION) ? std::cout : std::cerr;
        stream << "[OpenGL " << severityStr << " " << typeStr << "] " << message << std::endl;
    }
} // namespace Utils::GL
