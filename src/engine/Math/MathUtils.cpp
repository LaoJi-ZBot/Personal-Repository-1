#include "MathUtils.h"

namespace ArenaBreakout {
namespace Math {

std::mt19937 Random::rng_;

void Random::Seed(unsigned int seed) {
    rng_.seed(seed);
}

float Random::Float(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng_);
}

int Random::Int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

glm::vec3 Random::Vec3(const glm::vec3& min, const glm::vec3& max) {
    return glm::vec3(
        Float(min.x, max.x),
        Float(min.y, max.y),
        Float(min.z, max.z)
    );
}

} // namespace Math
} // namespace ArenaBreakout
