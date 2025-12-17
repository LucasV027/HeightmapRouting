#include <iostream>

#include "ScalarField.h"

int main() {
    Image img;
    if (!img.Load(DATA_DIR "Terrain/alps-montblanc.png")) {
        std::cerr << "Failed to load image" << std::endl;
        return 1;
    }

    const ScalarField hm(img, glm::vec2{0.0f, 0.0f}, glm::vec2{1000.f, 1000.f}, 100.f);

    hm.Triangulate().SaveOBJ("../heightmap.obj");

    return 0;
}
