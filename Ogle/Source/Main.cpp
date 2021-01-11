#include "Shader.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <iostream>

glm::vec3 g_camera_position(3.f, 2.f, 7.f);
glm::vec3 g_camera_front = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 g_camera_orientation(0.f, 1.f, 0.f);

float g_delta_time = 0.f;
float g_last_frame = 0.f;

// Todo: Fix this hardcode ASAP 
double g_last_x = (double)1280 / 2;
double g_last_y = (double)720 / 2;
float g_yaw = -90.f;
float g_pitch = 0.f;
float g_camera_fov = 60.f;
bool g_first_mouse = true;

glm::vec3 GetRayDirectionFromNDC(const glm::vec2& ndc, const glm::mat4& view_proj_inverse, const glm::vec3& ray_origin)
{
    glm::vec4 point_world_space = view_proj_inverse * glm::vec4(ndc, 0.f, 1.f);
    point_world_space /= point_world_space.w;
    return glm::vec3(point_world_space) - ray_origin;
}

std::unique_ptr<Mesh> GetFullscreenQuad()
{
    float quad_vertices[] =
    {
        -1.f, -1.f,
         1.f, -1.f,
         1.f,  1.f,
        -1.f,  1.f
    };

    unsigned int quad_indices[] = { 0, 1, 2, 2, 3, 0 };

    return std::make_unique<Mesh>(quad_vertices, 8 * 2, quad_indices, 6);
}

struct RayTracing final : public Application
{
    RayTracing()
    {
        settings.width = 1280;
        settings.height = 720;
        settings.window_title = "Ray Tracing | Ogle";
        settings.enable_cursor = false;
        settings.enable_debug_callback = true;
    }

