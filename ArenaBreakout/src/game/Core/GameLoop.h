#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <chrono>
#include <functional>

namespace ArenaBreakout {
namespace Game {

/**
 * @brief 游戏循环管理器
 * 实现固定时间步长的游戏循环 (目标 60 FPS)
 */
class GameLoop {
public:
    GameLoop();
    
    /**
     * @brief 设置目标 FPS
     */
    void SetTargetFPS(int fps);
    
    /**
     * @brief 获取目标 FPS
     */
    int GetTargetFPS() const { return targetFPS_; }
    
    /**
     * @brief 获取固定时间步长 (秒)
     */
    float GetFixedDeltaTime() const { return fixedDeltaTime_; }
    
    /**
     * @brief 开始新帧
     * @return true 如果应该继续运行
     */
    bool BeginFrame();
    
    /**
     * @brief 结束帧并控制帧率
     */
    void EndFrame();
    
    /**
     * @brief 获取实际 FPS
     */
    float GetCurrentFPS() const { return currentFPS_; }
    
    /**
     * @brief 获取累积时间 (用于物理更新等)
     */
    float GetAccumulator() const { return accumulator_; }
    
private:
    int targetFPS_;
    float fixedDeltaTime_;
    float currentFPS_;
    float accumulator_;
    
    std::chrono::high_resolution_clock::time_point frameStartTime_;
    std::chrono::high_resolution_clock::time_point lastFPSCalcTime_;
    int frameCount_;
};

} // namespace Game
} // namespace ArenaBreakout

#endif // GAME_LOOP_H
