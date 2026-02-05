#include "Window.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <imgui.h>

Window::Window(const int width, const int height, const std::string& title) {
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

#ifndef NDEBUG
    glfwSetErrorCallback(OnGLFWError);
#endif

    // Configure OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Setup input callbacks
    keyStates.resize(GLFW_KEY_LAST + 1, GLFW_RELEASE);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetWindowSizeCallback(window, OnWindowResize);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void Window::PollEvents() const {
    resized = false;
    std::ranges::fill(keyStates, -1);
    glfwPollEvents();
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window);
}

bool Window::IsKeyInState(const int key, const int state) const {
    if (ImGui::GetIO().WantCaptureKeyboard)
        return false;
    return keyStates[key] == state;
}

bool Window::IsKeyPressed(const int key) const {
    if (ImGui::GetIO().WantCaptureKeyboard)
        return false;
    return glfwGetKey(window, key) == GLFW_PRESS;
}

std::pair<double, double> Window::GetMousePosition() const {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return {x, y};
}

std::pair<int, int> Window::GetSize() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return {width, height};
}

float Window::GetAspectRatio() const {
    auto [width, height] = GetSize();
    return height == 0 ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
}

void Window::SetCursorMode(CursorMode mode) const {
    glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
}

void Window::OnGLFWError(const int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void Window::OnKeyEvent(GLFWwindow* window, const int key, int, const int action, int) {
    const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->keyStates[key] = action;
}

void Window::OnWindowResize(GLFWwindow* window, int, int) {
    const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->resized = true;
}
