#ifndef CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Ogle
{
struct Camera
{
    Camera(const glm::vec3& position_);

    inline glm::mat4 GetProjViewMatrix(float aspect_ratio) const
    {
        return glm::perspective(glm::radians(fov_y), aspect_ratio, 0.1f, 1000.f) * glm::lookAt(position, position + front, up);
    }

    void ProcessKeyboard(int key_code, float delta_time);
    void ProcessMouseMove(float x_offset, float y_offset);
    void ProcessMouseScroll(float vertical_offset);

private:
    void UpdateCameraVectors();

    glm::vec3 position;
    glm::vec3 front;        // Z
    glm::vec3 right;        // X
    glm::vec3 up;           // Y

    const glm::vec3 world_up = glm::vec3(0.f, 1.f, 0.f);
    const float sensitivity = 0.1f;

    float pitch = 0.f;      // X
    float yaw = -90.f;      // Y
    float fov_y = 60.f;
};
}   // namespace Ogle

#define CAMERA_H
#endif