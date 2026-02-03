#pragma once

#include <memory>

#include "Mesh.h"
#include "Orbiter.h"
#include "Program.h"
#include "Texture.h"
#include "Window.h"

class App {
public:
    App();
    ~App();

    void Run();

    static App& Get();
    static Window& GetWindow();

private:
    static void InitOpenGL();

    void InitImGui() const;
    static void BeginUI();
    static void EndUI();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Orbiter> camera;

    Mesh terrainMesh, waterMesh, pathMesh;
    Program terrainProgram, waterProgram, lineProgram;
    Texture heightTex, normalTex;

    Mat<float> hm;
    Mat<glm::vec3> normals;

    float heightScale = 15.f;
    float waterHeight = 1.0f;
};
