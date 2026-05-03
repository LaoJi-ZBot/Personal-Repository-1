#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <string>
#include <functional>

struct GLFWwindow;

namespace ArenaBreakout {
namespace Engine {

/**
 * @brief 窗口管理器 - 封装 GLFW 窗口管理功能
 * 使用单例模式管理窗口生命周期
 */
class WindowManager {
public:
    static WindowManager& Instance();
    
    // 禁止拷贝和移动
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;
    
    /**
     * @brief 初始化窗口系统
     * @return true 如果初始化成功
     */
    bool Initialize();
    
    /**
     * @brief 创建窗口
     * @param width 窗口宽度
     * @param height 窗口高度
     * @param title 窗口标题
     * @return true 如果创建成功
     */
    bool CreateWindow(int width, int height, const std::string& title);
    
    /**
     * @brief 销毁窗口和清理 GLFW
     */
    void Shutdown();
    
    /**
     * @brief 检查窗口是否应该关闭
     */
    bool ShouldClose() const;
    
    /**
     * @brief 轮询事件
     */
    void PollEvents();
    
    /**
     * @brief 交换缓冲区
     */
    void SwapBuffers();
    
    /**
     * @brief 获取窗口宽度
     */
    int GetWidth() const { return width_; }
    
    /**
     * @brief 获取窗口高度
     */
    int GetHeight() const { return height_; }
    
    /**
     * @brief 获取 GLFW 窗口指针
     */
    GLFWwindow* GetGLFWWindow() const { return window_; }
    
    /**
     * @brief 设置键盘回调
     */
    using KeyboardCallback = std::function<void(int key, int scancode, int action, int mods)>;
    void SetKeyboardCallback(KeyboardCallback callback);
    
    /**
     * @brief 设置鼠标按钮回调
     */
    using MouseButtonCallback = std::function<void(int button, int action, int mods)>;
    void SetMouseButtonCallback(MouseButtonCallback callback);
    
    /**
     * @brief 设置鼠标移动回调
     */
    using MouseMoveCallback = std::function<void(double xpos, double ypos)>;
    void SetMouseMoveCallback(MouseMoveCallback callback);
    
    /**
     * @brief 设置鼠标滚动回调
     */
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;
    void SetScrollCallback(ScrollCallback callback);
    
    /**
     * @brief 切换全屏模式
     * @param fullscreen 是否全屏
     */
    void ToggleFullscreen(bool fullscreen);
    
    /**
     * @brief 窗口是否居中
     */
    void CenterWindow();
    
private:
    WindowManager() : window_(nullptr), width_(1920), height_(1080), initialized_(false) {}
    ~WindowManager();
    
    GLFWwindow* window_;
    int width_;
    int height_;
    bool initialized_;
    
    KeyboardCallback keyboard_callback_;
    MouseButtonCallback mouse_button_callback_;
    MouseMoveCallback mouse_move_callback_;
    ScrollCallback scroll_callback_;
    
    // 静态回调函数
    static void GLFWErrorCallback(int error, const char* description);
    static void GLFWKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFWMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
    static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

} // namespace Engine
} // namespace ArenaBreakout

#endif // WINDOW_MANAGER_H
