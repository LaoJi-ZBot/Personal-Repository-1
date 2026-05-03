#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

namespace ArenaBreakout {
namespace Engine {

Renderer& Renderer::Instance() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer()
    : fps_(0.0f)
    , frameCount_(0)
    , fpsTimer_(0.0f)
    , initialized_(false)
    , viewportWidth_(1920)
    , viewportHeight_(1080) {
    
    clearColor_[0] = 0.15f;
    clearColor_[1] = 0.15f;
    clearColor_[2] = 0.15f;
    clearColor_[3] = 1.0f;
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[INFO] Initializing Renderer..." << std::endl;
    
    // 创建默认着色器
    CreateDefaultShader();
    
    // 创建测试场景
    CreateTestScene();
    
    // 创建相机
    camera_ = std::make_shared<Camera>();
    camera_->SetPosition(glm::vec3(0.0f, 2.0f, 8.0f));
    
    // 添加一个点光源
    PointLight light;
    light.position = glm::vec3(2.0f, 4.0f, 2.0f);
    AddPointLight(light);
    
    initialized_ = true;
    
    std::cout << "[INFO] Renderer initialized successfully" << std::endl;
    
    return true;
}

void Renderer::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[INFO] Shutting down Renderer..." << std::endl;
    
    renderBatches_.clear();
    floorMesh_.reset();
    cubeMesh_.reset();
    checkerTexture_.reset();
    defaultShader_.reset();
    camera_.reset();
    
    initialized_ = false;
}

void Renderer::CreateDefaultShader() {
    // 顶点着色器源码 - 冯氏光照模型
    const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    VertexColor = aColor;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

    // 片段着色器源码 - 冯氏光照 + 纹理采样
    const char* fragmentSource = R"(
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 VertexColor;

out vec4 FragColor;

// 材质属性
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

// 方向光
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight directionalLight;

// 点光源 (最多支持 4 个)
#define NR_POINT_LIGHTS 4
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int numPointLights;

// 纹理
uniform sampler2D texture1;
uniform bool useTexture;

// 计算方向光贡献
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    // 组合结果
    vec3 ambient = light.ambient * materialDiffuse;
    vec3 diffuse = light.diffuse * diff * materialDiffuse;
    vec3 specular = light.specular * spec * materialSpecular;
    return (ambient + diffuse + specular);
}

// 计算点光源贡献
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * (distance * distance));
    // 组合结果
    vec3 ambient = light.ambient * materialDiffuse;
    vec3 diffuse = light.diffuse * diff * materialDiffuse;
    vec3 specular = light.specular * spec * materialSpecular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    
    // 方向光
    vec3 result = CalcDirLight(directionalLight, norm, viewDir);
    
    // 点光源
    for(int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    
    // 纹理采样
    vec4 texColor = vec4(1.0);
    if(useTexture) {
        texColor = texture(texture1, TexCoord);
    }
    
    // 最终颜色 = 光照 * 纹理 * 顶点颜色
    FragColor = vec4(result, 1.0) * texColor * VertexColor;
}
)";

    defaultShader_ = std::make_unique<Shader>();
    if (!defaultShader_->CreateFromStrings(vertexSource, fragmentSource)) {
        std::cerr << "[ERROR] Failed to create default shader" << std::endl;
        return;
    }
    
    std::cout << "[INFO] Default shader created" << std::endl;
}

