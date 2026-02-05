#pragma once

#include <future>

#include "Core/Mesh.h"
#include "Core/Program.h"
#include "Core/Texture.h"
#include "Core/Camera/Camera.h"
#include "Mat.h"
#include "PathFinder.h"

struct TerrainSpace {
    glm::vec2 worldSize;
    float heightScale;
    glm::vec3 origin;
    glm::ivec2 dim;

    float CellSizeX() const { return worldSize.x / static_cast<float>(dim.x - 1); }
    float CellSizeZ() const { return worldSize.y / static_cast<float>(dim.y - 1); }

    glm::vec2 GridToWorldXZ(const int i, const int j) const {
        return {origin.x + i * CellSizeX(), origin.z + j * CellSizeZ()};
    }

    float HeightToWorld(const float h) const { return origin.y + h * heightScale; }

    glm::vec3 GridToWorld(const int i, const int j, const float h) const {
        auto xz = GridToWorldXZ(i, j);
        return {xz.x, HeightToWorld(h), xz.y};
    }
};

class AppLogic {
public:
    AppLogic();
    ~AppLogic();

    void Update(float dt);
    void Render();
    void UI();

private:
    void UpdateFlagTransforms();

private:
    std::unique_ptr<Camera> camera;

    Mesh terrainMesh, waterMesh, pathMesh, flagMesh;
    Program terrainProgram, waterProgram, lineProgram, flagProgram;
    Texture heightTex, normalTex;

    glm::ivec2 start = {20, 20};
    glm::ivec2 end = {300, 300};
    // 0 = start | 1 = end
    glm::mat4 flagsModels[2];

    TerrainSpace terrain;

    Mat<float> hm;
    Mat<glm::vec3> normals;

    std::future<PathFinder::Path> pendingJob;
    bool jobRunning = false;

    float heightScale = 15.f;
    float waterHeight = 1.0f;

    bool renderWater = false;
};
