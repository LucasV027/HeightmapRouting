#include "Program.h"

#include <iostream>
#include <vector>

#include "Utils.h"

Program::Program(const char* csSrc) {
    GLuint cs = CompileShader(GL_COMPUTE_SHADER, csSrc);
    handle = LinkProgram({cs});
    glDeleteShader(cs);
    QueryUniforms();
}

Program::Program(const char* vsSrc, const char* fsSrc) {
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc);
    handle = LinkProgram({vs, fs});
    glDeleteShader(vs);
    glDeleteShader(fs);
    QueryUniforms();
}

Program::~Program() {
    Cleanup();
}

Program::Program(Program&& other) noexcept :
    uniforms(std::move(other.uniforms)), handle(other.handle) {
    other.handle = GL_NONE;
}

Program& Program::operator=(Program&& other) noexcept {
    if (this != &other) {
        Cleanup();

        uniforms = std::move(other.uniforms);
        handle = other.handle;

        other.handle = GL_NONE;
    }
    return *this;
}

void Program::Bind() const {
    glUseProgram(handle);
}

void Program::Unbind() const {
    glUseProgram(0);
}

Program Program::FromFile(const std::filesystem::path& csPath) {
    const auto csSrc = Utils::ReadFile(csPath);
    return Program(csSrc.c_str());
}

Program Program::FromFile(const std::filesystem::path& vsPath,
                          const std::filesystem::path& fsPath) {
    const auto vsSrc = Utils::ReadFile(vsPath);
    const auto fsSrc = Utils::ReadFile(fsPath);
    return {vsSrc.c_str(), fsSrc.c_str()};
}

void Program::Cleanup() {
    if (handle != GL_NONE) {
        glDeleteProgram(handle);
        handle = GL_NONE;
    }
    uniforms.clear();
}

void Program::QueryUniforms() {
    GLint count;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);

    GLint maxLength;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

    std::vector<GLchar> name(maxLength);

    for (GLint i = 0; i < count; ++i) {
        UniformInfo info{};
        glGetActiveUniform(handle, i, maxLength, nullptr, &info.count, &info.type, name.data());
        info.location = glGetUniformLocation(handle, name.data());

        uniforms[std::string(name.data())] = info;
    }
}

GLint Program::GetUniformLocation(const std::string& name) {
    if (const auto it = uniforms.find(name); it != uniforms.end())
        return it->second.location;

    std::cerr << "Warning: Uniform '" << name << "' not found in shader program\n";
    return -1;
}

GLuint Program::CompileShader(const GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log.data() << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Program::LinkProgram(const std::vector<GLuint>& shaders) {
    const GLuint program = glCreateProgram();

    for (const GLuint shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Program linking failed:\n" << log.data() << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}
