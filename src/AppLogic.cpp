#include "AppLogic.h"

#include <future>

#include "Algorithm.h"
#include "PathFinder.h"
#include "UI.h"

AppLogic::AppLogic() {
    camera = std::make_unique<Orbiter>(glm::vec3(0.0f), 200.f, 1280.f / 720.f);

    const auto img = Image::FromFile(DATA_DIR "Terrain/AlpsMontBlanc.png", Image::Format::I);
    if (!img.has_value()) {
        throw std::runtime_error("Failed to load image");
    }

    hm = Mat<float>(*img);
    normals = Algorithm::NormalMap(hm, heightScale);

    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    terrainProgram = Program::FromFile(DATA_DIR "Shaders/Main.vert", DATA_DIR "Shaders/Main.frag");
    lineProgram = Program::FromFile(DATA_DIR "Shaders/Line.vert", DATA_DIR "Shaders/Line.frag");

    heightTex = Texture::From(hm);
    normalTex = Texture::From(normals);

    terrainMesh = Mesh::PlanarGrid(hm.Size(), {-50.0f, -50.0f}, {50.f, 50.f});
    waterMesh = Mesh::PlanarGrid({2u, 2u}, {-50.0f, -50.0f}, {50.0f, 50.0f});
    pathMesh.SetPrimitiveType(PrimitiveType::LINES).SetLayout({{GL_FLOAT, 3}});
}

AppLogic::~AppLogic() = default;

void AppLogic::Update() {
    camera->Update(0.01f);

    const auto vp = camera->Proj() * camera->View();
    terrainProgram.SetUniform("uVP", vp);
    terrainProgram.SetUniform("uHeightScale", heightScale);

    waterProgram.SetUniform("uVP", vp);
    waterProgram.SetUniform("uHeight", waterHeight);

    lineProgram.SetUniform("uVP", vp);

    if (jobRunning && pendingJob.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        const auto path = pendingJob.get();

        if (path.size() >= 2) {
            std::vector<float> vertices;
            std::vector<uint32_t> indices;
            vertices.reserve(path.size() * 3);
            indices.reserve((path.size() - 1) * 2);

            const auto min = glm::vec2(-50.f);
            const auto max = glm::vec2(50.f);
            const float dx = 1.0f / static_cast<float>(hm.Width() - 1);
            const float dy = 1.0f / static_cast<float>(hm.Height() - 1);
            for (const auto p : path) {
                vertices.push_back(min.x + (max.x - min.x) * p.x * dx);
                vertices.push_back(hm(p.x, p.y) * heightScale + 0.2f);
                vertices.push_back(min.y + (max.y - min.y) * p.y * dy);
            }
            for (size_t i = 0; i < path.size() - 1; ++i) {
                indices.push_back(static_cast<uint32_t>(i));
                indices.push_back(static_cast<uint32_t>(i + 1));
            }

            pathMesh.SetVertices(std::move(vertices)).SetIndices(std::move(indices)).Upload();
        }

        jobRunning = false;
    }
}

void AppLogic::Render() const {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    heightTex.Bind(0);
    normalTex.Bind(1);

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

void AppLogic::UI() {
    ImGui::Text("%d FPS", static_cast<int>(ImGui::GetIO().Framerate));
    camera->UI();
    ImGui::SliderFloat("Scale", &heightScale, 0.1f, 1000.0f);
    ImGui::SliderFloat("Water", &waterHeight, 0.0f, 100.0f);

    ImGui::SeparatorText("Path find");
    ImGui::Indent();

    static Curve curve(Curve::Interpolation::LINEAR);
    static int start[2] = {20, 20};
    static int end[2] = {300, 300};

    ImGui::CurveEditor("Height metric", curve);

    ImGui::InputInt2("Start", start);
    ImGui::InputInt2("End", end);

    if (ImGui::ComputeButton("Compute", jobRunning)) {
        jobRunning = true;
        pendingJob = std::async(std::launch::async, [&]() -> PathFinder::Path {
            return PathFinder()
                .From(start[0], start[1])
                .To(end[0], end[1])
                .With(curve, hm)
                .Compute();
        });
    }

    ImGui::Unindent();
}