void Renderer::CreateTestScene() {
    // 创建棋盘格纹理
    checkerTexture_ = std::make_shared<Texture>();
    float color1[3] = {0.2f, 0.2f, 0.2f};  // 深灰
    float color2[3] = {0.4f, 0.4f, 0.4f};  // 浅灰
    checkerTexture_->CreateCheckerboard(512, 32, color1, color2);
    
    // 创建地板网格
    floorMesh_ = std::make_shared<Mesh>();
    std::vector<Vertex> floorVertices;
    std::vector<unsigned int> floorIndices;
    
    // 地板：10x10 单位
    float floorSize = 10.0f;
    floorVertices.resize(4);
    floorVertices[0].position = glm::vec3(-floorSize, 0.0f, -floorSize);
    floorVertices[0].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    floorVertices[0].texCoord = glm::vec2(0.0f, 0.0f);
    floorVertices[0].color = glm::vec4(1.0f);
    
    floorVertices[1].position = glm::vec3(floorSize, 0.0f, -floorSize);
    floorVertices[1].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    floorVertices[1].texCoord = glm::vec2(5.0f, 0.0f);
    floorVertices[1].color = glm::vec4(1.0f);
    
    floorVertices[2].position = glm::vec3(floorSize, 0.0f, floorSize);
    floorVertices[2].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    floorVertices[2].texCoord = glm::vec2(5.0f, 5.0f);
    floorVertices[2].color = glm::vec4(1.0f);
    
    floorVertices[3].position = glm::vec3(-floorSize, 0.0f, floorSize);
    floorVertices[3].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    floorVertices[3].texCoord = glm::vec2(0.0f, 5.0f);
    floorVertices[3].color = glm::vec4(1.0f);
    
    floorIndices = {0, 1, 2, 2, 3, 0};
    floorMesh_->SetData(floorVertices, floorIndices);
    
    Material floorMat;
    floorMat.diffuseColor = glm::vec3(0.5f);
    floorMat.specularColor = glm::vec3(0.1f);
    floorMat.shininess = 16.0f;
    floorMesh_->SetMaterial(floorMat);
    
    // 创建立方体网格
    cubeMesh_ = std::make_shared<Mesh>();
    std::vector<Vertex> cubeVertices;
    std::vector<unsigned int> cubeIndices;
    
    // 简单的立方体 (1x1x1)
    float s = 0.5f;
    cubeVertices.resize(8);
    cubeVertices[0].position = glm::vec3(-s, -s, -s); cubeVertices[0].normal = glm::vec3(0, 0, -1); cubeVertices[0].texCoord = glm::vec2(0, 0);
    cubeVertices[1].position = glm::vec3( s, -s, -s); cubeVertices[1].normal = glm::vec3(0, 0, -1); cubeVertices[1].texCoord = glm::vec2(1, 0);
    cubeVertices[2].position = glm::vec3( s,  s, -s); cubeVertices[2].normal = glm::vec3(0, 0, -1); cubeVertices[2].texCoord = glm::vec2(1, 1);
    cubeVertices[3].position = glm::vec3(-s,  s, -s); cubeVertices[3].normal = glm::vec3(0, 0, -1); cubeVertices[3].texCoord = glm::vec2(0, 1);
    cubeVertices[4].position = glm::vec3(-s, -s,  s); cubeVertices[4].normal = glm::vec3(0, 0, 1); cubeVertices[4].texCoord = glm::vec2(0, 0);
    cubeVertices[5].position = glm::vec3( s, -s,  s); cubeVertices[5].normal = glm::vec3(0, 0, 1); cubeVertices[5].texCoord = glm::vec2(1, 0);
    cubeVertices[6].position = glm::vec3( s,  s,  s); cubeVertices[6].normal = glm::vec3(0, 0, 1); cubeVertices[6].texCoord = glm::vec2(1, 1);
    cubeVertices[7].position = glm::vec3(-s,  s,  s); cubeVertices[7].normal = glm::vec3(0, 0, 1); cubeVertices[7].texCoord = glm::vec2(0, 1);
    
    // 前面
    cubeIndices.insert(cubeIndices.end(), {0, 1, 2, 2, 3, 0});
    // 后面
    cubeIndices.insert(cubeIndices.end(), {1, 0, 4, 4, 5, 1});
    // 左面
    cubeIndices.insert(cubeIndices.end(), {3, 2, 6, 6, 7, 3});
    // 右面
    cubeIndices.insert(cubeIndices.end(), {5, 4, 7, 7, 6, 5});
    // 上面
    cubeIndices.insert(cubeIndices.end(), {4, 0, 3, 3, 7, 4});
    // 下面
    cubeIndices.insert(cubeIndices.end(), {1, 5, 6, 6, 2, 1});
    
    cubeMesh_->SetData(cubeVertices, cubeIndices);
    
    Material cubeMat;
    cubeMat.diffuseColor = glm::vec3(0.3f, 0.5f, 0.3f);  // 军事绿
    cubeMat.specularColor = glm::vec3(0.2f);
    cubeMat.shininess = 32.0f;
    cubeMesh_->SetMaterial(cubeMat);
    
    std::cout << "[INFO] Test scene created (floor + cube)" << std::endl;
}

void Renderer::BeginFrame() {
    // 清屏
    glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 设置视口
    glViewport(0, 0, viewportWidth_, viewportHeight_);
    
    // 清除渲染批次
    renderBatches_.clear();
}

void Renderer::EndFrame() {
    // 渲染场景
    RenderScene();
    
    // 更新 FPS
    UpdateFPS();
}

