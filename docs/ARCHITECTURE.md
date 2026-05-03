# ArenaBreakout 架构文档

## 项目概述

ArenaBreakout 是《暗区突围》的 PC 复刻版，使用 C++17、OpenGL 3.3 Core 和 OpenAL Soft 开发。

## 目录结构

```
ArenaBreakout/
├── CMakeLists.txt          # CMake 构建配置
├── build.sh                # 一键构建脚本
├── clean.sh                # 清理脚本
├── src/                    # 源代码
│   ├── main.cpp            # 程序入口
│   ├── engine/             # 引擎核心层
│   │   ├── Window/         # 窗口管理 (GLFW)
│   │   ├── Renderer/       # OpenGL 渲染器
│   │   ├── Audio/          # OpenAL 音频系统
│   │   ├── Input/          # 输入处理
│   │   ├── UI/             # UI 框架
│   │   ├── Physics/        # 物理与碰撞
│   │   └── Math/           # 数学工具
│   └── game/               # 游戏逻辑层
│       ├── Core/           # 游戏状态、游戏循环
│       ├── Player/         # 玩家角色
│       ├── Weapons/        # 武器系统
│       └── ...             # 其他游戏模块
├── include/                # 公共头文件
├── third_party/            # 第三方库
└── shaders/                # GLSL 着色器
```

## 核心组件

### Engine::WindowManager
- 单例模式管理 GLFW 窗口
- 支持 1920×1080 分辨率
- F11 全屏切换
- Esc 退出确认

### Engine::Renderer
- Shader: 着色器程序管理
- Camera: FPS 相机系统
- Mesh: VAO/VBO/EBO 封装
- Texture: 2D 纹理管理

### Game::Engine
- 顶层引擎管理器
- 游戏循环控制
- DeltaTime 计算

## 构建说明

### 依赖要求
- GCC ≥ 11 或 Clang ≥ 14
- CMake ≥ 3.16
- OpenGL 3.3 兼容 GPU

### 构建步骤

```bash
# 一键构建
./build.sh

# 或手动构建
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 运行

```bash
./build/ArenaBreakout
```

## 版本信息

当前版本：v0.1.0

## 下一步

第 2 步将实现完整的图形引擎核心，包括：
- 完整的着色器系统
- 3D 网格渲染
- 程序化纹理生成
- 测试场景渲染
