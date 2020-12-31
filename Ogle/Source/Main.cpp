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

glm::vec3 g_camera_position(3.f, 2.f, 7.f);
glm::vec3 g_camera_front = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 g_camera_orientation(0.f, 1.f, 0.f);

int g_width = 1280;
int g_height = 720;

float g_delta_time = 0.f;
float g_last_frame = 0.f;

double g_last_x = (double)g_width / 2;
double g_last_y = (double)g_height / 2;
float g_yaw = -90.f;
float g_pitch = 0.f;
float g_camera_fov = 60.f;
bool g_first_mouse = true;

void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLFWMouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
    if (g_first_mouse)
    {
        g_last_x = x_pos;
        g_last_y = y_pos;
        g_first_mouse = false;
    }

    float x_offset = (float)(x_pos - g_last_x);
    float y_offset = (float)(g_last_y - y_pos);

    const float sensitivity = 0.05f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    g_yaw += x_offset;
    g_pitch += y_offset;

    g_pitch = glm::clamp(g_pitch, -89.f, 89.f);

    g_camera_front.x = glm::cos(glm::radians(g_pitch)) * glm::cos(glm::radians(g_yaw));
    g_camera_front.y = glm::sin(glm::radians(g_pitch));
    g_camera_front.z = glm::cos(glm::radians(g_pitch)) * glm::sin(glm::radians(g_yaw));

    g_camera_front = glm::normalize(g_camera_front);

    g_last_x = x_pos;
    g_last_y = y_pos;
}

void GLFWScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
    g_camera_fov -= (float)y_offset;
    g_camera_fov = glm::clamp(g_camera_fov, 1.f, 45.f);
}

bool g_show_image_test = false;
void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } break;

        case GLFW_KEY_Q:
        {
            glfwSetCursorPosCallback(window, 0);
            glfwSetScrollCallback(window, 0);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            g_show_image_test = true;
        } break;

        case GLFW_KEY_E:
        {
            glfwSetCursorPosCallback(window, GLFWMouseCallback);
            glfwSetScrollCallback(window, GLFWScrollCallback);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            g_first_mouse = true;

            g_show_image_test = false;
        } break;
    }
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

struct Mesh
{
    Mesh(const float* vertices, unsigned int vertex_count, const unsigned int* indices, unsigned int index_count)
    {
        glGenVertexArrays(1, &vao);
        BindVAO();

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        UnbindVAO();
    }

    ~Mesh()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }

    inline void BindVAO() const { glBindVertexArray(vao); }
    inline void UnbindVAO() const { glBindVertexArray(0); }

private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
};

glm::vec3 GetRayDirectionFromNDC(const glm::vec2& ndc, const glm::mat4& view_proj_inverse, const glm::vec3& ray_origin)
{
    glm::vec4 point_world_space = view_proj_inverse * glm::vec4(ndc, 0.f, 1.f);
    point_world_space /= point_world_space.w;
    return glm::vec3(point_world_space) - ray_origin;
}

