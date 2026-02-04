#pragma once

#include <future>

#include "Core/Mesh.h"
#include "Core/Program.h"
#include "Core/Texture.h"
#include "Mat.h"
#include "Orbiter.h"
#include "PathFinder.h"

class AppLogic {
public:
    AppLogic();
    ~AppLogic();

    void Update();
    void Render() const;
    void UI();

private:
    std::unique_ptr<Orbiter> camera;

    Mesh terrainMesh, waterMesh, pathMesh;
    Program terrainProgram, waterProgram, lineProgram;
    Texture heightTex, normalTex;

    Mat<float> hm;
    Mat<glm::vec3> normals;

    std::future<PathFinder::Path> pendingJob;
    bool jobRunning = false;

    float heightScale = 15.f;
    float waterHeight = 1.0f;
    bool renderWater = false;
};
