#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <memory>
#include <vector>

namespace ArenaBreakout {
namespace Engine {

/**
 * @brief 纹理类 - 封装 OpenGL 2D 纹理
 * 使用 RAII 管理 GL 资源
 */
class Texture {
public:
    Texture();
    ~Texture();
    
    // 禁止拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // 允许移动
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    
    /**
     * @brief 从文件加载纹理
     * @param path 文件路径
     * @param generateMipmap 是否生成 mipmap
     * @return true 如果加载成功
     */
    bool LoadFromFile(const std::string& path, bool generateMipmap = true);
    
    /**
     * @brief 从内存数据创建纹理
     * @param data 像素数据 (RGBA)
     * @param width 宽度
     * @param height 高度
     * @param generateMipmap 是否生成 mipmap
     * @return true 如果创建成功
     */
    bool CreateFromData(const unsigned char* data, int width, int height, bool generateMipmap = true);
    
    /**
     * @brief 创建程序化棋盘格纹理
     * @param size 纹理尺寸 (size x size)
     * @param squareSize 方格大小
     * @param color1 颜色 1
     * @param color2 颜色 2
     * @return true 如果创建成功
     */
    bool CreateCheckerboard(int size, int squareSize, 
                           const float* color1, const float* color2);
    
    /**
     * @brief 绑定纹理到指定单元
     * @param unit 纹理单元 (GL_TEXTURE0 + unit)
     */
    void Bind(unsigned int unit = 0) const;
    
    /**
     * @brief 解绑纹理
     */
    static void Unbind();
    
    /**
     * @brief 销毁纹理资源
     */
    void Destroy();
    
    /**
     * @brief 获取纹理 ID
     */
    unsigned int GetTextureID() const { return textureID_; }
    
    /**
     * @brief 获取宽度
     */
    int GetWidth() const { return width_; }
    
    /**
     * @brief 获取高度
     */
    int GetHeight() const { return height_; }
    
    /**
     * @brief 检查是否有效
     */
    bool IsValid() const { return textureID_ != 0; }
    
private:
    unsigned int textureID_;
    int width_;
    int height_;
    
    void SetupParameters(bool generateMipmap);
};

// 使用 shared_ptr 管理 Texture
using TexturePtr = std::shared_ptr<Texture>;

} // namespace Engine
} // namespace ArenaBreakout

#endif // TEXTURE_H
