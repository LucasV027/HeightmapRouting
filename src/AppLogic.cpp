#include "AppLogic.h"

#include <future>
#include <ranges>

#include <glm/gtc/type_ptr.hpp>

#include "Algorithm.h"
#include "Core/App.h"
#include "Core/Camera/FreeCamera.h"
#include "Metric.h"
#include "PathFinder.h"
#include "UI.h"

AppLogic::AppLogic() {
    camera = FreeCamera::Create(glm::vec3(50.f), glm::vec3{0.0f, -1.0f, 0.01f}, 90.f);

    const auto heightData = Image::FromFile(DATA_DIR "Terrain/Heights.png", Image::Format::I);
    const auto vegetationData =
        Image::FromFile(DATA_DIR "Terrain/Vegetation.png", Image::Format::I);
    if (!heightData.has_value() || !vegetationData.has_value())
        throw std::runtime_error("Failed to load image");

    heightMap = Mat<float>(*heightData);
    auto vegetationMap = Mat<float>(*vegetationData);

    tm = Mat<uint8_t>(heightMap.Size(), (uint8_t)TerrainType::NORMAL);
    for (int y = 0; y < heightMap.Size().x; y++) {
        for (int x = 0; x < heightMap.Size().x; x++) {
            if (heightMap(x, y) * heightScale <= waterHeight) {
                tm(x, y) = (uint8_t)TerrainType::WATER;
            } else if (vegetationMap(x, y) <= 0.5f) {
                tm(x, y) = (uint8_t)TerrainType::FOREST;
            }
        }
    }

    terrain.dim = {heightMap.Width(), heightMap.Height()};
    terrain.heightScale = heightScale;
    terrain.origin = {0.0f, 0.0f, 0.0f};
    terrain.worldSize = {100.0f, 100.0f};

    normals = Algorithm::NormalMap(heightMap, heightScale);

    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    terrainProgram =
        Program::FromFile(DATA_DIR "Shaders/Terrain.vert", DATA_DIR "Shaders/Terrain.frag");
    lineProgram = Program::FromFile(DATA_DIR "Shaders/Line.vert", DATA_DIR "Shaders/Line.frag");
    flagProgram = Program::FromFile(DATA_DIR "Shaders/Flag.vert", DATA_DIR "Shaders/Flag.frag");

    heightTex = Texture::From(heightMap);
    normalTex = Texture::From(normals);
    typeTex = Texture::From(tm);

    flagMesh = Mesh::FromFile(DATA_DIR "Models/Flag.obj");
    terrainMesh = Mesh::PlanarGrid(heightMap.Size(), terrain.origin, terrain.worldSize);
    waterMesh = Mesh::PlanarGrid({2u, 2u}, terrain.origin, terrain.worldSize);

    pathMesh.SetPrimitiveType(PrimitiveType::LINES).SetLayout({{GL_FLOAT, 3}});

    UpdateFlagTransforms();
}

AppLogic::~AppLogic() = default;

void AppLogic::UpdateFlagTransforms() {
    // start
    {
        const float startHeight = heightMap(start.x, start.y);
        const glm::vec3 startWorldPos = terrain.GridToWorld(start.x, start.y, startHeight);
        flagTransforms[FLAG_START].Translate(startWorldPos);
    }

    // end
    {
        const float endHeight = heightMap(end.x, end.y);
        const glm::vec3 endWorldPos = terrain.GridToWorld(end.x, end.y, endHeight);
        flagTransforms[FLAG_END].Translate(endWorldPos);
    }
}


void AppLogic::Update(const float dt) {
    const auto& window = App::GetWindow();
    if (window.WasResized()) {
        auto [w, h] = window.GetSize();
        glViewport(0, 0, w, h);
    }

    UpdateFlagTransforms();

    camera->Update(dt);

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
                const float h = heightMap(p.x, p.y);
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
    typeTex.Bind(2);

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
    glLineWidth(1.f);
    lineProgram.Unbind();

    flagProgram.Bind();
    for (const auto& [transform, color] : std::views::zip(flagTransforms, flagColors)) {
        flagProgram.SetUniform("uModel", transform.GetMatrix());
        flagProgram.SetUniform("uColor", color);
        flagMesh.Draw();
    }
    flagProgram.Unbind();
}


void AppLogic::UI() {
    ImGui::SeparatorText("Info");
    ImGui::Text("%d FPS", static_cast<int>(ImGui::GetIO().Framerate));

    ImGui::SeparatorText("Render");
    static bool wireframe = false;
    if (ImGui::Checkbox("Wireframe", &wireframe))
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    ImGui::SliderFloat("Height scale", &heightScale, 0.1f, 1000.f);

    ImGui::Checkbox("Water", &renderWater);
    if (renderWater)
        ImGui::SliderFloat("Water level", &waterHeight, 0.f, 100.f);

    ImGui::SeparatorText("Path finding");
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
                .Size(heightMap.Width(), heightMap.Height())
                .SetConnectivity(PathFinder::Connectivity::C8)
                .With(0.1f, Metric::Distance())
                .With(10.f, Metric::Slope(heightMap, terrain.heightScale))
                .Compute();
        });
    }
}
