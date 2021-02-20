#ifndef CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Ogle
{
struct Camera
{
    Camera(const glm::vec3& position_, float near_ = 0.1f, float far_ = 1000.f, float movement_speed_ = 10000.f,
        float mouse_sensitivity_ = 0.1f);

    inline glm::mat4 GetProjViewMatrix(float aspect_ratio) const
    {
        return glm::perspective(glm::radians(fov_y), aspect_ratio, z_near, z_far) * glm::lookAt(position, position + front, up);
    }

    // Todo: This keyboard movement is kind of choppy fix it ffs
    void ProcessKeyboard(int key_code, float delta_time);
    void ProcessMouseMove(float x_offset, float y_offset);
    void ProcessMouseScroll(float vertical_offset);

    glm::vec3 position;

private:
    void UpdateCameraVectors();

    glm::vec3 front;        // Z
    glm::vec3 right;        // X
    glm::vec3 up;           // Y

    const glm::vec3 world_up = glm::vec3(0.f, 1.f, 0.f);
    float movement_speed;
    float mouse_sensitivity;

    float pitch = 0.f;      // X
    float yaw = -90.f;      // Y
    float fov_y = 60.f;

    float z_near;
    float z_far;
};
}   // namespace Ogle

#define CAMERA_H
#endif
