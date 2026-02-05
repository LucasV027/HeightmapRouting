#include "App.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "Utils.h"

static App* instance = nullptr;

App::App() {
    if (instance)
        throw std::runtime_error("App already initialized");
    instance = this;

    window = std::make_unique<Window>(1280, 720, "MMV");

    InitOpenGL();
    InitImGui();

    appLogic = std::make_unique<AppLogic>();
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    instance = nullptr;
}

void App::Run() const {
    double lastFrame = Time();

    while (!window->ShouldClose()) {
        const double time = Time();
        const double dt = time - lastFrame;
        lastFrame = time;

        appLogic->Update(static_cast<float>(dt));

        appLogic->Render();

        BeginUI();
        appLogic->UI();
        EndUI();

        window->SwapBuffers();
        window->PollEvents();
    }
}

App& App::Get() {
    return *instance;
}

Window& App::GetWindow() {
    return *instance->window;
}

double App::Time() {
    return glfwGetTime();
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

    ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), true);
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