int main()
{
    GLFWwindow* window;

    if (!glfwInit()) return -1;

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(g_width, g_height, "Ogle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowTitle(window, "Ogle");
    
    glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallback);

    glfwSetCursorPosCallback(window, GLFWMouseCallback);
    glfwSetScrollCallback(window, GLFWScrollCallback);
    glfwSetKeyCallback(window, GLFWKeyCallback);

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
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

            std::cout << "Note: Debug context initialized\n" << std::endl;
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

    glViewport(0, 0, g_width, g_height);

    float quad_vertices[] =
    {
        -1.f, -1.f,
         1.f, -1.f,
         1.f,  1.f,
        -1.f,  1.f
    };

    unsigned int quad_indices[] = { 0, 1, 2, 2, 3, 0 };

    Mesh fullscreen_quad(quad_vertices, 8 * 2, quad_indices, 6);

    GLuint fb_texture;
    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    Shader fullscreen_quad_shader("Source/Shaders/FullscreenQuadShader.vert", "Source/Shaders/FullscreenQuadShader.frag");

    Shader raytracing_shader("Source/Shaders/Raytracing.comp");

    glm::mat4 view = glm::translate(glm::mat4(1.f), -g_camera_position);
    glm::mat4 proj = glm::perspective(glm::radians(g_camera_fov), (float)g_width / (float)g_height, 0.1f, 100.f);

    glm::mat4 view_proj_inv = glm::inverse(view)* glm::inverse(proj);

    raytracing_shader.Bind();

    raytracing_shader.SetVec3("u_Eye", g_camera_position.x, g_camera_position.y, g_camera_position.z);

    glm::vec3 ray_direction00 = GetRayDirectionFromNDC(glm::vec2(-1.f, -1.f), view_proj_inv, g_camera_position);
    raytracing_shader.SetVec3("u_RayDirection00", ray_direction00.x, ray_direction00.y, ray_direction00.z);

    glm::vec3 ray_direction01 = GetRayDirectionFromNDC(glm::vec2(-1.f, 1.f), view_proj_inv, g_camera_position);
    raytracing_shader.SetVec3("u_RayDirection01", ray_direction01.x, ray_direction01.y, ray_direction01.z);

    glm::vec3 ray_direction10 = GetRayDirectionFromNDC(glm::vec2(1.f, -1.f), view_proj_inv, g_camera_position);
    raytracing_shader.SetVec3("u_RayDirection10", ray_direction10.x, ray_direction10.y, ray_direction10.z);

    glm::vec3 ray_direction11 = GetRayDirectionFromNDC(glm::vec2(1.f, 1.f), view_proj_inv, g_camera_position);
    raytracing_shader.SetVec3("u_RayDirection11", ray_direction11.x, ray_direction11.y, ray_direction11.z);

    GLint work_group_size[3] = {};
    glGetProgramiv(raytracing_shader.id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);

    // Image Processing Test
    GLuint image_tex = GLuint(-1);

    while (!glfwWindowShouldClose(window))
    {
        // Todo: For the image processing test I don't need delta_time
        float current_frame = (float)glfwGetTime();
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        // Todo: For the image processing test I don't need this camera calculation,
        // there is no notion of a camera
        const float camera_speed = 2.5f * g_delta_time;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            g_camera_position += camera_speed * g_camera_front;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            g_camera_position -= camera_speed * g_camera_front;
        }

        glm::vec3 camera_right = glm::normalize(glm::cross(g_camera_front, g_camera_orientation));
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            g_camera_position -= camera_speed * camera_right;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            g_camera_position += camera_speed * camera_right;
        }

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        if (g_show_image_test)
        {
            glClearColor(0.8f, 0.3f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (image_tex == GLuint(-1))
            {
                const char* image_path = "../Resources/container.jpg";
                int image_tex_width, image_tex_height, image_tex_channel_count;
                unsigned char* image_data = stbi_load(image_path, &image_tex_width, &image_tex_height, &image_tex_channel_count, 3);

                if (image_data)
                {
                    glGenTextures(1, &image_tex);
                    glBindTexture(GL_TEXTURE_2D, image_tex);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    // Todo: You can make this an immutable texture
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_tex_width, image_tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                else
                {
                    std::cout << "Failed to load image: " << image_path << std::endl;
                    exit(1);
                }

                stbi_image_free(image_data);
            }

            glBindTexture(GL_TEXTURE_2D, image_tex);

            fullscreen_quad.BindVAO();
            fullscreen_quad_shader.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else
        {
            view = glm::lookAt(g_camera_position, g_camera_position + g_camera_front, g_camera_orientation);
            proj = glm::perspective(glm::radians(g_camera_fov), (float)g_width / (float)g_height, 0.1f, 100.f);

            glm::mat4 view_proj_inv = glm::inverse(view) * glm::inverse(proj);

            raytracing_shader.Bind();

            raytracing_shader.SetVec3("u_Eye", g_camera_position.x, g_camera_position.y, g_camera_position.z);

            ray_direction00 = GetRayDirectionFromNDC(glm::vec2(-1.f, -1.f), view_proj_inv, g_camera_position);
            raytracing_shader.SetVec3("u_RayDirection00", ray_direction00.x, ray_direction00.y, ray_direction00.z);

            ray_direction01 = GetRayDirectionFromNDC(glm::vec2(-1.f, 1.f), view_proj_inv, g_camera_position);
            raytracing_shader.SetVec3("u_RayDirection01", ray_direction01.x, ray_direction01.y, ray_direction01.z);

            ray_direction10 = GetRayDirectionFromNDC(glm::vec2(1.f, -1.f), view_proj_inv, g_camera_position);
            raytracing_shader.SetVec3("u_RayDirection10", ray_direction10.x, ray_direction10.y, ray_direction10.z);

            ray_direction11 = GetRayDirectionFromNDC(glm::vec2(1.f, 1.f), view_proj_inv, g_camera_position);
            raytracing_shader.SetVec3("u_RayDirection11", ray_direction11.x, ray_direction11.y, ray_direction11.z);

            // Bind level 0 of the framebuffer texture to image binding point 0
            glBindImageTexture(0, fb_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            // Dispatch the compute shader to generate a frame in the framebuffer image
            glDispatchCompute(g_width / work_group_size[0], g_height / work_group_size[1], 1);

            // Unbind image binding point
            glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fb_texture);

            fullscreen_quad_shader.Bind();
            fullscreen_quad.BindVAO();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &fb_texture);
    glDeleteTextures(1, &image_tex);

    glfwTerminate();
    return 0;
}
