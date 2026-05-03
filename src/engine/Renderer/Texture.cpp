#include "Texture.h"

#include <glad/glad.h>
#include <iostream>

// 如果 stb_image 可用则包含
#ifdef HAVE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

namespace ArenaBreakout {
namespace Engine {

Texture::Texture() 
    : textureID_(0)
    , width_(0)
    , height_(0) {
}

Texture::~Texture() {
    Destroy();
}

Texture::Texture(Texture&& other) noexcept
    : textureID_(other.textureID_)
    , width_(other.width_)
    , height_(other.height_) {
    
    other.textureID_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        Destroy();
        
        textureID_ = other.textureID_;
        width_ = other.width_;
        height_ = other.height_;
        
        other.textureID_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }
    return *this;
}

void Texture::Destroy() {
    if (textureID_ != 0) {
        glDeleteTextures(1, &textureID_);
        textureID_ = 0;
        width_ = 0;
        height_ = 0;
    }
}

bool Texture::LoadFromFile(const std::string& path, bool generateMipmap) {
#ifdef HAVE_STB_IMAGE
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width_, &height_, &channels, 4);
    
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
        return false;
    }
    
    bool success = CreateFromData(data, width_, height_, generateMipmap);
    stbi_image_free(data);
    
    return success;
#else
    std::cerr << "[ERROR] stb_image not available, cannot load texture from file: " << path << std::endl;
    return false;
#endif
}

bool Texture::CreateFromData(const unsigned char* data, int width, int height, bool generateMipmap) {
    if (textureID_ != 0) {
        Destroy();
    }
    
    glGenTextures(1, &textureID_);
    Bind();
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    SetupParameters(generateMipmap);
    
    width_ = width;
    height_ = height;
    
    Unbind();
    
    std::cout << "[INFO] Texture created: " << width << "x" << height << " (ID: " << textureID_ << ")" << std::endl;
    
    return true;
}

bool Texture::CreateCheckerboard(int size, int squareSize, 
                                  const float* color1, const float* color2) {
    if (textureID_ != 0) {
        Destroy();
    }
    
    std::vector<unsigned char> data(size * size * 4);
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int idx = (y * size + x) * 4;
            
            // 判断方格
            bool isColor1 = ((x / squareSize) + (y / squareSize)) % 2 == 0;
            
            const float* color = isColor1 ? color1 : color2;
            
            data[idx + 0] = static_cast<unsigned char>(color[0] * 255.0f);
            data[idx + 1] = static_cast<unsigned char>(color[1] * 255.0f);
            data[idx + 2] = static_cast<unsigned char>(color[2] * 255.0f);
            data[idx + 3] = 255; // Alpha
        }
    }
    
    return CreateFromData(data.data(), size, size, false);
}

void Texture::Bind(unsigned int unit) const {
    if (textureID_ != 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID_);
    }
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetupParameters(bool generateMipmap) {
    if (generateMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

} // namespace Engine
} // namespace ArenaBreakout
