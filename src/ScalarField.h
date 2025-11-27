#pragma once

#include <vector>
#include <functional>
#include <filesystem>

#include <glm/glm.hpp>
#include <stb/stb_image.h>

struct Image {
    uint32_t width, height;
    std::vector<uint8_t> data;

    bool Load(const std::filesystem::path& path) {
        int w, h, bpp;
        stbi_uc* buf = stbi_load(path.string().c_str(), &w, &h, &bpp, STBI_grey);

        width = w;
        height = h;
        data.resize(w * h);
        memcpy(data.data(), buf, w * h);

        if (!buf)
            return false;

        stbi_image_free(buf);
        return true;
    }
};

struct Triangulation {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> indices;

    bool SaveOBJ(const std::filesystem::path& path);
};

class ScalarField {
public:
    ScalarField(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max);
    ScalarField(const Image& img, const glm::vec2& min, const glm::vec2& max, float maxHeight);
    ScalarField(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max,
                const std::function<float(const glm::vec2&)>& f);

    Triangulation Triangulate() const;

private:
    uint32_t Index(uint32_t x, uint32_t y) const;
    glm::vec2 Point(uint32_t x, uint32_t y) const;

private:
    glm::uvec2 size;
    glm::vec2 min, max, delta{};
    std::vector<float> heights;
};
