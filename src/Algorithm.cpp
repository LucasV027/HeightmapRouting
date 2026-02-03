#include "Algorithm.h"

Mat<glm::vec3> Algorithm::NormalMap(const Mat<float>& heights, const float scale) {
    const auto size = heights.Size();
    auto normals = Mat<glm::vec3>(size);

    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            const float hL = (x > 0) ? heights(x - 1, y) : heights(x, y);
            const float hR = (x < size.x - 1) ? heights(x + 1, y) : heights(x, y);
            const float hD = (y > 0) ? heights(x, y - 1) : heights(x, y);
            const float hU = (y < size.y - 1) ? heights(x, y + 1) : heights(x, y);

            auto tangent = glm::vec3(2.0f, (hR - hL) * scale, 0.0f);
            auto bitangent = glm::vec3(0.0f, (hU - hD) * scale, 2.0f);

            const auto normal = glm::normalize(glm::cross(bitangent, tangent));

            normals(x, y) = normal;
        }
    }
    return normals;
}

Mat<glm::vec2> Algorithm::Gradient(const Mat<float>& in) {
    const auto size = in.Size();
    Mat<glm::vec2> out(size);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            auto& o = out(x, y);

            if (y == 0)
                o.y = in(x, y) - in(x, y + 1);
            else if (y == size.y - 1)
                o.y = in(x, y - 1) - in(x, y);
            else
                o.y = in(x, y - 1) - in(x, y + 1);

            if (x == 0)
                o.x = in(x, y) - in(x + 1, y);
            else if (x == size.x - 1)
                o.x = in(x - 1, y) - in(x, y);
            else
                o.x = in(x - 1, y) - in(x + 1, y);

            o *= 0.5f;
        }
    }

    return out;
}
