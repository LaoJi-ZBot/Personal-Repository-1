#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace ArenaBreakout {
namespace Engine {

/**
 * @brief 顶点结构
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 color;
    
    Vertex() : position(0.0f), normal(0.0f, 1.0f, 0.0f), texCoord(0.0f), color(1.0f) {}
};

/**
 * @brief 材质属性
 */
struct Material {
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;
    
    Material() 
        : diffuseColor(0.5f, 0.5f, 0.5f)
        , specularColor(0.2f, 0.2f, 0.2f)
        , shininess(32.0f) {}
};

/**
 * @brief 网格类 - 封装 VAO/VBO/EBO
 * 使用 RAII 管理 GL 资源
 */
class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // 禁止拷贝
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    // 允许移动
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    
    /**
     * @brief 设置网格数据
     * @param vertices 顶点数组
     * @param indices 索引数组
     */
    void SetData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    
    /**
     * @brief 绑定 VAO 准备渲染
     */
    void Bind() const;
    
    /**
     * @brief 解绑 VAO
     */
    void Unbind() const;
    
    /**
     * @brief 渲染网格
     */
    void Render() const;
    
    /**
     * @brief 获取顶点数量
     */
    size_t GetVertexCount() const { return vertices_.size(); }
    
    /**
     * @brief 获取索引数量
     */
    size_t GetIndexCount() const { return indices_.size(); }
    
    /**
     * @brief 设置材质
     */
    void SetMaterial(const Material& mat) { material_ = mat; }
    
    /**
     * @brief 获取材质
     */
    const Material& GetMaterial() const { return material_; }
    
    /**
     * @brief 销毁网格资源
     */
    void Destroy();
    
    /**
     * @brief 检查是否有效
     */
    bool IsValid() const { return vao_ != 0; }
    
private:
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int ebo_;
    
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    Material material_;
    
    void SetupMesh();
};

// 使用 shared_ptr 管理 Mesh
using MeshPtr = std::shared_ptr<Mesh>;

} // namespace Engine
} // namespace ArenaBreakout

#endif // MESH_H
