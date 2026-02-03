#pragma once

#include <memory>

#include "AppLogic.h"
#include "Window.h"

class App {
public:
    App();
    ~App();

    void Run() const;

    static App& Get();
    static Window& GetWindow();

private:
    static void InitOpenGL();
    void InitImGui() const;

    static void BeginUI();
    static void EndUI();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<AppLogic> appLogic;
};
