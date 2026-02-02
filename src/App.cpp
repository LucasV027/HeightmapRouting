#include "App.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "ImCurve.h"
#include "PathFinder.h"
#include "Utils.h"

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

    hm = std::make_unique<Mat<float>>(*img);

    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    terrainProgram = Program::FromFile(DATA_DIR "Shaders/Main.vert", DATA_DIR "Shaders/Main.frag");
    lineProgram = Program::FromFile(DATA_DIR "Shaders/Line.vert", DATA_DIR "Shaders/Line.frag");

    heightTex = Texture::From(*hm);

    terrainMesh = Mesh::PlanarGrid(hm->Size(), {-50.0f, -50.0f}, {50.f, 50.f});
    waterMesh = Mesh::PlanarGrid({2u, 2u}, {-50.0f, -50.0f}, {50.0f, 50.0f});
    pathMesh.SetPrimitiveType(PrimitiveType::LINES).SetLayout({{GL_FLOAT, 3}});
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
            terrainProgram.SetUniform("uVP", vp);
            terrainProgram.SetUniform("uHeightScale", heigthScale);

            waterProgram.SetUniform("uVP", vp);
            waterProgram.SetUniform("uHeight", waterHeight);

            lineProgram.SetUniform("uVP", vp);
        }

        // Render
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            heightTex.Bind();

            terrainProgram.Bind();
            terrainMesh.Draw();
            terrainProgram.Unbind();

            waterProgram.Bind();
            waterMesh.Draw();
            waterProgram.Unbind();

            lineProgram.Bind();
            glLineWidth(3.0f);
            pathMesh.Draw();
            lineProgram.Unbind();
        }

        // UI
        {
            BeginUI();
            ImGui::Text("%d FPS", static_cast<int>(ImGui::GetIO().Framerate));
            camera->UI();
            ImGui::SliderFloat("Scale", &heigthScale, 0.1f, 1000.0f);
            ImGui::SliderFloat("Water", &waterHeight, 0.0f, 100.0f);

            ImGui::SeparatorText("Path find");
            ImGui::Indent();

            static PathFinder pathFinder;
            static Curve curve(Curve::Interpolation::LINEAR);
            static int start[2] = {20, 20};
            static int end[2] = {300, 300};

            ImGui::CurveEditor("Height metric", curve);

            ImGui::InputInt2("Start", start);
            ImGui::InputInt2("End", end);

            if (ImGui::Button("Compute")) {
                auto path = PathFinder() //
                                .From(start[0], start[1])
                                .To(end[0], end[1])
                                .With(curve, *hm)
                                .Compute();

                if (path.size() >= 2) {
                    std::vector<float> vertices;
                    std::vector<uint32_t> indices;
                    vertices.reserve(path.size() * 3);
                    indices.reserve((path.size() - 1) * 2);

                    auto min = glm::vec2(-50.f);
                    auto max = glm::vec2(50.f);
                    const float dx = 1.0f / static_cast<float>(hm->Width() - 1);
                    const float dy = 1.0f / static_cast<float>(hm->Height() - 1);
                    for (const auto p : path) {
                        vertices.push_back(min.x + (max.x - min.x) * p.x * dx);
                        vertices.push_back((*hm)(p.x, p.y) * heigthScale + 0.2f);
                        vertices.push_back(min.y + (max.y - min.y) * p.y * dy);
                    }
                    for (size_t i = 0; i < path.size() - 1; ++i) {
                        indices.push_back(static_cast<uint32_t>(i));
                        indices.push_back(static_cast<uint32_t>(i + 1));
                    }

                    pathMesh.SetVertices(std::move(vertices))
                        .SetIndices(std::move(indices))
                        .Upload();
                }
            }

            ImGui::Unindent();


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
