#include "Utils.h"

#include <fstream>
#include <iostream>

#include <tiny_obj_loader.h>

namespace Utils {
    std::string ReadFile(const std::filesystem::path& path) {
        std::ifstream in(path);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    }

    Model Model::FromFile(const std::filesystem::path& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!LoadObj(&attrib, &shapes, &materials, &err, path.string().c_str())) {
            throw std::runtime_error("Utils::MeshFromFile - Failed to load OBJ: " + err);
        }

        Model ret;
        int c = 0;
        if (!attrib.vertices.empty()) {
            ret.attribs |= Attribs::POSITION;
            c += 3;
        }
        if (!attrib.normals.empty()) {
            ret.attribs |= Attribs::NORMAL;
            c += 3;
        }
        if (!attrib.texcoords.empty()) {
            ret.attribs |= Attribs::TEXCOORD;
            c += 2;
        }

        ret.vertices.reserve(attrib.vertices.size() * c);

        for (const auto& [name, mesh] : shapes) {
            for (const auto& [vIdx, nIdx, uvIdx] : mesh.indices) {
                // Position
                if (ret.attribs & Attribs::POSITION) {
                    ret.vertices.push_back(attrib.vertices[3 * vIdx + 0]);
                    ret.vertices.push_back(attrib.vertices[3 * vIdx + 1]);
                    ret.vertices.push_back(attrib.vertices[3 * vIdx + 2]);
                }

                // Normal
                if (ret.attribs & Attribs::NORMAL) {
                    ret.vertices.push_back(attrib.normals[3 * nIdx + 0]);
                    ret.vertices.push_back(attrib.normals[3 * nIdx + 1]);
                    ret.vertices.push_back(attrib.normals[3 * nIdx + 2]);
                }

                // UV
                if (ret.attribs & Attribs::TEXCOORD) {
                    ret.vertices.push_back(attrib.texcoords[2 * uvIdx + 0]);
                    ret.vertices.push_back(attrib.texcoords[2 * uvIdx + 1]);
                }

                ret.indices.push_back((ret.vertices.size() - 1) / c);
            }
        }
        return ret;
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
