#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <glm/glm.hpp>
#include <random>

namespace ArenaBreakout {
namespace Math {

/**
 * @brief 数学工具函数
 */

// 随机数生成器
class Random {
public:
    static void Seed(unsigned int seed);
    static float Float(float min = 0.0f, float max = 1.0f);
    static int Int(int min, int max);
    static glm::vec3 Vec3(const glm::vec3& min, const glm::vec3& max);
    
private:
    static std::mt19937 rng_;
};

// 角度转换
inline float ToRadians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

inline float ToDegrees(float radians) {
    return radians * 180.0f / 3.14159265358979323846f;
}

// 插值函数
inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
    return glm::mix(a, b, t);
}

// 钳制函数
inline float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline int ClampInt(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// 距离计算
inline float Distance(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(b - a);
}

inline float DistanceSquared(const glm::vec3& a, const glm::vec3& b) {
    return glm::length2(b - a);
}

// 边界检查
inline bool PointInBox(const glm::vec3& point, const glm::vec3& min, const glm::vec3& max) {
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

} // namespace Math
} // namespace ArenaBreakout

#endif // MATH_UTILS_H