void Renderer::SubmitMesh(const MeshPtr& mesh, const glm::mat4& model) {
    if (!mesh || !mesh->IsValid()) {
        return;
    }
    
    RenderBatch batch;
    batch.mesh = mesh;
    batch.model = model;
    renderBatches_.push_back(batch);
}

void Renderer::SetCamera(const std::shared_ptr<Camera>& camera) {
    camera_ = camera;
}

void Renderer::AddPointLight(const PointLight& light) {
    if (pointLights_.size() < 4) {
        pointLights_.push_back(light);
    }
}

void Renderer::ClearPointLights() {
    pointLights_.clear();
}

void Renderer::SetClearColor(float r, float g, float b, float a) {
    clearColor_[0] = r;
    clearColor_[1] = g;
    clearColor_[2] = b;
    clearColor_[3] = a;
}

void Renderer::EnableBlending(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void Renderer::SetViewport(int width, int height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
}

void Renderer::RenderScene() {
    if (!defaultShader_ || !camera_) {
        return;
    }
    
    defaultShader_->Use();
    
    // 设置矩阵 uniforms
    glm::mat4 view = camera_->GetViewMatrix();
    glm::mat4 projection = camera_->GetProjectionMatrix();
    
    defaultShader_->SetMat4("view", glm::value_ptr(view));
    defaultShader_->SetMat4("projection", glm::value_ptr(projection));
    
    // 设置方向光
    defaultShader_->SetVec3("directionalLight.direction", 
                            directionalLight_.direction.x, 
                            directionalLight_.direction.y, 
                            directionalLight_.direction.z);
    defaultShader_->SetVec3("directionalLight.ambient", 
                            directionalLight_.ambient.x, 
                            directionalLight_.ambient.y, 
                            directionalLight_.ambient.z);
    defaultShader_->SetVec3("directionalLight.diffuse", 
                            directionalLight_.diffuse.x, 
                            directionalLight_.diffuse.y, 
                            directionalLight_.diffuse.z);
    defaultShader_->SetVec3("directionalLight.specular", 
                            directionalLight_.specular.x, 
                            directionalLight_.specular.y, 
                            directionalLight_.specular.z);
    
    // 设置点光源
    defaultShader_->SetInt("numPointLights", static_cast<int>(pointLights_.size()));
    for (size_t i = 0; i < pointLights_.size(); ++i) {
        std::string name = "pointLights[" + std::to_string(i) + "].";
        auto& light = pointLights_[i];
        defaultShader_->SetVec3(name + "position", light.position.x, light.position.y, light.position.z);
        defaultShader_->SetVec3(name + "ambient", light.ambient.x, light.ambient.y, light.ambient.z);
        defaultShader_->SetVec3(name + "diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
        defaultShader_->SetVec3(name + "specular", light.specular.x, light.specular.y, light.specular.z);
        defaultShader_->SetFloat(name + "constant", light.constant);
        defaultShader_->SetFloat(name + "linear", light.linear);
        defaultShader_->SetFloat(name + "quadratic", light.quadratic);
    }
    
    // 绑定纹理
    checkerTexture_->Bind(0);
    defaultShader_->SetInt("texture1", 0);
    defaultShader_->SetBool("useTexture", true);
    
    // 渲染所有批次
    for (auto& batch : renderBatches_) {
        // 设置材质 uniforms
        auto& mat = batch.mesh->GetMaterial();
        defaultShader_->SetVec3("materialDiffuse", mat.diffuseColor.r, mat.diffuseColor.g, mat.diffuseColor.b);
        defaultShader_->SetVec3("materialSpecular", mat.specularColor.r, mat.specularColor.g, mat.specularColor.b);
        defaultShader_->SetFloat("materialShininess", mat.shininess);
        
        // 设置模型矩阵
        defaultShader_->SetMat4("model", glm::value_ptr(batch.model));
        
        // 渲染网格
        batch.mesh->Render();
    }
}

void Renderer::UpdateFPS() {
    frameCount_++;
    fpsTimer_ += 0.016f;  // 假设约 60fps
    
    if (fpsTimer_ >= 1.0f) {
        fps_ = static_cast<float>(frameCount_) / fpsTimer_;
        frameCount_ = 0;
        fpsTimer_ = 0.0f;
        
        // 更新窗口标题显示 FPS
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "暗区突围 | Arena Breakout - FPS: " << fps_;
        // 注意：这里需要通过 WindowManager 来更新标题
    }
}

} // namespace Engine
} // namespace ArenaBreakout
