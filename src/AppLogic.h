#pragma once

#include <future>

#include "Core/Camera/Camera.h"
#include "Core/Mesh.h"
#include "Core/Program.h"
#include "Core/Texture.h"
#include "Core/Transform.h"
#include "PathFinder.h"
#include "Terrain.h"

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
    Texture heightTex, normalTex, typeTex;

    // Flags
    glm::ivec2 start = {20, 20};
    glm::ivec2 end = {500, 300};
    enum { FLAG_START = 0, FLAG_END = 1 };
    std::array<Transform, 2> flagTransforms;
    std::array<glm::vec3, 2> flagColors = {
        glm::vec3{0.8f, 0.0f, 0.0f},
        glm::vec3{0.0f, 0.0f, 0.8f},
    };

    // Terrain
    Terrain terrain;

    // Path find
    std::future<PathFinder::Path> pendingJob;
    bool jobRunning = false;
    PathFinder::Path path;
    bool allowBridges = false;
};
