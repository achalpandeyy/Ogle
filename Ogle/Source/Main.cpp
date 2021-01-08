#include "Win32.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture2D.h"

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
bool g_show_fft_ocean_test = false;
void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } break;

        case GLFW_KEY_I:
        {
            glfwSetCursorPosCallback(window, 0);
            glfwSetScrollCallback(window, 0);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            g_show_image_test = true;
        } break;

        case GLFW_KEY_R:
        {
            glfwSetCursorPosCallback(window, GLFWMouseCallback);
            glfwSetScrollCallback(window, GLFWScrollCallback);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            g_first_mouse = true;

            g_show_image_test = false;
        } break;

        case GLFW_KEY_O:
        {
            glfwSetCursorPosCallback(window, GLFWMouseCallback);
            glfwSetScrollCallback(window, GLFWScrollCallback);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            g_first_mouse = true;

            g_show_fft_ocean_test = true;
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

glm::vec3 GetRayDirectionFromNDC(const glm::vec2& ndc, const glm::mat4& view_proj_inverse, const glm::vec3& ray_origin)
{
    glm::vec4 point_world_space = view_proj_inverse * glm::vec4(ndc, 0.f, 1.f);
    point_world_space /= point_world_space.w;
    return glm::vec3(point_world_space) - ray_origin;
}

Mesh GetFullscreenQuad()
{
    float quad_vertices[] =
    {
        -1.f, -1.f,
         1.f, -1.f,
         1.f,  1.f,
        -1.f,  1.f
    };

    unsigned int quad_indices[] = { 0, 1, 2, 2, 3, 0 };

    return Mesh(quad_vertices, 8 * 2, quad_indices, 6);
}

struct Test
{
    virtual ~Test() {}

    virtual void SetWindowInput(GLFWwindow* window) const
    {
        glfwSetCursorPosCallback(window, GLFWMouseCallback);
        glfwSetScrollCallback(window, GLFWScrollCallback);
        glfwSetKeyCallback(window, GLFWKeyCallback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    virtual void SetUp() = 0;
    virtual void OnFrameBegin(GLFWwindow* window) const = 0;
    virtual void OnRender() const = 0;
};

struct TestRayTracing : public Test
{
    TestRayTracing() : fullscreen_quad_shader("Source/Shaders/FullscreenQuadShader.vert", "Source/Shaders/FullscreenQuadShader.frag"),
        raytracing_shader("Source/Shaders/Raytracing.comp"), fullscreen_quad(GetFullscreenQuad()),
        fb_texture(Texture2D(g_width, g_height, GL_RGBA32F, GL_RGBA, GL_FLOAT))
    {

    }

    void SetUp() override
    {
        // Todo: I don't think you need this piece of code here, since the first iteration of my main while loop would do exactly this!
        glm::mat4 view = glm::translate(glm::mat4(1.f), -g_camera_position);
        glm::mat4 proj = glm::perspective(glm::radians(g_camera_fov), (float)g_width / (float)g_height, 0.1f, 100.f);

        glm::mat4 view_proj_inv = glm::inverse(view) * glm::inverse(proj);

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

        work_group_size = std::make_unique<GLint[]>(3);
        glGetProgramiv(raytracing_shader.id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size.get());

        // Todo: I don't think it is required
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    }

    void OnFrameBegin(GLFWwindow* window) const override
    {
        float current_frame = (float)glfwGetTime();
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

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
    }

    void OnRender() const override
    {
        glm::mat4 view = glm::lookAt(g_camera_position, g_camera_position + g_camera_front, g_camera_orientation);
        glm::mat4 proj = glm::perspective(glm::radians(g_camera_fov), (float)g_width / (float)g_height, 0.1f, 100.f);

        glm::mat4 view_proj_inv = glm::inverse(view) * glm::inverse(proj);

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

        // Bind level 0 of the framebuffer texture to image binding point 0
        glBindImageTexture(0, fb_texture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        // Dispatch the compute shader to generate a frame in the framebuffer image
        glDispatchCompute(g_width / work_group_size[0], g_height / work_group_size[1], 1);

        // Unbind image binding point
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        fb_texture.Bind();

        fullscreen_quad_shader.Bind();
        fullscreen_quad.BindVAO();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    Shader fullscreen_quad_shader;
    Shader raytracing_shader;
    Mesh fullscreen_quad;

    // Todo: I think the OpenGL spec needs this to be an immutable storage
    Texture2D fb_texture;

    // Todo: You need it here? Probably???
    std::unique_ptr<GLint[]> work_group_size = nullptr;
};

struct TestImageProcessing : public Test
{
    TestImageProcessing() : blur_shader("Source/Shaders/GaussianBlur.comp"),
        fullscreen_quad_shader("Source/Shaders/FullscreenQuadShader.vert", "Source/Shaders/FullscreenQuadShader.frag"),
        fullscreen_quad(GetFullscreenQuad())
    {
        const char* image_path = "../Resources/container.jpg";
        int image_tex_width, image_tex_height, image_tex_channel_count;
        unsigned char* image_data = stbi_load(image_path, &image_tex_width, &image_tex_height, &image_tex_channel_count, 3);

        if (!image_data)
        {
            std::cout << "Failed to load image: " << image_path << std::endl;
            exit(1);
        }

        image_texture = std::make_unique<Texture2D>(image_tex_width, image_tex_height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
            GL_LINEAR, GL_LINEAR, image_data);

        stbi_image_free(image_data);

        fb_texture = std::make_unique<Texture2D>(image_tex_width, image_tex_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR);

        const int blur_kernel_width = 9;
        const float blur_kernel_sigma = 2.f;

        float* blur_kernel = (float*)malloc(blur_kernel_width * blur_kernel_width * sizeof(float));

        float filter_sum = 0.f;
        for (int y = -blur_kernel_width / 2; y <= blur_kernel_width / 2; ++y)
        {
            for (int x = -blur_kernel_width / 2; x <= blur_kernel_width / 2; ++x)
            {
                const float filter_value = expf(-float(x * x + y * y) / (2.f * blur_kernel_sigma * blur_kernel_sigma));

                unsigned int idx = (y + blur_kernel_width / 2) * blur_kernel_width + (x + blur_kernel_width / 2);
                blur_kernel[idx] = filter_value;

                filter_sum += filter_value;
            }
        }

        float normalization_factor = 1.f / filter_sum;
        for (int y = -blur_kernel_width / 2; y <= blur_kernel_width / 2; ++y)
        {
            for (int x = -blur_kernel_width / 2; x <= blur_kernel_width / 2; ++x)
            {
                unsigned int idx = (y + blur_kernel_width / 2) * blur_kernel_width + (x + blur_kernel_width / 2);
                blur_kernel[idx] *= normalization_factor;
            }
        }

        blur_kernel_texture = std::make_unique<Texture2D>(blur_kernel_width, blur_kernel_width, GL_R32F, GL_RED, GL_FLOAT,
            GL_NEAREST, GL_NEAREST, blur_kernel);

        free(blur_kernel);
    }

    void SetWindowInput(GLFWwindow* window) const override
    {
        glfwSetCursorPosCallback(window, 0);
        glfwSetScrollCallback(window, 0);
        glfwSetKeyCallback(window, GLFWKeyCallback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void SetUp() override
    {
        work_group_size = std::make_unique<GLint[]>(3);
        glGetProgramiv(blur_shader.id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size.get());
    }

    void OnFrameBegin(GLFWwindow* window) const override
    {

    }

    void OnRender() const override
    {
        blur_shader.Bind();

        blur_shader.SetInt("u_ImageSampler", 0);
        image_texture->Bind(0);

        blur_shader.SetInt("u_FilterSampler", 1);
        blur_kernel_texture->Bind(1);

        // Todo: Maybe you don't have to compute the blurred image every frame, but only when it is changed!

        // Bind level 0 of the framebuffer texture to image binding point 0
        glBindImageTexture(0, fb_texture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        // Dispatch the compute shader to generate a frame in the framebuffer image
        glDispatchCompute(image_texture->width / work_group_size[0], image_texture->height / work_group_size[1], 1);

        // Unbind image binding point
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        fb_texture->Bind();

        // Note: This will strech imageprocessing_fb_texture along the x axis and display it
        fullscreen_quad.BindVAO();
        fullscreen_quad_shader.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    Shader blur_shader;
    Shader fullscreen_quad_shader;
    Mesh fullscreen_quad;

    // Todo: I think the OpenGL spec needs this to be an immutable storage
    std::unique_ptr<Texture2D> fb_texture = nullptr;

    std::unique_ptr<Texture2D> image_texture = nullptr;
    std::unique_ptr<Texture2D> blur_kernel_texture = nullptr;

    // Todo: You need it here? Probably???
    std::unique_ptr<GLint[]> work_group_size = nullptr;
};

struct TestOceanFFT : public Test
{
    void SetUp() override
    {

    }

    void OnFrameBegin(GLFWwindow* window) const override
    {

    }

    void OnRender() const override
    {

    }
};

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

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Test* test = new TestRayTracing();
    test->SetWindowInput(window);

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

    test->SetUp();

    while (!glfwWindowShouldClose(window))
    {
        test->OnFrameBegin(window);

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        test->OnRender();

        glfwSwapBuffers(window);
    }

    delete test;
    glfwTerminate();

    return 0;
}
