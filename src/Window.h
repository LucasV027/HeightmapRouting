#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

class Window {
public:
    Window(int w, int h, const std::string& title);
    ~Window();

    GLFWwindow* Handle() const;
    bool ShouldClose() const;

    void Swap() const;
    void Poll() const;

    bool IsKeyInState(int key, int state) const;
    bool IsKeyPressed(int key) const;

private:
    static void GLFWErrorCallback(int error, const char* description);

private:
    mutable std::vector<int> keyStates;
    GLFWwindow* window;
};
