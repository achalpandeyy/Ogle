#ifndef APPLICATION_H

#include "Win32.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

struct ApplicationSettings
{
    unsigned int width = 1280;
    unsigned int height = 720;
    std::string window_title = "Ogle";
    bool enable_cursor = true;
    bool enable_debug_callback = true;
};

struct Application
{
    virtual void Initialize() = 0;
    virtual void Update() = 0;

    virtual ~Application() {}

    int Run();

protected:
    virtual void OnWindowResize(int width, int height) {}
    virtual void OnKeyPress(int key_code) {}
    virtual void OnKeyRelease(int key_code) {}
    virtual void OnMouseMove(double x, double y) {}
    virtual void OnMouseScroll(double vertical_offset) {}

    GLFWwindow* window = nullptr;
    float delta_time = 0.f;

    ApplicationSettings settings;

//     // Todo: Connect this ApplicationSettings::enable_debug_callback not the _DEBUG macro
// #ifdef _DEBUG
//     void WINAPI GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
//         const void* user_param);
// #endif

private:
    void InitializeBase();

    void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void GLFWMouseCallback(GLFWwindow* window, double x_pos, double y_pos);
    void GLFWScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
    void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void GLFWFramebufferSizeCallbackHelper(GLFWwindow* window, int width, int height);
    static void GLFWMouseCallbackHelper(GLFWwindow* window, double x_pos, double y_pos);
    static void GLFWScrollCallbackHelper(GLFWwindow* window, double x_offset, double y_offset);
    static void GLFWKeyCallbackHelper(GLFWwindow* window, int key, int scancode, int action, int mods);

    //     // Todo: Connect this ApplicationSettings::enable_debug_callback not the _DEBUG macro
    // #ifdef _DEBUG
    //     void WINAPI GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
    //         const void* user_param);
    // #endif
};

#define APPLICATION_H
#endif
