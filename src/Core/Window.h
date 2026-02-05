#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

class Window {
public:
    enum class CursorMode {
        NORMAL = GLFW_CURSOR_NORMAL,
        DISABLED = GLFW_CURSOR_DISABLED,
        HIDDEN = GLFW_CURSOR_HIDDEN,
    };

public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // Non-copyable, non-movable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Core operations
    void SwapBuffers() const;
    void PollEvents() const;
    bool ShouldClose() const;

    // Input queries
    bool IsKeyInState(int key, int state) const;
    bool IsKeyPressed(int key) const;
    std::pair<double, double> GetMousePosition() const;

    // Window properties
    std::pair<int, int> GetSize() const;
    float GetAspectRatio() const;
    bool WasResized() const { return resized; }

    // Settings
    void SetCursorMode(CursorMode mode) const;

    // Direct handle access (if needed for external libraries)
    GLFWwindow* GetHandle() const { return window; }

private:
    static void OnGLFWError(int error, const char* description);
    static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void OnWindowResize(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* window;
    mutable std::vector<int> keyStates;
    mutable bool resized = false;
};
