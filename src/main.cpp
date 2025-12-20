#include <iostream>

#include "App.h"

int main() {
    try {
        App app;
        app.Run();
    } catch (std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
