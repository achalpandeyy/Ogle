#include "Camera.h"

#include <GLFW/glfw3.h>

namespace Ogle
{
Camera::Camera(const glm::vec3& position_, float near_, float far_, float movement_speed_, float mouse_sensitivity_)
    : position(position_), movement_speed(movement_speed_), mouse_sensitivity(mouse_sensitivity_), z_near(near_), z_far(far_)
{
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(int key_code, float delta_time)
{
    float delta_distance = movement_speed * delta_time;

    if (key_code == GLFW_KEY_W)
        position += front * delta_distance;
    if (key_code == GLFW_KEY_S)
        position -= front * delta_distance;
    if (key_code == GLFW_KEY_A)
        position -= right * delta_distance;
    if (key_code == GLFW_KEY_D)
        position += right * delta_distance;
}

void Camera::ProcessMouseMove(float x_offset, float y_offset)
{
    yaw += mouse_sensitivity * x_offset;
    pitch += mouse_sensitivity * y_offset;

    pitch = glm::clamp(pitch, -89.f, 89.f);

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float vertical_offset)
{
    fov_y -= vertical_offset;
    fov_y = glm::clamp(fov_y, 1.f, 45.f);
}

void Camera::UpdateCameraVectors()
{
    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
}   // namespace Ogle