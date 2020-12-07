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

glm::vec3 camera_position(0.f, 0.f, 3.f);
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
float camera_fov = 45.f;
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
    camera_fov -= y_offset;
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

    // Todo: You can use the GL_TRIANGLE_STRIP primitive, that way, you won't need
    // to store this many vertices for a cube
    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

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
    }

    glViewport(0, 0, width, height);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    const char* tex_path = "../Resources/container.jpg";
    int tex_width, tex_height, tex_channel_count;
    unsigned char* tex_data = stbi_load(tex_path, &tex_width, &tex_height, &tex_channel_count, 0);
    assert(tex_channel_count == 3);
    if (!tex_data)
    {
        std::cout << "Failed to load texture at " << tex_path << std::endl;
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(tex_data);

    Shader shader("Source/Shaders/Shader.vert", "Source/Shaders/Shader.frag");
    shader.Bind();

    glm::mat4 model = glm::mat4(1.f);

    glm::mat4 view = glm::translate(glm::mat4(1.f), -camera_position);
    glm::mat4 proj = glm::perspective(glm::radians(camera_fov), (float)width / (float)height, 0.1f, 100.f);

    glm::mat4 pvm = proj * view * model;
    shader.SetMat4("pvm", glm::value_ptr(pvm), false);

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
        pvm = proj * view * model;

        shader.Bind();
        shader.SetMat4("pvm", glm::value_ptr(pvm), false);

        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture);

    glfwTerminate();
    return 0;
}
