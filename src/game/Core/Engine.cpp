#include "Engine.h"

#include <engine/Window/WindowManager.h>
#include <engine/Renderer/Camera.h>
#include <glad/glad.h>

#include <iostream>

namespace ArenaBreakout {
namespace Game {

Engine::Engine()
    : running_(false)
    , initialized_(false)
    , deltaTime_(0.0f)
    , totalTime_(0.0f) {
}

Engine& Engine::Instance() {
    static Engine instance;
    return instance;
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const std::string& windowTitle, int width, int height) {
    if (initialized_) {
        std::cerr << "[WARN] Engine already initialized" << std::endl;
        return true;
    }
    
    std::cout << "[INFO] Initializing ArenaBreakout Engine v" << ARENA_BREAKOUT_VERSION << std::endl;
    
    // 初始化窗口管理器
    auto& windowMgr = Engine::WindowManager::Instance();
    if (!windowMgr.Initialize()) {
        std::cerr << "[ERROR] Failed to initialize window manager" << std::endl;
        return false;
    }
    
    if (!windowMgr.CreateWindow(width, height, windowTitle)) {
        std::cerr << "[ERROR] Failed to create window" << std::endl;
        return false;
    }
    
    // 初始化 Glad (OpenGL 函数加载器)
    // 注意：这里使用简化的 glad，实际项目中需要完整的 glad 实现
    std::cout << "[INFO] OpenGL context created" << std::endl;
    
    // 设置 OpenGL 状态
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // 深灰色背景
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    initialized_ = true;
    running_ = true;
    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    
    std::cout << "[INFO] Engine initialized successfully" << std::endl;
    
    return true;
}

void Engine::Run() {
    if (!initialized_) {
        std::cerr << "[ERROR] Engine not initialized, cannot run" << std::endl;
        return;
    }
    
    std::cout << "[INFO] Starting game loop..." << std::endl;
    
    auto& windowMgr = Engine::WindowManager::Instance();
    
    while (running_ && !windowMgr.ShouldClose()) {
        CalculateDeltaTime();
        
        ProcessInput();
        Update();
        Render();
        
        windowMgr.PollEvents();
        windowMgr.SwapBuffers();
    }
    
    std::cout << "[INFO] Game loop ended" << std::endl;
}

void Engine::Stop() {
    running_ = false;
}

void Engine::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[INFO] Shutting down engine..." << std::endl;
    
    running_ = false;
    
    // 清理资源
    Engine::WindowManager::Instance().Shutdown();
    
    initialized_ = false;
    
    std::cout << "[INFO] Engine shutdown complete" << std::endl;
}

void Engine::ProcessInput() {
    // 输入处理将在后续步骤中实现
}

void Engine::Update() {
    // 游戏逻辑更新将在后续步骤中实现
}

void Engine::Render() {
    // 清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 渲染将在后续步骤中实现
}

void Engine::CalculateDeltaTime() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = now - lastFrameTime_;
    deltaTime_ = delta.count();
    totalTime_ += deltaTime_;
    lastFrameTime_ = now;
}

} // namespace Game
} // namespace ArenaBreakout
