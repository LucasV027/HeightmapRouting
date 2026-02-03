#include "UI.h"

#include <cmath>

#include "Core/App.h"

static constexpr float POINT_GRAB_RADIUS = 7.0f;
static constexpr float POINT_RADIUS = 5.0f;
static constexpr float POINT_OUTLINE_THICKNESS = 1.0f;

static constexpr int GRID_DIVISIONS = 4;
static constexpr int SAMPLES_PER_SEGMENT = 15;

// For selection & hovering
static constexpr float SATURATION_FACTOR = 2.5f;

static constexpr ImU32 COLOR_POINT_DEFAULT = IM_COL32(50, 50, 50, 255);
static constexpr ImU32 COLOR_POINT_OUTLINE = IM_COL32(255, 255, 255, 255);
static constexpr ImU32 COLOR_ANCHOR_LEFT = IM_COL32(255, 100, 100, 255);
static constexpr ImU32 COLOR_ANCHOR_RIGHT = IM_COL32(40, 213, 255, 255);

static constexpr ImU32 COLOR_CANVAS_BORDER = IM_COL32(200, 200, 200, 255);
static constexpr ImU32 COLOR_CANVAS_BG = IM_COL32(50, 50, 50, 255);

static constexpr ImU32 COLOR_GRID_LINE = IM_COL32(100, 100, 100, 100);
static constexpr ImU32 COLOR_CURVE_SEGMENT = IM_COL32(57, 128, 233, 128);
static constexpr float CURVE_THICKNESS = 2.0f;

static constexpr float ANCHOR_EXTENSION = 10.0f;

struct Context {
    ImDrawList* drawList;
    ImVec2 canvasPos;
    ImVec2 canvasSize;
    Curve& curve;
    int& selectedPoint;
    int& hoveredPoint;
};

static void HandleInteraction(const Context& ctx);
static void DrawGrid(const Context& ctx);
static void DrawPoints(const Context& ctx);

static void DrawCurve(const Context& ctx);
static void DrawCurveLinear(const Context& ctx);
static void DrawCurveInterpolation(const Context& ctx);

static ImVec2 NormalizedToScreen(Curve::Point normalized, ImVec2 canvasPos, ImVec2 canvasSize);
static ImVec2 ScreenToNormalized(ImVec2 screen, ImVec2 canvasPos, ImVec2 canvasSize);
static ImU32 SaturateColor(ImU32 color, float factor);

void ImGui::CurveEditor(const char* label, Curve& curve, const ImVec2 size) {
    ImGui::PushID(label);
    ImGui::BeginGroup();
    ImGui::Text("%s", label);

    ImGuiStorage* storage = ImGui::GetStateStorage();
    const ImGuiID selectedPointID = ImGui::GetID("selectedPoint");
    const ImGuiID hoveredPointID = ImGui::GetID("hoveredPoint");

    int selectedPoint = storage->GetInt(selectedPointID, -1);
    int hoveredPoint = storage->GetInt(hoveredPointID, -1);

    const Context ctx{
        .drawList = ImGui::GetWindowDrawList(),
        .canvasPos = ImGui::GetCursorScreenPos(),
        .canvasSize = size,
        .curve = curve,
        .selectedPoint = selectedPoint,
        .hoveredPoint = hoveredPoint,
    };

    // Background
    ctx.drawList->AddRectFilled(
        ctx.canvasPos,
        ImVec2(ctx.canvasPos.x + ctx.canvasSize.x, ctx.canvasPos.y + ctx.canvasSize.y),
        COLOR_CANVAS_BG);

    // Border
    ctx.drawList->AddRect(
        ctx.canvasPos,
        ImVec2(ctx.canvasPos.x + ctx.canvasSize.x, ctx.canvasPos.y + ctx.canvasSize.y),
        COLOR_CANVAS_BORDER);

    DrawGrid(ctx);
    DrawCurve(ctx);
    DrawPoints(ctx);

    HandleInteraction(ctx);

    storage->SetInt(selectedPointID, ctx.selectedPoint);
    storage->SetInt(hoveredPointID, ctx.hoveredPoint);

    // Invisible zone for mouse detection
    const auto extendedPos =
        ImVec2(ctx.canvasPos.x - ANCHOR_EXTENSION, ctx.canvasPos.y - ANCHOR_EXTENSION);
    const auto extendedSize =
        ImVec2(ctx.canvasSize.x + 2 * ANCHOR_EXTENSION, ctx.canvasSize.y + 2 * ANCHOR_EXTENSION);

    ImGui::SetCursorScreenPos(extendedPos);
    ImGui::InvisibleButton("canvas", extendedSize);
    ImGui::SetCursorScreenPos(
        ImVec2(ctx.canvasPos.x, ctx.canvasPos.y + ctx.canvasSize.y + ANCHOR_EXTENSION));

    int currentMode = static_cast<int>(curve.GetInterpolationMode());
    const char* modes[] = {"Linear", "Cosinus"};
    ImGui::SetNextItemWidth(ctx.canvasSize.x);

    if (ImGui::Combo("##InterpolationMode", &currentMode, modes, IM_ARRAYSIZE(modes))) {
        curve.SetInterpolationMode(static_cast<Curve::Interpolation>(currentMode));
    }

    ImGui::EndGroup();
    ImGui::PopID();
}

