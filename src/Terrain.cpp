#include "Terrain.h"

#include "Algorithm.h"


Terrain Terrain::Load(const std::filesystem::path& heightPath,
                      const std::filesystem::path& typePath,
                      const glm::vec2 worldSize,
                      const float heightScale,
                      const float waterHeight,
                      const glm::vec3& origin) {
    const auto heightData = Image::FromFile(heightPath, Image::Format::I);
    const auto typeData = Image::FromFile(typePath, Image::Format::I);
    if (!heightData.has_value() || !typeData.has_value())
        throw std::runtime_error("Terrain::Load() - Failed to load image");

    if (heightData->width != typeData->width || heightData->height != typeData->height)
        throw std::runtime_error("Terrain::Load() - Incoherent sizes");

    Terrain ret;
    ret.heightMap = Mat<float>(*heightData);
    ret.typeMap = Mat<TileType>(ret.heightMap.Size(), TileType::NORMAL);

    for (int y = 0; y < ret.heightMap.Size().x; y++) {
        for (int x = 0; x < ret.heightMap.Size().x; x++) {
            if (ret.heightMap(x, y) * heightScale <= waterHeight) {
                ret.typeMap(x, y) = TileType::WATER;

                // TODO: value map for each tile (for now 0 (black) = forest)
            } else if ((*typeData)(x, y) == 0) {
                ret.typeMap(x, y) = TileType::FOREST;
            }
        }
    }

    ret.dimensions = ret.heightMap.Size();
    ret.origin = origin;
    ret.worldSize = worldSize;

    ret.heightScale = heightScale;
    ret.waterHeight = waterHeight;
    return ret;
}

float Terrain::CellSizeX() const {
    return worldSize.x / static_cast<float>(dimensions.x - 1);
}

float Terrain::CellSizeZ() const {
    return worldSize.y / static_cast<float>(dimensions.y - 1);
}

glm::vec3 Terrain::GridToWorld(const int x, const int y) const {
    return {
        origin.x + x * CellSizeX(),
        heightMap(x, y) * heightScale,
        origin.z + y * CellSizeZ(),
    };
}

glm::vec3 Terrain::GridToWorldAboveWater(const int x, const int y) const {
    auto p = GridToWorld(x, y);
    p.y = std::max(p.y, waterHeight);
    return p;
}
