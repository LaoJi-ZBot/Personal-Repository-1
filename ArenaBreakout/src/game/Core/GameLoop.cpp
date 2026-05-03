#include "GameLoop.h"

#include <thread>
#include <iostream>

namespace ArenaBreakout {
namespace Game {

GameLoop::GameLoop()
    : targetFPS_(60)
    , fixedDeltaTime_(1.0f / 60.0f)
    , currentFPS_(0.0f)
    , accumulator_(0.0f)
    , frameCount_(0) {
    frameStartTime_ = std::chrono::high_resolution_clock::now();
    lastFPSCalcTime_ = frameStartTime_;
}

void GameLoop::SetTargetFPS(int fps) {
    if (fps > 0) {
        targetFPS_ = fps;
        fixedDeltaTime_ = 1.0f / static_cast<float>(fps);
    }
}

bool GameLoop::BeginFrame() {
    frameStartTime_ = std::chrono::high_resolution_clock::now();
    return true;
}

void GameLoop::EndFrame() {
    // 计算帧时间
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> frameDuration = frameEndTime - frameStartTime_;
    float frameTime = frameDuration.count();
    
    // 累积时间用于固定步长更新
    accumulator_ += frameTime;
    
    // 限制最大累积时间防止螺旋死亡
    const float maxAccumulator = 0.25f;  // 最多累积 250ms
    if (accumulator_ > maxAccumulator) {
        accumulator_ = maxAccumulator;
    }
    
    // 计算 FPS
    frameCount_++;
    
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fpsDuration = now - lastFPSCalcTime_;
    
    if (fpsDuration.count() >= 1.0f) {
        currentFPS_ = static_cast<float>(frameCount_) / fpsDuration.count();
        frameCount_ = 0;
        lastFPSCalcTime_ = now;
        
        // 可选：打印 FPS 到窗口标题 (将在 WindowManager 中实现)
        // std::cout << "[FPS] " << currentFPS_ << std::endl;
    }
    
    // 帧率控制 (可选，如果需要同步到目标 FPS)
    // float targetFrameTime = 1.0f / static_cast<float>(targetFPS_);
    // if (frameTime < targetFrameTime) {
    //     float sleepTime = targetFrameTime - frameTime;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime * 1000)));
    // }
}

} // namespace Game
} // namespace ArenaBreakout
