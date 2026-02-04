#include "AppLogic.h"

#include <future>

#include <glm/gtc/type_ptr.hpp>

#include "Algorithm.h"
#include "Metric.h"
#include "PathFinder.h"
#include "UI.h"

AppLogic::AppLogic() {
    camera = std::make_unique<Orbiter>(glm::vec3{50.f, 0.f, 50.f}, 200.f, 1280.f / 720.f);

    const auto img = Image::FromFile(DATA_DIR "Terrain/AlpsMontBlanc.png", Image::Format::I);
    if (!img.has_value())
        throw std::runtime_error("Failed to load image");

    hm = Mat<float>(*img);

    terrain.dim = {hm.Width(), hm.Height()};
    terrain.heightScale = heightScale;
    terrain.origin = {0.0f, 0.0f, 0.0f};
    terrain.worldSize = {100.0f, 100.0f};

    normals = Algorithm::NormalMap(hm, heightScale);

    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    terrainProgram = Program::FromFile(DATA_DIR "Shaders/Main.vert", DATA_DIR "Shaders/Main.frag");
    lineProgram = Program::FromFile(DATA_DIR "Shaders/Line.vert", DATA_DIR "Shaders/Line.frag");
    flagProgram = Program::FromFile(DATA_DIR "Shaders/Flag.vert", DATA_DIR "Shaders/Flag.frag");

    heightTex = Texture::From(hm);
    normalTex = Texture::From(normals);

    flagMesh = Mesh::FromFile(DATA_DIR "Models/Flag.obj");
    terrainMesh = Mesh::PlanarGrid(hm.Size(), terrain.origin, terrain.worldSize);
    waterMesh = Mesh::PlanarGrid({2u, 2u}, terrain.origin, terrain.worldSize);

    pathMesh.SetPrimitiveType(PrimitiveType::LINES).SetLayout({{GL_FLOAT, 3}});

    UpdateFlagTransforms();
}

AppLogic::~AppLogic() = default;

void AppLogic::UpdateFlagTransforms() {
    // start
    const float sh = hm(start.x, start.y);
    const glm::vec3 startWorldPos = terrain.GridToWorld(start.x, start.y, sh);
    flagsModels[0] = glm::translate(glm::mat4(1.f), startWorldPos);

    // end
    const float eh = hm(end.x, end.y);
    const glm::vec3 endWorldPos = terrain.GridToWorld(end.x, end.y, eh);
    flagsModels[1] = glm::translate(glm::mat4(1.f), endWorldPos);
}


void AppLogic::Update() {
    UpdateFlagTransforms();

    camera->Update(0.01f);

    terrain.heightScale = heightScale;

    const glm::mat4 vp = camera->Proj() * camera->View();

    terrainProgram.SetUniform("uVP", vp);
    terrainProgram.SetUniform("uHeightScale", terrain.heightScale);

    if (renderWater) {
        waterProgram.SetUniform("uVP", vp);
        waterProgram.SetUniform("uHeight", waterHeight);
    }

    lineProgram.SetUniform("uVP", vp);
    flagProgram.SetUniform("uVP", vp);

    if (jobRunning &&
        pendingJob.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        const auto path = pendingJob.get();

        if (path.size() >= 2) {
            std::vector<float> vertices;
            std::vector<uint32_t> indices;

            vertices.reserve(path.size() * 3);
            indices.reserve((path.size() - 1) * 2);

            for (const auto& p : path) {
                float h = hm(p.x, p.y);
                glm::vec3 w = terrain.GridToWorld(p.x, p.y, h);
                w.y += 0.2f;

                vertices.push_back(w.x);
                vertices.push_back(w.y);
                vertices.push_back(w.z);
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


void AppLogic::Render() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    heightTex.Bind(0);
    normalTex.Bind(1);

    terrainProgram.Bind();
    terrainMesh.Draw();
    terrainProgram.Unbind();

    if (renderWater) {
        waterProgram.Bind();
        waterMesh.Draw();
        waterProgram.Unbind();
    }

    lineProgram.Bind();
    glLineWidth(3.f);
    pathMesh.Draw();
    lineProgram.Unbind();

    flagProgram.Bind();
    flagProgram.SetUniform("uModel", flagsModels[0]);
    flagMesh.Draw();
    flagProgram.SetUniform("uModel", flagsModels[1]);
    flagMesh.Draw();
    flagProgram.Unbind();
}


void AppLogic::UI() {
    ImGui::Text("%d FPS", static_cast<int>(ImGui::GetIO().Framerate));

    camera->UI();

    ImGui::SliderFloat("Height scale", &heightScale, 0.1f, 1000.f);

    ImGui::Checkbox("Water", &renderWater);
    if (renderWater)
        ImGui::SliderFloat("Water level", &waterHeight, 0.f, 100.f);

    ImGui::SeparatorText("Path finding");
    ImGui::Indent();

    ImGui::InputInt2("Start", glm::value_ptr(start));
    ImGui::InputInt2("End", glm::value_ptr(end));
    start = glm::clamp(start, glm::ivec2(0), terrain.dim - 1);
    end = glm::clamp(end, glm::ivec2(0), terrain.dim - 1);

    if (ImGui::ComputeButton("Compute", jobRunning)) {
        jobRunning = true;
        pendingJob = std::async(std::launch::async, [&]() -> PathFinder::Path {
            return PathFinder()
                .From(start.x, start.y)
                .To(end.x, end.y)
                .Size(hm.Width(), hm.Height())
                .SetConnectivity(PathFinder::Connectivity::C8)
                .With(0.1f, Metric::Distance())
                .With(10.f, Metric::Slope(hm, terrain.heightScale))
                .Compute();
        });
    }

    ImGui::Unindent();
}
