#include "Utils.h"

#include <format>
#include <fstream>
#include <iostream>
#include <utility>

#define EXIT(fmt, ...)                                             \
    do {                                                           \
        std::string message = std::format(fmt, ##__VA_ARGS__);     \
        std::cerr << std::format("[ERROR]:\n{}", message) << "\n"; \
        std::abort();                                              \
    } while (false)


void CheckCompilation(const GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint infoLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

        std::string logs;
        logs.resize(infoLength);

        glGetShaderInfoLog(shader, infoLength, nullptr, logs.data());
        EXIT("Shader compilation failed\n{}", logs);
    }
}

void CheckLinking(const GLuint prog) {
    glValidateProgram(prog);
    int success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        GLint infoLength;
        glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &infoLength);

        std::string logs;
        logs.resize(infoLength);

        glGetProgramInfoLog(prog, 512, nullptr, logs.data());
        EXIT("Program linking failed\n{}", logs);
    }
}

GLuint CreateShader(const GLenum type, const char* src) {
    const GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    CheckCompilation(s);
    return s;
}

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path) {
        std::ifstream in(path);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    }
} // namespace Utils

namespace Utils::GL {
    GLuint CreateProgram(const char* vsSrc, const char* fsSrc) {

        const GLuint p = glCreateProgram();


        const GLuint vs = CreateShader(GL_VERTEX_SHADER, vsSrc);
        const GLuint fs = CreateShader(GL_FRAGMENT_SHADER, fsSrc);
        glAttachShader(p, vs);
        glAttachShader(p, fs);

        glLinkProgram(p);
        CheckLinking(p);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return p;
    }

    GLuint CreateProgram(const char* src) {
        const GLuint p = glCreateProgram();

        const GLuint cs = CreateShader(GL_COMPUTE_SHADER, src);
        glAttachShader(p, cs);

        glLinkProgram(p);

        glDeleteShader(cs);

        return p;
    }

    GLuint
    CreateTexture(const int width, const int height, const TextureFormat format, const void* data) {
        GLenum glInternal, glFormat, glType;
        switch (format) {
        case TextureFormat::Float1:
            glInternal = GL_R16F;
            glFormat = GL_RED;
            glType = GL_FLOAT;
            break;
        case TextureFormat::Float3:
            glInternal = GL_RGB32F;
            glFormat = GL_RGB;
            glType = GL_FLOAT;
            break;
        }

        GLuint texture;

        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, glInternal, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, glFormat, glType, data);

        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return texture;
    }

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
