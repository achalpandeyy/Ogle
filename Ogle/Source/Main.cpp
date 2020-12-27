#include "Win32.h"
#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

glm::vec3 camera_position(3.f, 2.f, 7.f);
glm::vec3 camera_front = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 camera_orientation(0.f, 1.f, 0.f);

int width = 1280;
int height = 720;

float delta_time = 0.f;
float last_frame = 0.f;

double last_x = (double)width / 2;
double last_y = (double)height / 2;
float yaw = -90.f;
float pitch = 0.f;
float camera_fov = 60.f;
bool first_mouse = true;

void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLFWMouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
    if (first_mouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = (float)(x_pos - last_x);
    float y_offset = (float)(last_y - y_pos);

    const float sensitivity = 0.05f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    pitch = glm::clamp(pitch, -89.f, 89.f);

    camera_front.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    camera_front.y = glm::sin(glm::radians(pitch));
    camera_front.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));

    camera_front = glm::normalize(camera_front);

    last_x = x_pos;
    last_y = y_pos;
}

void GLFWScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    camera_fov -= (float)y_offset;
    camera_fov = glm::clamp(camera_fov, 1.f, 45.f);
}


#ifdef _DEBUG
void WINAPI GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char* message, const void* user_param)
{
    std::cout << "-----------------------" << std::endl;
    std::cout << "Debug Message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
    }
    std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    }
    std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    }
    std::cout << std::endl;

    std::cout << "----------------" << std::endl;

    // Todo: Can you somehow get a line number from OpenGL?
    __debugbreak();
}
#endif

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(width, height, "Ogle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowTitle(window, "Ogle");

    glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallback);
    glfwSetCursorPosCallback(window, GLFWMouseCallback);
    glfwSetScrollCallback(window, GLFWScrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#ifdef _DEBUG
    {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            std::cout << "Note: Debug context initialized\n" << std::endl;

            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }
#endif

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

    glViewport(0, 0, width, height);

    GLuint fb_texture;
    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    float quad_vertices[] =
    {
        -1.f, -1.f,
         1.f, -1.f,
         1.f,  1.f,
        -1.f,  1.f
    };

    unsigned int quad_indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint quad_vao;
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    GLuint quad_ibo;
    glGenBuffers(1, &quad_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    Shader raytracing_shader("Source/Shaders/Raytracing.comp");
    Shader quad_shader("Source/Shaders/QuadShader.vert", "Source/Shaders/QuadShader.frag");

    GLint work_group_size[3] = {};
    glGetProgramiv(raytracing_shader.id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size);

    glm::mat4 view = glm::translate(glm::mat4(1.f), -camera_position);
    glm::mat4 proj = glm::perspective(glm::radians(camera_fov), (float)width / (float)height, 0.1f, 100.f);

    glm::mat4 view_proj_inv = glm::inverse(view)* glm::inverse(proj);

    raytracing_shader.Bind();

    raytracing_shader.SetVec3("u_Eye", camera_position.x, camera_position.y, camera_position.z);

    glm::vec4 ray00 = view_proj_inv * glm::vec4(-1.f, -1.f, 0.f, 1.f);
    ray00 /= ray00.w;
    raytracing_shader.SetVec3("u_Ray00", ray00.x - camera_position.x, ray00.y - camera_position.y, ray00.z - camera_position.z);

    glm::vec4 ray01 = view_proj_inv * glm::vec4(-1.f, 1.f, 0.f, 1.f);
    ray01 /= ray01.w;
    raytracing_shader.SetVec3("u_Ray01", ray01.x - camera_position.x, ray01.y - camera_position.y, ray01.z - camera_position.z);

    glm::vec4 ray10 = view_proj_inv * glm::vec4(1.f, -1.f, 0.f, 1.f);
    ray10 /= ray10.w;
    raytracing_shader.SetVec3("u_Ray10", ray10.x - camera_position.x, ray10.y - camera_position.y, ray10.z - camera_position.z);

    glm::vec4 ray11 = view_proj_inv * glm::vec4(1.f, 1.f, 0.f, 1.f);
    ray11 /= ray11.w;
    raytracing_shader.SetVec3("u_Ray11", ray11.x - camera_position.x, ray11.y - camera_position.y, ray11.z - camera_position.z);

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = (float)glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        const float camera_speed = 2.5f * delta_time;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera_position += camera_speed * camera_front;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera_position -= camera_speed * camera_front;
        }

        glm::vec3 camera_right = glm::normalize(glm::cross(camera_front, camera_orientation));
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera_position -= camera_speed * camera_right;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera_position += camera_speed * camera_right;
        }

        glfwPollEvents();

        view = glm::lookAt(camera_position, camera_position + camera_front, camera_orientation);
        proj = glm::perspective(glm::radians(camera_fov), (float)width / (float)height, 0.1f, 100.f);

        glm::mat4 view_proj_inv = glm::inverse(view) * glm::inverse(proj);

        raytracing_shader.Bind();

        raytracing_shader.SetVec3("u_Eye", camera_position.x, camera_position.y, camera_position.z);

        ray00 = view_proj_inv * glm::vec4(-1.f, -1.f, 0.f, 1.f);
        ray00 /= ray00.w;
        raytracing_shader.SetVec3("u_Ray00", ray00.x - camera_position.x, ray00.y - camera_position.y, ray00.z - camera_position.z);

        ray01 = view_proj_inv * glm::vec4(-1.f, 1.f, 0.f, 1.f);
        ray01 /= ray01.w;
        raytracing_shader.SetVec3("u_Ray01", ray01.x - camera_position.x, ray01.y - camera_position.y, ray01.z - camera_position.z);

        ray10 = view_proj_inv * glm::vec4(1.f, -1.f, 0.f, 1.f);
        ray10 /= ray10.w;
        raytracing_shader.SetVec3("u_Ray10", ray10.x - camera_position.x, ray10.y - camera_position.y, ray10.z - camera_position.z);

        ray11 = view_proj_inv * glm::vec4(1.f, 1.f, 0.f, 1.f);
        ray11 /= ray11.w;
        raytracing_shader.SetVec3("u_Ray11", ray11.x - camera_position.x, ray11.y - camera_position.y, ray11.z - camera_position.z);

        // Bind level 0 of the framebuffer texture to image binding point 0
        glBindImageTexture(0, fb_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        // Todo: Shoule width and height be a power of two?
        // Dispatch the compute shader to generate a frame in the framebuffer image
        glDispatchCompute(width / work_group_size[0], height / work_group_size[1], 1);

        // Unbind image binding point
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb_texture);

        quad_shader.Bind();
        glBindVertexArray(quad_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteBuffers(1, &quad_ibo);
    glDeleteTextures(1, &fb_texture);

    glfwTerminate();
    return 0;
}