void HandleInteraction(const Context& ctx) {
    const ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mousePos = io.MousePos;
    auto& points = ctx.curve.GetPoints();

    ctx.hoveredPoint = -1;

    const bool inExtendedCanvas = mousePos.x >= ctx.canvasPos.x - ANCHOR_EXTENSION &&
        mousePos.x <= ctx.canvasPos.x + ctx.canvasSize.x + ANCHOR_EXTENSION &&
        mousePos.y >= ctx.canvasPos.y - ANCHOR_EXTENSION &&
        mousePos.y <= ctx.canvasPos.y + ctx.canvasSize.y + ANCHOR_EXTENSION;

    const bool inCanvas = mousePos.x >= ctx.canvasPos.x &&
        mousePos.x <= ctx.canvasPos.x + ctx.canvasSize.x && mousePos.y >= ctx.canvasPos.y &&
        mousePos.y <= ctx.canvasPos.y + ctx.canvasSize.y;

    if (inExtendedCanvas) {
        for (size_t i = 0; i < points.size(); ++i) {
            const ImVec2 screenPos = NormalizedToScreen(points[i], ctx.canvasPos, ctx.canvasSize);
            const float dist = std::hypotf(mousePos.x - screenPos.x, mousePos.y - screenPos.y);

            if (dist < POINT_GRAB_RADIUS) {
                ctx.hoveredPoint = static_cast<int>(i);
                break;
            }
        }

        // Select point
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ctx.selectedPoint = ctx.hoveredPoint;
        }

        // Delete point
        if (inCanvas && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ctx.hoveredPoint != -1) {
            ctx.curve.RemovePoint(ctx.hoveredPoint);
            ctx.hoveredPoint = -1;
            ctx.selectedPoint = -1;
        }
    }

    // Add point
    if (inCanvas && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ctx.hoveredPoint == -1) {
        const auto [x, y] = ScreenToNormalized(mousePos, ctx.canvasPos, ctx.canvasSize);
        ctx.curve.AddPoint(x, y);
    }

    // Move point
    if (ctx.selectedPoint != -1 && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto [x, y] = ScreenToNormalized(mousePos, ctx.canvasPos, ctx.canvasSize);
        ctx.curve.MovePoint(ctx.selectedPoint, x, y);
    }

    // Drop
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        ctx.selectedPoint = -1;
    }
}


void DrawGrid(const Context& ctx) {
    for (int i = 1; i < GRID_DIVISIONS; ++i) {
        const float x = ctx.canvasPos.x + (ctx.canvasSize.x / GRID_DIVISIONS) * i;
        const float y = ctx.canvasPos.y + (ctx.canvasSize.y / GRID_DIVISIONS) * i;

        // Vertical
        ctx.drawList->AddLine(ImVec2(x, ctx.canvasPos.y),
                              ImVec2(x, ctx.canvasPos.y + ctx.canvasSize.y), COLOR_GRID_LINE);

        // Horizontal
        ctx.drawList->AddLine(ImVec2(ctx.canvasPos.x, y),
                              ImVec2(ctx.canvasPos.x + ctx.canvasSize.x, y), COLOR_GRID_LINE);
    }
}

