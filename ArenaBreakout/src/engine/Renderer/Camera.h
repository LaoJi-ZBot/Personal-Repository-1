#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ArenaBreakout {
namespace Engine {

/**
 * @brief 相机类 - 第一人称视角 (FPS Camera)
 */
class Camera {
public:
    Camera();
    
    /**
     * @brief 更新视图矩阵
     */
    void UpdateViewMatrix();
    
    /**
     * @brief 处理键盘输入移动相机
     * @param direction 移动方向 (WASD)
     * @param deltaTime 时间增量
     */
    void ProcessKeyboard(int direction, float deltaTime);
    
    /**
     * @brief 处理鼠标移动改变视角
     * @param xoffset X 轴偏移
     * @param yoffset Y 轴偏移
     * @param constrainPitch 是否限制俯仰角
     */
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    
    /**
     * @brief 处理鼠标滚轮缩放 FOV
     * @param yoffset 滚轮偏移
     */
    void ProcessMouseScroll(float yoffset);
    
    /**
     * @brief 获取视图矩阵
     */
    const glm::mat4& GetViewMatrix() const { return viewMatrix_; }
    
    /**
     * @brief 获取投影矩阵
     */
    const glm::mat4& GetProjectionMatrix() const { return projectionMatrix_; }
    
    /**
     * @brief 获取相机位置
     */
    const glm::vec3& GetPosition() const { return position_; }
    
    /**
     * @brief 设置相机位置
     */
    void SetPosition(const glm::vec3& pos) { position_ = pos; }
    
    /**
     * @brief 获取相机前方方向
     */
    glm::vec3 GetFront() const;
    
    /**
     * @brief 设置 FOV
     */
    void SetFOV(float fov) { 
        fov_ = fov; 
        UpdateProjectionMatrix();
    }
    
    /**
     * @brief 设置宽高比
     */
    void SetAspectRatio(float aspect) { 
        aspectRatio_ = aspect; 
        UpdateProjectionMatrix();
    }
    
    /**
     * @brief 设置鼠标灵敏度
     */
    void SetSensitivity(float sens) { mouseSensitivity_ = sens; }
    
    /**
     * @brief 获取鼠标灵敏度
     */
    float GetSensitivity() const { return mouseSensitivity_; }
    
    // 相机方向常量
    enum Direction {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    
private:
    void UpdateProjectionMatrix();
    
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;
    
    glm::mat4 viewMatrix_;
    glm::mat4 projectionMatrix_;
    
    // 欧拉角
    float yaw_;   // 偏航角 (左右)
    float pitch_; // 俯仰角 (上下)
    
    // 相机参数
    float moveSpeed_;
    float mouseSensitivity_;
    float fov_;
    float aspectRatio_;
    float nearPlane_;
    float farPlane_;
};

} // namespace Engine
} // namespace ArenaBreakout

#endif // CAMERA_H
