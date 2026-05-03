/**
 * ArenaBreakout - 《暗区突围》PC 复刻版
 * 
 * 主入口文件
 */

#include <game/Core/Engine.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "  Arena Breakout - PC 复刻版" << std::endl;
    std::cout << "  Version: " << ARENA_BREAKOUT_VERSION << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // 获取引擎实例并初始化
    auto& engine = ArenaBreakout::Game::Engine::Instance();
    
    if (!engine.Initialize("暗区突围 | Arena Breakout", 1920, 1080)) {
        std::cerr << "[ERROR] Failed to initialize engine" << std::endl;
        return -1;
    }
    
    // 运行游戏循环
    engine.Run();
    
    // 清理资源
    engine.Shutdown();
    
    std::cout << "Application exited normally" << std::endl;
    
    return 0;
}
