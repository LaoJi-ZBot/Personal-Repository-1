#include "WindowManager.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

namespace ArenaBreakout {
namespace Engine {

WindowManager& WindowManager::Instance() {
    static WindowManager instance;
    return instance;
}

WindowManager::~WindowManager() {
    Shutdown();
}

bool WindowManager::Initialize() {
    if (initialized_) {
        return true;
    }
    
    // 设置 GLFW 错误回调
    glfwSetErrorCallback(GLFWErrorCallback);
    
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // 配置 OpenGL 版本为 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // 禁用 vsync (由用户控制)
    glfwSwapInterval(0);
    
    initialized_ = true;
    std::cout << "[INFO] GLFW initialized successfully" << std::endl;
    
    return true;
}

bool WindowManager::CreateWindow(int width, int height, const std::string& title) {
    if (!initialized_) {
        if (!Initialize()) {
            return false;
        }
    }
    
    width_ = width;
    height_ = height;
    
    // 创建窗口
    window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
    if (!window_) {
        std::cerr << "[ERROR] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // 设置当前上下文
    glfwMakeContextCurrent(window_);
    
    // 设置窗口居中
    CenterWindow();
    
    // 注册回调
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, GLFWKeyboardCallback);
    glfwSetMouseButtonCallback(window_, GLFWMouseButtonCallback);
    glfwSetCursorPosCallback(window_, GLFWMouseMoveCallback);
    glfwSetScrollCallback(window_, GLFWScrollCallback);
    
    std::cout << "[INFO] Window created: " << width_ << "x" << height_ << " - \"" << title << "\"" << std::endl;
    
    return true;
}

void WindowManager::Shutdown() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    
    if (initialized_) {
        glfwTerminate();
        initialized_ = false;
    }
    
    std::cout << "[INFO] WindowManager shutdown complete" << std::endl;
}

bool WindowManager::ShouldClose() const {
    return window_ && glfwWindowShouldClose(window_);
}

void WindowManager::PollEvents() {
    glfwPollEvents();
}

void WindowManager::SwapBuffers() {
    if (window_) {
        glfwSwapBuffers(window_);
    }
}

void WindowManager::SetKeyboardCallback(KeyboardCallback callback) {
    keyboard_callback_ = std::move(callback);
}

void WindowManager::SetMouseButtonCallback(MouseButtonCallback callback) {
    mouse_button_callback_ = std::move(callback);
}

void WindowManager::SetMouseMoveCallback(MouseMoveCallback callback) {
    mouse_move_callback_ = std::move(callback);
}

void WindowManager::SetScrollCallback(ScrollCallback callback) {
    scroll_callback_ = std::move(callback);
}

void WindowManager::ToggleFullscreen(bool fullscreen) {
    if (!window_) return;
    
    if (fullscreen) {
        // 获取主显示器
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        // 保存当前窗口位置和大小
        int oldWidth, oldHeight;
        glfwGetWindowSize(window_, &oldWidth, &oldHeight);
        
        // 切换到全屏
        glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        width_ = mode->width;
        height_ = mode->height;
    } else {
        // 切换回窗口模式
        GLFWmonitor* monitor = glfwGetWindowMonitor(window_);
        if (monitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            
            // 使用默认尺寸或保存的尺寸
            int newWidth = 1920;
            int newHeight = 1080;
            
            glfwSetWindowMonitor(window_, nullptr, 
                                 (mode->width - newWidth) / 2, 
                                 (mode->height - newHeight) / 2,
                                 newWidth, newHeight, 0);
            width_ = newWidth;
            height_ = newHeight;
            
            CenterWindow();
        }
    }
}

void WindowManager::CenterWindow() {
    if (!window_) return;
    
    GLFWmonitor* monitor = glfwGetWindowMonitor(window_);
    if (!monitor) {
        monitor = glfwGetPrimaryMonitor();
    }
    
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window_, &windowWidth, &windowHeight);
    
    int posX = (mode->width - windowWidth) / 2;
    int posY = (mode->height - windowHeight) / 2;
    
    glfwSetWindowPos(window_, posX, posY);
}

void WindowManager::GLFWErrorCallback(int error, const char* description) {
    std::cerr << "[GLFW Error " << error << "]: " << description << std::endl;
}

void WindowManager::GLFWKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->keyboard_callback_) {
        self->keyboard_callback_(key, scancode, action, mods);
    }
    
    // F11 切换全屏
    if (action == GLFW_PRESS && key == GLFW_KEY_F11) {
        self->ToggleFullscreen(true);
    }
    
    // Alt+F4 或 Esc 退出
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void WindowManager::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->mouse_button_callback_) {
        self->mouse_button_callback_(button, action, mods);
    }
}

void WindowManager::GLFWMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->mouse_move_callback_) {
        self->mouse_move_callback_(xpos, ypos);
    }
}

void WindowManager::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->scroll_callback_) {
        self->scroll_callback_(xoffset, yoffset);
    }
}

} // namespace Engine
} // namespace ArenaBreakout