    void Initialize() override
    {
        raytracing_cs = std::make_unique<Shader>("Source/Shaders/Raytracing.comp");
        fullscreen_quad_prog = std::make_unique<Shader>("Source/Shaders/FullscreenQuadShader.vert",
            "Source/Shaders/FullscreenQuadShader.frag");
        fb_texture = std::make_unique<Texture2D>(settings.width, settings.height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
        fullscreen_quad = GetFullscreenQuad();

        work_group_size = std::make_unique<GLint[]>(3);
        glGetProgramiv(raytracing_cs->id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size.get());
    }

    void Update() override
    {
        float delta_distance_front = camera_speed_front * delta_time;
        float delta_distance_right = camera_speed_right * delta_time;

        g_camera_position += delta_distance_front * g_camera_front;

        // Todo: Make camera_right global (later, a member) as well
        glm::vec3 camera_right = glm::normalize(glm::cross(g_camera_front, g_camera_orientation));
        g_camera_position += delta_distance_right * camera_right;

        glm::mat4 view = glm::lookAt(g_camera_position, g_camera_position + g_camera_front, g_camera_orientation);
        glm::mat4 proj = glm::perspective(glm::radians(g_camera_fov), (float)settings.width / (float)settings.height, 0.1f, 100.f);

        glm::mat4 view_proj_inv = glm::inverse(view) * glm::inverse(proj);

        raytracing_cs->Bind();

        raytracing_cs->SetVec3("u_Eye", g_camera_position.x, g_camera_position.y, g_camera_position.z);

        glm::vec3 ray_direction00 = GetRayDirectionFromNDC(glm::vec2(-1.f, -1.f), view_proj_inv, g_camera_position);
        raytracing_cs->SetVec3("u_RayDirection00", ray_direction00.x, ray_direction00.y, ray_direction00.z);

        glm::vec3 ray_direction01 = GetRayDirectionFromNDC(glm::vec2(-1.f, 1.f), view_proj_inv, g_camera_position);
        raytracing_cs->SetVec3("u_RayDirection01", ray_direction01.x, ray_direction01.y, ray_direction01.z);

        glm::vec3 ray_direction10 = GetRayDirectionFromNDC(glm::vec2(1.f, -1.f), view_proj_inv, g_camera_position);
        raytracing_cs->SetVec3("u_RayDirection10", ray_direction10.x, ray_direction10.y, ray_direction10.z);

        glm::vec3 ray_direction11 = GetRayDirectionFromNDC(glm::vec2(1.f, 1.f), view_proj_inv, g_camera_position);
        raytracing_cs->SetVec3("u_RayDirection11", ray_direction11.x, ray_direction11.y, ray_direction11.z);

        // Bind level 0 of the framebuffer texture to image binding point 0
        glBindImageTexture(0, fb_texture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        // Dispatch the compute shader to generate a frame in the framebuffer image
        glDispatchCompute(settings.width / work_group_size[0], settings.height / work_group_size[1], 1);

        // Unbind image binding point
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        fb_texture->Bind();

        fullscreen_quad_prog->Bind();
        fullscreen_quad->BindVAO();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void OnKeyPress(int key_code) override
    {
        if (key_code == GLFW_KEY_W)
            camera_speed_front = 10.f;
        else if (key_code == GLFW_KEY_S)
            camera_speed_front = -10.f;

        if (key_code == GLFW_KEY_D)
            camera_speed_right = 10.f;
        else if (key_code == GLFW_KEY_A)
            camera_speed_right = -10.f;
    }

    void OnKeyRelease(int key_code) override
    {
        if (key_code == GLFW_KEY_W || key_code == GLFW_KEY_S)
            camera_speed_front = 0.f;
        
        if (key_code == GLFW_KEY_D || key_code == GLFW_KEY_A)
            camera_speed_right = 0.f;
    }

    void OnMouseMove(double x, double y) override
    {
        if (g_first_mouse)
        {
            g_last_x = x;
            g_last_y = y;
            g_first_mouse = false;
        }

        float x_offset = (float)(x- g_last_x);
        float y_offset = (float)(g_last_y - y);

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

        g_last_x = x;
        g_last_y = y;
    }

    void OnMouseScroll(double vertical_offset) override
    {
        g_camera_fov -= (float)vertical_offset;
        g_camera_fov = glm::clamp(g_camera_fov, 1.f, 45.f);
    }

private:
    std::unique_ptr<Shader> raytracing_cs = nullptr;
    std::unique_ptr<Shader> fullscreen_quad_prog = nullptr;
    std::unique_ptr<Texture2D> fb_texture = nullptr; // Todo: I think the OpenGL spec wants this to be an immutable storage
    std::unique_ptr<Mesh> fullscreen_quad = nullptr;
    std::unique_ptr<GLint[]> work_group_size = nullptr;

    float camera_speed_front = 0.f;
    float camera_speed_right = 0.f;
};

struct ImageProcessing final : public Application
{
    ImageProcessing()
    {
        settings.width = 1280;
        settings.height = 720;
        settings.window_title = "Image Processing | Ogle";
        settings.enable_cursor = true;
        settings.enable_debug_callback = true;
    }

    void Initialize() override
    {
        blur_cs = std::make_unique<Shader>("Source/Shaders/GaussianBlur.comp");
        fullscreen_quad_prog = std::make_unique<Shader>("Source/Shaders/FullscreenQuadShader.vert",
            "Source/Shaders/FullscreenQuadShader.frag");
        fullscreen_quad = GetFullscreenQuad();

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

        work_group_size = std::make_unique<GLint[]>(3);
        glGetProgramiv(blur_cs->id, GL_COMPUTE_WORK_GROUP_SIZE, work_group_size.get());
    }

    void Update() override
    {
        blur_cs->Bind();

        blur_cs->SetInt("u_ImageSampler", 0);
        image_texture->Bind(0);

        blur_cs->SetInt("u_FilterSampler", 1);
        blur_kernel_texture->Bind(1);

        // Bind level 0 of the framebuffer texture to image binding point 0
        glBindImageTexture(0, fb_texture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        // Dispatch the compute shader to generate a frame in the framebuffer image
        glDispatchCompute(image_texture->width / work_group_size[0], image_texture->height / work_group_size[1], 1);

        // Unbind image binding point
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

        fb_texture->Bind();

        fullscreen_quad->BindVAO();
        fullscreen_quad_prog->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    std::unique_ptr<Shader> blur_cs = nullptr;
    std::unique_ptr<Shader> fullscreen_quad_prog = nullptr;
    std::unique_ptr<Mesh> fullscreen_quad = nullptr;
    std::unique_ptr<Texture2D> image_texture = nullptr;
    std::unique_ptr<Texture2D> fb_texture = nullptr; // Todo: I think the OpenGL spec wants it to be an immutable storage
    std::unique_ptr<Texture2D> blur_kernel_texture = nullptr;
    std::unique_ptr<GLint[]> work_group_size = nullptr;
};

int main()
{
    ImageProcessing ip;
    return ip.Run();

    // RayTracing rt;
    // return rt.Run();
}