#include "AppLogic.h"

#include <future>
#include <ranges>

#include <glm/gtc/type_ptr.hpp>

#include "Algorithm.h"
#include "Core/App.h"
#include "Core/Camera/FreeCamera.h"
#include "Core/Utils.h"
#include "Metric.h"
#include "PathFinder.h"
#include "UI.h"

template <>
Texture Texture::From<Terrain::TileType>(const Mat<Terrain::TileType>& mat) {
    return {mat.Width(), mat.Height(), Format::U8_1, reinterpret_cast<const uint8_t*>(mat.Data())};
}

AppLogic::AppLogic() {
    camera = FreeCamera::Create(glm::vec3(50.f), glm::vec3{0.0f, -1.0f, 0.01f}, 90.f);

    terrain = Terrain::Load(DATA_DIR "Terrain/Heights.png", //
                            DATA_DIR "Terrain/Type.png",    //
                            {100.0f, 100.0f},               // worldSize
                            15.0f,                          // heightScale
                            3.0f                            // waterHeight
    );


    // --- Render ---
    waterProgram = Program::FromFile(DATA_DIR "Shaders/Water.vert", DATA_DIR "Shaders/Water.frag");
    terrainProgram =
        Program::FromFile(DATA_DIR "Shaders/Terrain.vert", DATA_DIR "Shaders/Terrain.frag");
    lineProgram = Program::FromFile(DATA_DIR "Shaders/Line.vert", DATA_DIR "Shaders/Line.frag");
    flagProgram = Program::FromFile(DATA_DIR "Shaders/Flag.vert", DATA_DIR "Shaders/Flag.frag");

    const auto normals = Algorithm::NormalMap(terrain.heightMap, terrain.heightScale);
    heightTex = Texture::From(terrain.heightMap);
    normalTex = Texture::From(normals);
    typeTex = Texture::From(terrain.typeMap);

    flagMesh = Mesh::FromFile(DATA_DIR "Models/Flag.obj");
    terrainMesh = Mesh::PlanarGrid(terrain.dimensions, terrain.origin, terrain.worldSize);
    waterMesh = Mesh::PlanarGrid({2u, 2u}, terrain.origin, terrain.worldSize);

    pathMesh.SetPrimitiveType(PrimitiveType::LINES).SetLayout({{GL_FLOAT, 3}});

    UpdateFlagTransforms();
}

AppLogic::~AppLogic() = default;

void AppLogic::UpdateFlagTransforms() {
    flagTransforms[FLAG_START].Translate(terrain.GridToWorld(start.x, start.y));
    flagTransforms[FLAG_END].Translate(terrain.GridToWorld(end.x, end.y));
}


void AppLogic::Update(const float dt) {
    const auto& window = App::GetWindow();
    if (window.WasResized()) {
        auto [w, h] = window.GetSize();
        glViewport(0, 0, w, h);
    }

    UpdateFlagTransforms();

    camera->Update(dt);


    const glm::mat4 vp = camera->Proj() * camera->View();

    terrainProgram.SetUniform("uVP", vp);
    terrainProgram.SetUniform("uHeightScale", terrain.heightScale);

    if (terrain.waterHeight != -1.f) {
        waterProgram.SetUniform("uVP", vp);
        waterProgram.SetUniform("uHeight", terrain.waterHeight);
    }

    lineProgram.SetUniform("uVP", vp);
    flagProgram.SetUniform("uVP", vp);

    if (jobRunning &&
        pendingJob.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        path = pendingJob.get();

        if (path) {
            std::vector<float> vertices;
            std::vector<uint32_t> indices;

            vertices.reserve(path.points.size() * 3);
            indices.reserve((path.points.size() - 1) * 2);

            for (const auto& p : path.points) {
                glm::vec3 w = terrain.GridToWorldAboveWater(p.x, p.y);
                w.y += 0.2f;

                vertices.push_back(w.x);
                vertices.push_back(w.y);
                vertices.push_back(w.z);
            }

            for (size_t i = 0; i < path.points.size() - 1; ++i) {
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

    if (terrain.waterHeight != -1.f) {
        waterProgram.Bind();
        waterMesh.Draw();
        waterProgram.Unbind();
    }

    if (path) {
        lineProgram.Bind();
        glLineWidth(3.f);
        pathMesh.Draw();
        glLineWidth(1.f);
        lineProgram.Unbind();
    }

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

    ImGui::SeparatorText("Path finding");
    ImGui::InputInt2("Start", glm::value_ptr(start));
    ImGui::InputInt2("End", glm::value_ptr(end));
    start = glm::clamp(start, glm::ivec2(0), terrain.dimensions - 1);
    end = glm::clamp(end, glm::ivec2(0), terrain.dimensions - 1);

    if (ImGui::ComputeButton("Compute", jobRunning)) {
        jobRunning = true;
        pendingJob = std::async(std::launch::async, [&]() -> PathFinder::Path {
            return PathFinder()
                .From(start.x, start.y)
                .To(end.x, end.y)
                .Size(terrain.heightMap.Width(), terrain.heightMap.Height())
                .SetConnectivity(PathFinder::Connectivity::C8)
                .With(0.1f, Metric::Distance())
                .With(10.f, Metric::Slope(terrain.heightMap, terrain.heightScale))
                .With(1.f, Metric::Terrain(terrain.typeMap))
                .Compute();
        });
    }

    if (path) {
        ImGui::Text("Found path with cost %.2f", path.cost);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("No path found");
        ImGui::PopStyleColor();
    }
}
