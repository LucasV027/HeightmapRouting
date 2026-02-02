#include "App.h"

#include <iostream>

#include "Curve.h"
#include "ImCurve.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Image.h"
#include "Mat.h"
#include "Orbiter.h"
#include "Utils.h"
#include "Window.h"

static App* instance = nullptr;

App::App() {
    if (instance)
        throw std::runtime_error("App already initialized");
    instance = this;

    window = std::make_unique<Window>(1280, 720, "MMV");
    camera = std::make_unique<Orbiter>(glm::vec3(0.0f), 200.f, 1280.f / 720.f);

    InitOpenGL();
    InitImGui();

    const auto img = Image::FromFile(DATA_DIR "Terrain/AlpsMontBlanc.png", Image::Format::I);
    if (!img.has_value()) {
        throw std::runtime_error("Failed to load image");
    }

    const Mat<float> hm(*img);

    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    program = Program::FromFile(DATA_DIR "Shaders/Main.vert", DATA_DIR "Shaders/Main.frag");
    heightTex = Texture::From(hm);
    mesh = Mesh::PlanarGrid(hm.Size(), glm::vec2{-50.0f, -50.0f}, glm::vec2{50.f, 50.f});
    waterMesh = Mesh::PlanarGrid({2u, 2u}, glm::vec2{-50.0f, -50.0f}, glm::vec2{50.0f, 50.0f});
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    instance = nullptr;
}

void App::Run() {
    while (!window->ShouldClose()) {
        // Update
        {
            camera->Update(0.01f);

            auto vp = camera->Proj() * camera->View();
            program.SetUniform("uVP", vp);
            program.SetUniform("uHeightScale", scale);
            waterProgram.SetUniform("uVP", vp);
            waterProgram.SetUniform("uHeight", waterHeight);
        }

        // Render
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            program.Bind();
            heightTex.Bind();
            mesh.Draw();
            program.Unbind();

            waterProgram.Bind();
            waterMesh.Draw();
            waterProgram.Unbind();
        }

        // UI
        {
            BeginUI();
            ImGui::Text("%d FPS", (int)ImGui::GetIO().Framerate);
            camera->UI();
            ImGui::SliderFloat("Scale", &scale, 0.1f, 1000.0f);
            ImGui::SliderFloat("Water", &waterHeight, 0.0f, 100.0f);

            static Curve curve1(Curve::Interpolation::LINEAR);
            static Curve curve2(Curve::Interpolation::COSINUS);
            ImGui::CurveEditor("Curve1", curve1);
            ImGui::CurveEditor("Curve2", curve2);

            EndUI();
        }

        window->Swap();
        window->Poll();
    }
}

App& App::Get() {
    return *instance;
}

Window& App::GetWindow() {
    return *instance->window;
}

void App::InitOpenGL() {
#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Utils::GL::OpenGLMessageCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr,
                          GL_FALSE);
#endif

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void App::InitImGui() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    ImGui::GetStyle().Alpha = 0.8f;

    ImGui_ImplGlfw_InitForOpenGL(window->Handle(), true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
}

void App::BeginUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void App::EndUI() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
