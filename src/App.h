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

    Mesh mesh;
    Program program;
    Texture heightTex;

    float scale = 15.f;
};
