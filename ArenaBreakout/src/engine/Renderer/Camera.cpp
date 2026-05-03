#include "Camera.h"

namespace ArenaBreakout {
namespace Engine {

Camera::Camera()
    : position_(0.0f, 0.0f, 3.0f)
    , front_(0.0f, 0.0f, -1.0f)
    , up_(0.0f, 1.0f, 0.0f)
    , right_(1.0f, 0.0f, 0.0f)
    , worldUp_(0.0f, 1.0f, 0.0f)
    , yaw_(-90.0f)
    , pitch_(0.0f)
    , moveSpeed_(5.0f)
    , mouseSensitivity_(0.1f)
    , fov_(45.0f)
    , aspectRatio_(16.0f / 9.0f)
    , nearPlane_(0.1f)
    , farPlane_(1000.0f)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix() {
    viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::UpdateProjectionMatrix() {
    projectionMatrix_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = moveSpeed_ * deltaTime;
    
    switch (direction) {
        case FORWARD:
            position_ += front_ * velocity;
            break;
        case BACKWARD:
            position_ -= front_ * velocity;
            break;
        case LEFT:
            position_ -= right_ * velocity;
            break;
        case RIGHT:
            position_ += right_ * velocity;
            break;
        case UP:
            position_ += worldUp_ * velocity;
            break;
        case DOWN:
            position_ -= worldUp_ * velocity;
            break;
    }
    
    UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw_ += xoffset;
    pitch_ += yoffset;
    
    // 限制俯仰角在 -89 到 89 度之间
    if (constrainPitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }
    
    // 更新前方向量
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    newFront.y = sin(glm::radians(pitch_));
    newFront.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    
    front_ = glm::normalize(newFront);
    
    // 重新计算右方和上方向量
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
    
    UpdateViewMatrix();
}

void Camera::ProcessMouseScroll(float yoffset) {
    fov_ -= yoffset;
    
    // 限制 FOV 在 1 到 90 度之间
    if (fov_ < 1.0f) fov_ = 1.0f;
    if (fov_ > 90.0f) fov_ = 90.0f;
    
    UpdateProjectionMatrix();
}

glm::vec3 Camera::GetFront() const {
    return front_;
}

} // namespace Engine
} // namespace ArenaBreakout
