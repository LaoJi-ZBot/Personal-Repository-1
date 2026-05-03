#include "Mesh.h"

#include <glad/glad.h>

namespace ArenaBreakout {
namespace Engine {

Mesh::Mesh() 
    : vao_(0)
    , vbo_(0)
    , ebo_(0) {
}

Mesh::~Mesh() {
    Destroy();
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao_(other.vao_)
    , vbo_(other.vbo_)
    , ebo_(other.ebo_)
    , vertices_(std::move(other.vertices_))
    , indices_(std::move(other.indices_))
    , material_(other.material_) {
    
    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Destroy();
        
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);
        material_ = other.material_;
        
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
    }
    return *this;
}

void Mesh::Destroy() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
        vao_ = 0;
        vbo_ = 0;
        ebo_ = 0;
    }
}

void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    vertices_ = vertices;
    indices_ = indices;
    
    if (vao_ != 0) {
        Destroy();
    }
    
    SetupMesh();
}

void Mesh::SetupMesh() {
    // 生成 VAO/VBO/EBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    Bind();
    
    // 绑定并填充顶点缓冲
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), 
                 vertices_.data(), GL_STATIC_DRAW);
    
    // 绑定并填充索引缓冲
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), 
                 indices_.data(), GL_STATIC_DRAW);
    
    // 设置顶点属性指针
    // position (vec3) - location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          reinterpret_cast<void*>(0));
    
    // normal (vec3) - location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    
    // texCoord (vec2) - location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    
    // color (vec4) - location 3
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                          reinterpret_cast<void*>(offsetof(Vertex, color)));
    
    Unbind();
}

void Mesh::Bind() const {
    if (vao_ != 0) {
        glBindVertexArray(vao_);
    }
}

void Mesh::Unbind() const {
    glBindVertexArray(0);
}

void Mesh::Render() const {
    if (vao_ == 0 || indices_.empty()) {
        return;
    }
    
    Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);
    Unbind();
}

} // namespace Engine
} // namespace ArenaBreakout
