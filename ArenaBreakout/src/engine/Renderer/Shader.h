#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

namespace ArenaBreakout {
namespace Engine {

/**
 * @brief 着色器类 - 封装 OpenGL 着色器程序
 * 使用 RAII 管理 GL 资源
 */
class Shader {
public:
    Shader();
    ~Shader();
    
    // 禁止拷贝
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // 允许移动
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    /**
     * @brief 从字符串创建着色器程序
     * @param vertexSource 顶点着色器源码
     * @param fragmentSource 片段着色器源码
     * @return true 如果编译成功
     */
    bool CreateFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    
    /**
     * @brief 从文件加载着色器程序
     * @param vertexPath 顶点着色器文件路径
     * @param fragmentPath 片段着色器文件路径
     * @return true 如果加载成功
     */
    bool CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    
    /**
     * @brief 使用此着色器程序
     */
    void Use() const;
    
    /**
     * @brief 销毁着色器程序
     */
    void Destroy();
    
    /**
     * @brief 获取程序 ID
     */
    unsigned int GetProgramID() const { return programID_; }
    
    /**
     * @brief 检查是否有效
     */
    bool IsValid() const { return programID_ != 0; }
    
    // Uniform 设置函数
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, float x, float y) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;
    void SetMat4(const std::string& name, const float* matrix) const;
    
private:
    unsigned int programID_;
    mutable std::unordered_map<std::string, int> uniformLocations_;
    
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string& name) const;
};

} // namespace Engine
} // namespace ArenaBreakout

#endif // SHADER_H