void DrawPoints(const Context& ctx) {
    auto& points = ctx.curve.GetPoints();
    for (int i = 0; i < points.size(); ++i) {
        ImVec2 screenPos = NormalizedToScreen(points[i], ctx.canvasPos, ctx.canvasSize);

        ImU32 color;
        if (i == 0)
            color = COLOR_ANCHOR_LEFT;
        else if (i == points.size() - 1)
            color = COLOR_ANCHOR_RIGHT;
        else
            color = COLOR_POINT_DEFAULT;


        if (i == ctx.selectedPoint || i == ctx.hoveredPoint) {
            color = SaturateColor(color, SATURATION_FACTOR);
        }

        ctx.drawList->AddCircleFilled(screenPos, POINT_RADIUS, color);
        ctx.drawList->AddCircle(screenPos, POINT_RADIUS, COLOR_POINT_OUTLINE, 0,
                                POINT_OUTLINE_THICKNESS);
    }
}

void DrawCurve(const Context& ctx) {
    if (ctx.curve.GetInterpolationMode() == Curve::Interpolation::LINEAR)
        DrawCurveLinear(ctx);
    else
        DrawCurveInterpolation(ctx);
}

void DrawCurveInterpolation(const Context& ctx) {
    auto& points = ctx.curve.GetPoints();

    if (points.size() < 2)
        return;

    const float step = 1.0f / (SAMPLES_PER_SEGMENT * (points.size() - 1));

    ImVec2 prevScreenPos = NormalizedToScreen(points[0], ctx.canvasPos, ctx.canvasSize);

    for (float x = step; x <= 1.0f; x += step) {
        const float y = ctx.curve(x);
        const Curve::Point currentPoint{x, y};
        const ImVec2 currentScreenPos =
            NormalizedToScreen(currentPoint, ctx.canvasPos, ctx.canvasSize);
        ctx.drawList->AddLine(prevScreenPos, currentScreenPos, COLOR_CURVE_SEGMENT,
                              CURVE_THICKNESS);
        prevScreenPos = currentScreenPos;
    }

    const ImVec2 lastScreenPos = NormalizedToScreen(points.back(), ctx.canvasPos, ctx.canvasSize);
    ctx.drawList->AddLine(prevScreenPos, lastScreenPos, COLOR_CURVE_SEGMENT, CURVE_THICKNESS);
}

void DrawCurveLinear(const Context& ctx) {
    auto& points = ctx.curve.GetPoints();

    if (points.size() < 2)
        return;

    for (size_t i = 0; i < points.size() - 1; ++i) {
        ImVec2 p1 = NormalizedToScreen(points[i], ctx.canvasPos, ctx.canvasSize);
        ImVec2 p2 = NormalizedToScreen(points[i + 1], ctx.canvasPos, ctx.canvasSize);
        ctx.drawList->AddLine(p1, p2, COLOR_CURVE_SEGMENT, CURVE_THICKNESS);
    }
}

ImVec2
NormalizedToScreen(const Curve::Point normalized, const ImVec2 canvasPos, const ImVec2 canvasSize) {
    return {
        canvasPos.x + normalized.x * canvasSize.x,
        canvasPos.y + (1.0f - normalized.y) * canvasSize.y,
    };
}

ImVec2 ScreenToNormalized(const ImVec2 screen, const ImVec2 canvasPos, const ImVec2 canvasSize) {
    return {
        (screen.x - canvasPos.x) / canvasSize.x,
        1.0f - (screen.y - canvasPos.y) / canvasSize.y,
    };
}

static ImU32 SaturateColor(const ImU32 color, const float factor) {
    float r = ((color >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f;
    float g = ((color >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f;
    float b = ((color >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f;

    r = std::min(1.0f, r * factor);
    g = std::min(1.0f, g * factor);
    b = std::min(1.0f, b * factor);

    return IM_COL32(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255),
                    255);
}

bool ImGui::ComputeButton(const char* label, const bool computing, const ImVec2& size) {
    if (computing) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    const bool ret = ImGui::Button(label, size);

    if (computing) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        ImGui::SameLine();
        ImGui::Text("%c", "|/-\\"[static_cast<int>(ImGui::GetTime() / 0.05f) & 3]);
    }
    return ret;
}
