#pragma once

#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path);

    namespace GL {
        GLuint CreateProgram(const char* vsSrc, const char* fsSrc);
        GLuint CreateProgram(const char* src);

        enum class TextureFormat { Float1, Float3 };
        GLuint
        CreateTexture(int width, int height, TextureFormat format, const void* data = nullptr);

        template <typename T>
        void SetUniform(GLuint program, GLint location, const T& value) {
            // clang-format off
            if constexpr      (std::is_same_v<T, float>)     { glProgramUniform1f(program, location, value); }
            else if constexpr (std::is_same_v<T, int>)       { glProgramUniform1i(program, location, value); }
            else if constexpr (std::is_same_v<T, uint32_t>)  { glProgramUniform1ui(program, location, value); }
            else if constexpr (std::is_same_v<T, glm::vec2>) { glProgramUniform2fv(program, location, 1, &value[0]); }
            else if constexpr (std::is_same_v<T, glm::vec3>) { glProgramUniform3fv(program, location, 1, &value[0]); }
            else if constexpr (std::is_same_v<T, glm::vec4>) { glProgramUniform4fv(program, location, 1, &value[0]); }
            else if constexpr (std::is_same_v<T, glm::mat4>) { glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, glm::value_ptr(value)); }
            else { static_assert(false, "Unsupported uniform type"); }
            // clang-format on
        }

        void GLAPIENTRY OpenGLMessageCallback(GLenum source,
                                              GLenum type,
                                              GLuint id,
                                              GLenum severity,
                                              GLsizei length,
                                              const GLchar* message,
                                              const void* userParam);

    } // namespace GL
} // namespace Utils
