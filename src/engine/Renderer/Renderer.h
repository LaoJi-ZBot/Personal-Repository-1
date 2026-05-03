#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace ArenaBreakout {
namespace Engine {

class Shader;
class Mesh;
class Texture;
class Camera;

using MeshPtr = std::shared_ptr<Mesh>;
using TexturePtr = std::shared_ptr<Texture>;

/**
 * @brief 光源类型
 */
enum class LightType {
    Directional,
    Point
};

/**
 * @brief 方向光
 */
struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    DirectionalLight() 
        : direction(-0.2f, -1.0f, -0.3f)
        , ambient(0.1f, 0.1f, 0.15f)  // 轻微蓝灰色环境光
        , diffuse(0.6f, 0.6f, 0.6f)
        , specular(0.3f, 0.3f, 0.3f) {}
};

/**
 * @brief 点光源
 */
struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    
    PointLight() 
        : position(0.0f, 5.0f, 0.0f)
        , ambient(0.05f, 0.05f, 0.05f)
        , diffuse(0.8f, 0.8f, 0.7f)
        , specular(0.5f, 0.5f, 0.5f)
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f) {}
};

/**
 * @brief 渲染器类 - 管理 OpenGL 渲染管线
 * 使用单例模式
 */
class Renderer {
public:
    static Renderer& Instance();
    
    // 禁止拷贝和移动
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    
    /**
     * @brief 初始化渲染器
     * @return true 如果初始化成功
     */
    bool Initialize();
    
    /**
     * @brief 销毁渲染器并清理资源
     */
    void Shutdown();
    
    /**
     * @brief 开始渲染帧
     */
    void BeginFrame();
    
    /**
     * @brief 结束渲染帧
     */
    void EndFrame();
    
    /**
     * @brief 提交网格进行渲染
     * @param mesh 网格对象
     * @param model 模型矩阵
     */
    void SubmitMesh(const MeshPtr& mesh, const glm::mat4& model);
    
    /**
     * @brief 设置相机
     */
    void SetCamera(const std::shared_ptr<Camera>& camera);
    
    /**
     * @brief 获取相机
     */
    std::shared_ptr<Camera> GetCamera() const { return camera_; }
    
    /**
     * @brief 设置方向光
     */
    void SetDirectionalLight(const DirectionalLight& light) { directionalLight_ = light; }
    
    /**
     * @brief 获取方向光
     */
    const DirectionalLight& GetDirectionalLight() const { return directionalLight_; }
    
    /**
     * @brief 添加点光源
     */
    void AddPointLight(const PointLight& light);
    
    /**
     * @brief 清除所有点光源
     */
    void ClearPointLights();
    
    /**
     * @brief 获取默认着色器
     */
    Shader* GetDefaultShader() const { return defaultShader_.get(); }
    
    /**
     * @brief 设置清屏颜色
     */
    void SetClearColor(float r, float g, float b, float a = 1.0f);
    
    /**
     * @brief 启用/禁用混合模式
     */
    void EnableBlending(bool enable);
    
    /**
     * @brief 设置视口大小
     */
    void SetViewport(int width, int height);
    
    /**
     * @brief 获取 FPS
     */
    float GetFPS() const { return fps_; }
    
private:
    Renderer();
    ~Renderer();
    
    void CreateDefaultShader();
    void CreateTestScene();
    void RenderScene();
    void UpdateFPS();
    
    std::unique_ptr<Shader> defaultShader_;
    std::shared_ptr<Camera> camera_;
    
    DirectionalLight directionalLight_;
    std::vector<PointLight> pointLights_;
    
    // 测试场景网格
    MeshPtr floorMesh_;
    MeshPtr cubeMesh_;
    TexturePtr checkerTexture_;
    
    // 渲染批次
    struct RenderBatch {
        MeshPtr mesh;
        glm::mat4 model;
    };
    std::vector<RenderBatch> renderBatches_;
    
    // 清屏颜色
    float clearColor_[4];
    
    // 视口
    int viewportWidth_;
    int viewportHeight_;
    
    // FPS 计算
    float fps_;
    int frameCount_;
    float fpsTimer_;
    
    bool initialized_;
};

} // namespace Engine
} // namespace ArenaBreakout

#endif // RENDERER_H
