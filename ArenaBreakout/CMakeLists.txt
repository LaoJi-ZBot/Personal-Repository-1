# ArenaBreakout - CMake 构建配置
cmake_minimum_required(VERSION 3.16)
project(ArenaBreakout VERSION 0.1.0 LANGUAGES CXX C)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 默认构建类型为 Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 项目版本宏
add_definitions(-DARENA_BREAKOUT_VERSION_MAJOR=0)
add_definitions(-DARENA_BREAKOUT_VERSION_MINOR=1)
add_definitions(-DARENA_BREAKOUT_VERSION_PATCH=0)
add_definitions(-DARENA_BREAKOUT_VERSION="0.1.0")

# 第三方库路径
set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party")

# GLFW (静态库)
set(GLFW_DIR "${THIRD_PARTY_DIR}/glfw-3.4")
if(EXISTS "${GLFW_DIR}")
    add_subdirectory("${GLFW_DIR}" "${CMAKE_BINARY_DIR}/glfw" EXCLUDE_FROM_ALL)
    include_directories("${GLFW_DIR}/include")
endif()

# GLM (header-only)
set(GLM_DIR "${THIRD_PARTY_DIR}/glm")
if(EXISTS "${GLM_DIR}")
    include_directories("${GLM_DIR}")
endif()

# Glad (OpenGL 加载器)
set(GLAD_DIR "${THIRD_PARTY_DIR}/glad")
if(EXISTS "${GLAD_DIR}")
    include_directories("${GLAD_DIR}/include")
    set(GLAD_SOURCES "${GLAD_DIR}/src/glad.c")
endif()

# OpenAL Soft (静态库)
set(OPENAL_DIR "${THIRD_PARTY_DIR}/openal-soft-1.24.0")
if(EXISTS "${OPENAL_DIR}")
    set(ALSOFT_SAMPLES 0 CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES 0 CACHE BOOL "" FORCE)
    set(LIBTYPE STATIC CACHE STRING "" FORCE)
    add_subdirectory("${OPENAL_DIR}" "${CMAKE_BINARY_DIR}/openal" EXCLUDE_FROM_ALL)
    include_directories("${OPENAL_DIR}/include")
endif()

# FreeType (静态库)
set(FREETYPE_DIR "${THIRD_PARTY_DIR}/freetype-2.13.3")
if(EXISTS "${FREETYPE_DIR}")
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
    add_subdirectory("${FREETYPE_DIR}" "${CMAKE_BINARY_DIR}/freetype" EXCLUDE_FROM_ALL)
    include_directories("${FREETYPE_DIR}/include")
endif()

# Bullet Physics (可选，静态库)
set(BULLET_DIR "${THIRD_PARTY_DIR}/bullet3-3.25")
if(EXISTS "${BULLET_DIR}")
    set(USE_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
    set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory("${BULLET_DIR}" "${CMAKE_BINARY_DIR}/bullet" EXCLUDE_FROM_ALL)
    include_directories("${BULLET_DIR}/src")
endif()

# STB (header-only)
set(STB_DIR "${THIRD_PARTY_DIR}/stb")
if(EXISTS "${STB_DIR}")
    include_directories("${STB_DIR}")
endif()

# 源文件
set(PROJECT_SOURCES
    src/main.cpp
    # Engine
    src/engine/Window/WindowManager.cpp
    src/engine/Renderer/Renderer.cpp
    src/engine/Renderer/Shader.cpp
    src/engine/Renderer/Camera.cpp
    src/engine/Renderer/Mesh.cpp
    src/engine/Renderer/Texture.cpp
    src/engine/Math/MathUtils.cpp
    src/engine/Audio/AudioEngine.cpp
    src/engine/Audio/SoundBuffer.cpp
    # Game Core
    src/game/Core/Engine.cpp
    src/game/Core/GameLoop.cpp
    src/game/AudioGen/SoundGenerator.cpp
)

# 添加 Glad 源文件
if(EXISTS "${GLAD_SOURCES}")
    list(APPEND PROJECT_SOURCES "${GLAD_SOURCES}")
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_FILE_IF_EXISTS:glfw>
    $<TARGET_FILE_IF_EXISTS:OpenAL>
    $<TARGET_FILE_IF_EXISTS:freetype>
    $<TARGET_FILE_IF_EXISTS:BulletCollision>
    $<TARGET_FILE_IF_EXISTS:BulletDynamics>
    $<TARGET_FILE_IF_EXISTS:LinearMath>
)

# 链接系统库
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    pthread
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# 输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

# 打印配置信息
message(STATUS "===========================================")
message(STATUS "ArenaBreakout Build Configuration")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "===========================================")
