#include "Shader.h"

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ArenaBreakout {
namespace Engine {

Shader::Shader() : programID_(0) {}

Shader::~Shader() {
    Destroy();
}

Shader::Shader(Shader&& other) noexcept 
    : programID_(other.programID_), uniformLocations_(std::move(other.uniformLocations_)) {
    other.programID_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        Destroy();
        programID_ = other.programID_;
        uniformLocations_ = std::move(other.uniformLocations_);
        other.programID_ = 0;
    }
    return *this;
}

void Shader::Destroy() {
    if (programID_ != 0) {
        glDeleteProgram(programID_);
        programID_ = 0;
        uniformLocations_.clear();
    }
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[ERROR] Shader compilation failed (" 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
                  << "): " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool Shader::CreateFromStrings(const std::string& vertexSource, const std::string& fragmentSource) {
    if (programID_ != 0) {
        Destroy();
    }
    
    // 编译顶点着色器
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        return false;
    }
    
    // 编译片段着色器
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // 链接程序
    programID_ = glCreateProgram();
    glAttachShader(programID_, vertexShader);
    glAttachShader(programID_, fragmentShader);
    glLinkProgram(programID_);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(programID_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID_, 512, nullptr, infoLog);
        std::cerr << "[ERROR] Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(programID_);
        programID_ = 0;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    // 删除着色器对象 (已链接到程序)
    glDetachShader(programID_, vertexShader);
    glDetachShader(programID_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    std::cout << "[INFO] Shader program created successfully (ID: " << programID_ << ")" << std::endl;
    
    return true;
}

bool Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream vFile, fFile;
    
    try {
        vFile.open(vertexPath);
        fFile.open(fragmentPath);
        
        if (!vFile.is_open() || !fFile.is_open()) {
            std::cerr << "[ERROR] Could not open shader files: " << vertexPath << ", " << fragmentPath << std::endl;
            return false;
        }
        
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        
        vFile.close();
        fFile.close();
        
        return CreateFromStrings(vStream.str(), fStream.str());
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to load shader files: " << e.what() << std::endl;
        return false;
    }
}

void Shader::Use() const {
    if (programID_ != 0) {
        glUseProgram(programID_);
    }
}

int Shader::GetUniformLocation(const std::string& name) const {
    auto it = uniformLocations_.find(name);
    if (it != uniformLocations_.end()) {
        return it->second;
    }
    
    int location = glGetUniformLocation(programID_, name.c_str());
    if (location == -1) {
        std::cerr << "[WARN] Uniform '" << name << "' not found in shader program" << std::endl;
    }
    
    uniformLocations_[name] = location;
    return location;
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, float x, float y) const {
    glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat4(const std::string& name, const float* matrix) const {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
}

} // namespace Engine
} // namespace ArenaBreakout
