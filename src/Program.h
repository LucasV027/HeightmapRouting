#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Window.h"

class Program {
public:
    Program() = default;
    ~Program();

    // Delete copy, allow move (Rule of Five)
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    Program(Program&& other) noexcept;
    Program& operator=(Program&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    GLuint Handle() const { return handle; }

    static Program FromFile(const std::filesystem::path& csPath);
    static Program FromFile(const std::filesystem::path& vsPath,
                            const std::filesystem::path& fsPath);

    template <typename T>
    void SetUniform(const std::string& name, const T& value) {
        int location = GetUniformLocation(name);

        // clang-format off
        if constexpr (std::is_same_v<T, float>)          { glProgramUniform1f(handle, location, value); }
        else if constexpr (std::is_same_v<T, int>)       { glProgramUniform1i(handle, location, value); }
        else if constexpr (std::is_same_v<T, uint32_t>)  { glProgramUniform1ui(handle, location, value); }
        else if constexpr (std::is_same_v<T, glm::vec2>) { glProgramUniform2fv(handle, location, 1, &value[0]); }
        else if constexpr (std::is_same_v<T, glm::vec3>) { glProgramUniform3fv(handle, location, 1, &value[0]); }
        else if constexpr (std::is_same_v<T, glm::vec4>) { glProgramUniform4fv(handle, location, 1, &value[0]); }
        else if constexpr (std::is_same_v<T, glm::mat4>) { glProgramUniformMatrix4fv(handle, location, 1, GL_FALSE, &value[0][0]); }
        else { static_assert(false, "Unsupported uniform type"); }
        // clang-format on
    }

private:
    explicit Program(const char* csSrc);
    Program(const char* vsSrc, const char* fsSrc);

    void Cleanup();
    void QueryUniforms();
    GLint GetUniformLocation(const std::string& name);

    static GLuint CompileShader(GLenum type, const char* source);
    static GLuint LinkProgram(const std::vector<GLuint>& shaders);

private:
    struct UniformInfo {
        GLint location;
        GLenum type;
        GLsizei count;
    };

    std::unordered_map<std::string, UniformInfo> uniforms;
    GLuint handle = GL_NONE;
};
