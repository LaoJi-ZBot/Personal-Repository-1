#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <memory>
#include <chrono>

namespace ArenaBreakout {
namespace Engine {
    class WindowManager;
    class Camera;
}
namespace Game {

/**
 * @brief 引擎顶层管理器
 * 负责生命周期管理和游戏循环
 */
class Engine {
public:
    static Engine& Instance();
    
    // 禁止拷贝和移动
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
    
    /**
     * @brief 初始化引擎
     * @param windowTitle 窗口标题
     * @param width 窗口宽度
     * @param height 窗口高度
     * @return true 如果初始化成功
     */
    bool Initialize(const std::string& windowTitle, int width, int height);
    
    /**
     * @brief 运行引擎主循环
     */
    void Run();
    
    /**
     * @brief 停止引擎
     */
    void Stop();
    
    /**
     * @brief 销毁引擎并清理资源
     */
    void Shutdown();
    
    /**
     * @brief 检查引擎是否运行中
     */
    bool IsRunning() const { return running_; }
    
    /**
     * @brief 获取 DeltaTime (秒)
     */
    float GetDeltaTime() const { return deltaTime_; }
    
    /**
     * @brief 获取总运行时间 (秒)
     */
    float GetTotalTime() const { return totalTime_; }
    
private:
    Engine();
    ~Engine();
    
    void ProcessInput();
    void Update();
    void Render();
    void CalculateDeltaTime();
    
    bool running_;
    bool initialized_;
    
    float deltaTime_;
    float totalTime_;
    std::chrono::high_resolution_clock::time_point lastFrameTime_;
};

} // namespace Game
} // namespace ArenaBreakout

#endif // ENGINE_H
