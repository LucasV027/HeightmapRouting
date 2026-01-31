#pragma once

#include <imgui.h>

#include "Curve.h"

namespace ImGui {

    void CurveEditor(const char* label, Curve& curve, ImVec2 size = ImVec2(150, 150));

} // namespace ImGui
