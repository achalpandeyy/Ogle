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

    virtual void SetApplicationSettings() { settings = ApplicationSettings(); }

    virtual ~Application() {}

    int Run();

protected:
    virtual void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    virtual void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
    virtual void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
    virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void GLFWMouseCallback(GLFWwindow* window, double x_pos, double y_pos);
    static void GLFWScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
    static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

    //     // Todo: Connect this ApplicationSettings::enable_debug_callback not the _DEBUG macro
    // #ifdef _DEBUG
    //     void WINAPI GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
    //         const void* user_param);
    // #endif
};

#define APPLICATION_H
#endif
