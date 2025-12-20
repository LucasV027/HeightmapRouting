#include "App.h"

#include <iostream>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Orbiter.h"
#include "ScalarField.h"
#include "Utils.h"
#include "Window.h"

static App* instance = nullptr;

App::App() {
    if (instance)
        throw std::runtime_error("App already initialized");
    instance = this;

    window = std::make_unique<Window>(1280, 720, "MMV");
    camera = std::make_unique<Orbiter>(glm::vec3(0.0f), 500.f, 1280.f / 720.f);

    InitOpenGL();
    InitImGui();

    Image img;
    if (!img.Load(DATA_DIR "Terrain/alps-montblanc.png")) {
        std::cerr << "Failed to load image" << std::endl;
        throw std::runtime_error("Failed to load image");
    }

    ScalarField hm(img, glm::vec2{-500.0f, -500.0f}, glm::vec2{1000.f, 1000.f});
    const PlanarGridMesh pgm = hm.GridMesh();
    count = pgm.indices.size() * 3;


    {
        const auto vertSrc = Utils::ReadFile(DATA_DIR "Shaders/Main.vert");
        const auto fragSrc = Utils::ReadFile(DATA_DIR "Shaders/Main.frag");
        program = Utils::GL::CreateProgram(vertSrc.c_str(), fragSrc.c_str());
    }


    {
        glCreateTextures(GL_TEXTURE_2D, 1, &heightTex);
        glTextureStorage2D(heightTex, 1, GL_R16F, hm.Width(), hm.Height());
        glTextureSubImage2D(heightTex, 0, 0, 0, hm.Width(), hm.Height(), GL_RED, GL_FLOAT,
                            hm.Data());

        glTextureParameteri(heightTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(heightTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(heightTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(heightTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    {
        auto& indices = pgm.indices;
        auto& positions = pgm.positions;
        auto& uvs = pgm.uvs;

        glCreateVertexArrays(1, &vao);

        // IBO
        glCreateBuffers(1, &ibo);
        glNamedBufferStorage(ibo, indices.size() * sizeof(indices[0]), indices.data(),
                             GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(vao, ibo);

        // VBO - Positions
        glCreateBuffers(1, &vboPos);
        glNamedBufferStorage(vboPos, positions.size() * sizeof(positions[0]), positions.data(),
                             GL_DYNAMIC_STORAGE_BIT);

        // VBO - UV
        glCreateBuffers(1, &vboUV);
        glNamedBufferStorage(vboUV, uvs.size() * sizeof(uvs[0]), uvs.data(),
                             GL_DYNAMIC_STORAGE_BIT);

        // Bind VBO
        glVertexArrayVertexBuffer(vao, 0, vboPos, 0, sizeof(positions[0]));
        glVertexArrayVertexBuffer(vao, 1, vboUV, 0, sizeof(uvs[0]));

        // Position (2 Floats | Location = 0)
        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, 0, 0);

        // UVs (2 Floats | Location = 1)
        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, 1, 1);
    }
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &vao);
    const GLuint vbos[] = {vboPos, vboUV, ibo};
    glDeleteBuffers(std::size(vbos), vbos);
    glDeleteTextures(1, &heightTex);
    glDeleteProgram(program);

    instance = nullptr;
}

void App::Run() {
    while (!window->ShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto vp = camera->Proj() * camera->View();

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "uVP"), 1, GL_FALSE, glm::value_ptr(vp));
        glUniform1i(glGetUniformLocation(program, "uHeightMap"), 0);
        glUniform1f(glGetUniformLocation(program, "uHeightScale"), 100.0f);

        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightTex);

        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glUseProgram(0);

        camera->Update(0.01f);

        BeginUI();

        camera->UI();

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
