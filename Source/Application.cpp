#include "Application.h"

#include <iostream>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Ogle
{
int Application::Run()
{
    InitializeBase();

    Initialize();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float start_time = (float)glfwGetTime();

        Update();

        delta_time = (float)glfwGetTime() - start_time;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    return 0;
}

void Application::GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    OnWindowResize(width, height);
}

void Application::GLFWMouseCallback(GLFWwindow* window, double x, double y)
{
    OnMouseMove(float(x - last_x), float(last_y - y));
    last_x = (float)x;
    last_y = (float)y;
}

void Application::GLFWScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    OnMouseScroll((float)y_offset);
}

void Application::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        OnKeyPress(key);
    else if (action == GLFW_RELEASE)
        OnKeyRelease(key);
}

// // Todo: Connect this ApplicationSettings::enable_debug_callback not the _DEBUG macro
// #ifdef _DEBUG
// void WINAPI Application::GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
//     const char* message, const void* user_param)
// {
//     std::cout << "-----------------------" << std::endl;
//     std::cout << "Debug Message (" << id << "): " << message << std::endl;
// 
//     switch (source)
//     {
//     case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
//     case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System"; break;
//     case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
//     case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
//     case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
//     case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
//     }
//     std::cout << std::endl;
// 
//     switch (type)
//     {
//     case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
//     case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
//     case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
//     case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
//     case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
//     case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
//     case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
//     case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
//     case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
//     }
//     std::cout << std::endl;
// 
//     switch (severity)
//     {
//     case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
//     case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
//     case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
//     case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
//     }
//     std::cout << std::endl;
// 
//     std::cout << "----------------" << std::endl;
// 
//     // Todo: Can you somehow get a line number from OpenGL?
//     __debugbreak();
// }
// #endif

void Application::InitializeBase()
{
    if (glfwInit() != GLFW_TRUE)
        exit(-1);

    if (settings.enable_debug_callback)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(settings.width, settings.height, settings.window_title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    // Callbacks
    glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallbackHelper);
    glfwSetKeyCallback(window, GLFWKeyCallbackHelper);
    glfwSetCursorPosCallback(window, GLFWMouseCallbackHelper);
    glfwSetScrollCallback(window, GLFWScrollCallbackHelper);

    // Input Modes
    glfwSetInputMode(window, GLFW_CURSOR, settings.enable_cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // if (settings.enable_debug_callback)
    // {
    //     int flags;
    //     glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    //     if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    //     {
    //         glEnable(GL_DEBUG_OUTPUT);
    //         glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //         glDebugMessageCallback(GLDebugOutput, nullptr);
    //         glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // 
    //         std::cout << "Note: Debug context initialized\n" << std::endl;
    //     }
    // }

    // Todo: Make this logging optional
    {
        const GLubyte* opengl_vendor = glGetString(GL_VENDOR);
        const GLubyte* opengl_renderer = glGetString(GL_RENDERER);
        const GLubyte* opengl_version = glGetString(GL_VERSION);

        std::cout << "GPU Vendor: " << opengl_vendor << std::endl;
        std::cout << "Renderer: " << opengl_renderer << std::endl;
        std::cout << "OpenGL Version: " << opengl_version << std::endl;

        std::cout << std::endl;

        GLint max_work_group_count;
        GLint max_work_group_size;
        for (unsigned int i = 0; i < 3; ++i)
        {
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &max_work_group_count);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &max_work_group_size);
            std::cout << (char)(i + 88) << ":" << std::endl;
            std::cout << "\tMax Work Group Count: " << max_work_group_count << std::endl;
            std::cout << "\tMax Work Group Size: " << max_work_group_size << std::endl;

            std::cout << std::endl;
        }

        GLint max_work_group_invocations;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_work_group_invocations);

        std::cout << "Max Work Group Invocations: " << max_work_group_invocations << std::endl;
    }

    // Mouse
    last_x = settings.width / 2.f;
    last_y = settings.height / 2.f;
}

void Application::GLFWFramebufferSizeCallbackHelper(GLFWwindow* window, int width, int height)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GLFWFramebufferSizeCallback(window, width, height);
}

void Application::GLFWMouseCallbackHelper(GLFWwindow* window, double x_pos, double y_pos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GLFWMouseCallback(window, x_pos, y_pos);
}

void Application::GLFWScrollCallbackHelper(GLFWwindow* window, double x_offset, double y_offset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GLFWScrollCallback(window, x_offset, y_offset);
}

void Application::GLFWKeyCallbackHelper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GLFWKeyCallback(window, key, scancode, action, mods);
}
}   // namespace Ogle