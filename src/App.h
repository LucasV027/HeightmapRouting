#pragma once

#include <memory>

#include "Orbiter.h"
#include "Window.h"

class App {
public:
    App();
    ~App();

    void Run();

private:
    static void InitOpenGL();

    void InitImGui() const;
    static void BeginUI();
    static void EndUI();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Orbiter> camera;

    GLuint program;
    GLuint heightTex;
    GLuint vao, vboPos, vboUV, ibo;
    uint32_t count;
};
