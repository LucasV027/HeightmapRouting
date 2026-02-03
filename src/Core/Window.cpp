#include "Window.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

Window::Window(const int w, const int h, const std::string& title) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

#ifndef NDEBUG
    glfwSetErrorCallback(GLFWErrorCallback);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    keyStates.resize(GLFW_KEY_LAST + 1, GLFW_RELEASE);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow* window, const int key, int, const int action, int) {
        const auto& _this = *static_cast<Window*>(glfwGetWindowUserPointer(window));
        _this.keyStates[key] = action;
    });
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Window::Handle() const {
    return window;
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::Swap() const {
    glfwSwapBuffers(window);
}

void Window::Poll() const {
    std::ranges::fill(keyStates, -1);
    glfwPollEvents();
}

bool Window::IsKeyInState(const int key, const int state) const {
    return keyStates[key] == state;
}

bool Window::IsKeyPressed(const int key) const {
    return glfwGetKey(window, key);
}

void Window::GLFWErrorCallback(const int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
