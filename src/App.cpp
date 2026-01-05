#include "App.h"

#include <iostream>

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

    const auto img = Image::FromFile(DATA_DIR "Terrain/alps-montblanc.png");
    if (!img.has_value()) {
        throw std::runtime_error("Failed to load image");
    }

    Mat<float> hm(*img);

    const auto vertSrc = Utils::ReadFile(DATA_DIR "Shaders/Main.vert");
    const auto fragSrc = Utils::ReadFile(DATA_DIR "Shaders/Main.frag");
    program = Utils::GL::CreateProgram(vertSrc.c_str(), fragSrc.c_str());

    heightTex = Utils::GL::CreateTexture(hm.Width(), hm.Height(), Utils::GL::TextureFormat::Float1,
                                         hm.Data());

    mesh = Mesh::PlanarGrid(hm.Size(), glm::vec2{-50.0f, -50.0f}, glm::vec2{50.f, 50.f});
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    instance = nullptr;
}

void App::Run() {
    while (!window->ShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto vp = camera->Proj() * camera->View();

        static int uVPLocation = glGetUniformLocation(program, "uVP");
        static int uHeightScaleLocation = glGetUniformLocation(program, "uHeightScale");

        Utils::GL::SetUniform(program, uVPLocation, vp);
        Utils::GL::SetUniform(program, uHeightScaleLocation, scale);

        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightTex);

        mesh.Draw();

        glUseProgram(0);

        camera->Update(0.01f);

        BeginUI();

        camera->UI();

        ImGui::SliderFloat("Scale", &scale, 0.1f, 1000.0f);

        EndUI();

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
