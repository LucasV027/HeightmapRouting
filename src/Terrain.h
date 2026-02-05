#pragma once

#include "Mat.h"

struct Terrain {
    enum class TileType : uint8_t { NORMAL = 0, WATER = 1, FOREST = 2 };

    Mat<float> heightMap;
    Mat<uint8_t> typeMap;

    glm::ivec2 dimensions;
    glm::vec3 origin;
    glm::vec2 worldSize;

    float heightScale;
    float waterHeight;

    static Terrain Load(const std::filesystem::path& heightPath,
                        const std::filesystem::path& typePath,
                        glm::vec2 worldSize,
                        float heightScale,
                        float waterHeight = -1.f,
                        const glm::vec3& origin = glm::vec3(0.f));

    float CellSizeX() const;
    float CellSizeZ() const;

    glm::vec3 GridToWorld(int x, int y) const;
    glm::vec3 GridToWorldAboveWater(int x, int y) const;
};
