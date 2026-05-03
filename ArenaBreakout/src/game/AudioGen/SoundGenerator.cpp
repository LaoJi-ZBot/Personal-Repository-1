#include "SoundGenerator.h"
#include <random>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace arena {
namespace audio {

// ==================== 随机数生成器 ====================

std::mt19937& SoundGenerator::getRandomEngine() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

// ==================== 工具函数 ====================

std::vector<float> SoundGenerator::generateWhiteNoise(size_t samples, float amplitude) {
    std::vector<float> noise(samples);
    std::uniform_real_distribution<float> dist(-amplitude, amplitude);
    auto& rng = getRandomEngine();
    for (size_t i = 0; i < samples; ++i) {
        noise[i] = dist(rng);
    }
    return noise;
}

std::vector<float> SoundGenerator::generatePinkNoise(size_t samples, float amplitude) {
    // 使用 Voss-McCartney 算法生成粉红噪声
    std::vector<float> noise(samples, 0.0f);
    auto& rng = getRandomEngine();
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    const int numRows = 16;
    std::vector<float> rowValues(numRows, 0.0f);
    std::vector<int> rowSteps(numRows, 1);
    
    for (size_t i = 0; i < numRows; ++i) {
        rowSteps[i] = 1 << i;
    }
    
    for (size_t i = 0; i < samples; ++i) {
        float sum = 0.0f;
        for (int row = 0; row < numRows; ++row) {
            if (i % rowSteps[row] == 0) {
                rowValues[row] = dist(rng);
            }
            sum += rowValues[row];
        }
        noise[i] = (sum / numRows) * amplitude;
    }
    
    return noise;
}

std::vector<float> SoundGenerator::generateSineWave(float freq, float duration, float amplitude, float phase, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    std::vector<float> wave(samples);
    float omega = 2.0f * 3.14159265359f * freq / sampleRate;
    
    for (size_t i = 0; i < samples; ++i) {
        wave[i] = amplitude * std::sin(omega * static_cast<float>(i) + phase);
    }
    
    return wave;
}

std::vector<float> SoundGenerator::generateSawWave(float freq, float duration, float amplitude, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    std::vector<float> wave(samples);
    float period = static_cast<float>(sampleRate) / freq;
    
    for (size_t i = 0; i < samples; ++i) {
        float t = std::fmod(static_cast<float>(i), period) / period;
        wave[i] = amplitude * (2.0f * t - 1.0f);
    }
    
    return wave;
}

std::vector<float> SoundGenerator::generateSquareWave(float freq, float duration, float amplitude, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    std::vector<float> wave(samples);
    float period = static_cast<float>(sampleRate) / freq;
    
    for (size_t i = 0; i < samples; ++i) {
        float t = std::fmod(static_cast<float>(i), period) / period;
        wave[i] = (t < 0.5f) ? amplitude : -amplitude;
    }
    
    return wave;
}

void SoundGenerator::applyEnvelope(std::vector<float>& samples, float attack, float decay, float sustain, float release) {
    if (samples.empty()) return;
    
    size_t totalSamples = samples.size();
    size_t attackSamples = static_cast<size_t>(attack * DEFAULT_SAMPLE_RATE);
    size_t decaySamples = static_cast<size_t>(decay * DEFAULT_SAMPLE_RATE);
    size_t releaseSamples = static_cast<size_t>(release * DEFAULT_SAMPLE_RATE);
    size_t sustainSamples = totalSamples - attackSamples - decaySamples - releaseSamples;
    
    attackSamples = std::min(attackSamples, totalSamples);
    decaySamples = std::min(decaySamples, totalSamples - attackSamples);
    releaseSamples = std::min(releaseSamples, totalSamples);
    sustainSamples = totalSamples - attackSamples - decaySamples - releaseSamples;
    
    size_t idx = 0;
    
    // Attack
    for (size_t i = 0; i < attackSamples && idx < totalSamples; ++i, ++idx) {
        float t = static_cast<float>(i) / attackSamples;
        samples[idx] *= t;
    }
    
    // Decay
    for (size_t i = 0; i < decaySamples && idx < totalSamples; ++i, ++idx) {
        float t = 1.0f - (static_cast<float>(i) / decaySamples) * (1.0f - sustain);
        samples[idx] *= t;
    }
    
    // Sustain
    for (size_t i = 0; i < sustainSamples && idx < totalSamples; ++i, ++idx) {
        samples[idx] *= sustain;
    }
    
    // Release
    for (size_t i = 0; i < releaseSamples && idx < totalSamples; ++i, ++idx) {
        float t = 1.0f - static_cast<float>(i) / releaseSamples;
        samples[idx] *= t;
    }
}

void SoundGenerator::applyExponentialDecay(std::vector<float>& samples, float decayFactor) {
    float decay = 1.0f;
    for (auto& sample : samples) {
        sample *= decay;
        decay *= decayFactor;
    }
}

std::vector<float> SoundGenerator::lowpassFilter(const std::vector<float>& input, float cutoff, int sampleRate) {
    if (input.empty()) return input;
    
    // 简单的一阶低通滤波器
    float RC = 1.0f / (2.0f * 3.14159265359f * cutoff);
    float dt = 1.0f / static_cast<float>(sampleRate);
    float alpha = dt / (RC + dt);
    
    std::vector<float> output(input.size());
    output[0] = input[0];
    
    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = output[i-1] + alpha * (input[i] - output[i-1]);
    }
    
    return output;
}

std::vector<float> SoundGenerator::highpassFilter(const std::vector<float>& input, float cutoff, int sampleRate) {
    if (input.empty()) return input;
    
    // 简单的一阶高通滤波器
    float RC = 1.0f / (2.0f * 3.14159265359f * cutoff);
    float dt = 1.0f / static_cast<float>(sampleRate);
    float alpha = RC / (RC + dt);
    
    std::vector<float> output(input.size());
    float prevIn = 0.0f;
    float prevOut = 0.0f;
    
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = alpha * (prevOut + input[i] - prevIn);
        prevIn = input[i];
        prevOut = output[i];
    }
    
    return output;
}

std::vector<float> SoundGenerator::addReverb(const std::vector<float>& input, float mix, float decay) {
    if (input.empty()) return input;
    
    // 简易多抽头延迟混响
    std::vector<float> output = input;
    size_t delay1 = DEFAULT_SAMPLE_RATE * 0.03f;  // 30ms
    size_t delay2 = DEFAULT_SAMPLE_RATE * 0.05f;  // 50ms
    size_t delay3 = DEFAULT_SAMPLE_RATE * 0.08f;  // 80ms
    
    // 扩展输出缓冲区以容纳混响尾音
    size_t reverbTail = DEFAULT_SAMPLE_RATE / 2;  // 500ms 混响
    output.resize(input.size() + reverbTail, 0.0f);
    
    // 添加多个延迟抽头
    for (size_t i = 0; i < input.size(); ++i) {
        if (i + delay1 < output.size()) output[i + delay1] += input[i] * decay * 0.5f * mix;
        if (i + delay2 < output.size()) output[i + delay2] += input[i] * decay * 0.3f * mix;
        if (i + delay3 < output.size()) output[i + delay3] += input[i] * decay * 0.2f * mix;
    }
    
    // 混响衰减
    for (size_t i = input.size(); i < output.size(); ++i) {
        output[i] *= decay;
    }
    
    return output;
}

void SoundGenerator::applyFadeIn(std::vector<float>& samples, size_t fadeSamples) {
    fadeSamples = std::min(fadeSamples, samples.size());
    for (size_t i = 0; i < fadeSamples; ++i) {
        float t = static_cast<float>(i) / fadeSamples;
        samples[i] *= t;
    }
}

void SoundGenerator::applyFadeOut(std::vector<float>& samples, size_t fadeSamples) {
    fadeSamples = std::min(fadeSamples, samples.size());
    for (size_t i = 0; i < fadeSamples; ++i) {
        float t = 1.0f - static_cast<float>(i) / fadeSamples;
        samples[samples.size() - 1 - i] *= t;
    }
}

std::vector<float> SoundGenerator::mixAudio(const std::vector<float>& a, const std::vector<float>& b, float mixA, float mixB) {
    size_t len = std::max(a.size(), b.size());
    std::vector<float> result(len, 0.0f);
    
    for (size_t i = 0; i < len; ++i) {
        if (i < a.size()) result[i] += a[i] * mixA;
        if (i < b.size()) result[i] += b[i] * mixB;
    }
    
    // 防止削波
    float maxVal = 0.0f;
    for (float s : result) maxVal = std::max(maxVal, std::abs(s));
    if (maxVal > 1.0f) {
        float scale = 0.9f / maxVal;
        for (auto& s : result) s *= scale;
    }
    
    return result;
}

std::vector<float> SoundGenerator::concatenateAudio(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result(a.size() + b.size());
    std::copy(a.begin(), a.end(), result.begin());
    std::copy(b.begin(), b.end(), result.begin() + a.size());
    return result;
}

// ==================== 枪声生成 ====================

SoundGenerator::GunSoundParams SoundGenerator::getGunParams(const std::string& gunType) {
    GunSoundParams params;
    
    if (gunType == "AK" || gunType == "AKM" || gunType == "AK-74") {
        // AK 系列：低沉粗犷
        params.baseFreq = 80.0f;
        params.decayRate = 0.97f;
        params.noiseAmount = 0.8f;
        params.duration = 0.35f;
        params.lowFreqRatio = 0.4f;
        params.mechanicalClick = 0.15f;
    } else if (gunType == "M4" || gunType == "M4A1" || gunType == "AR15") {
        // M4 系列：清脆干净
        params.baseFreq = 120.0f;
        params.decayRate = 0.98f;
        params.noiseAmount = 0.6f;
        params.duration = 0.25f;
        params.lowFreqRatio = 0.2f;
        params.highFreqRatio = 0.3f;
        params.mechanicalClick = 0.1f;
    } else if (gunType == "Sniper" || gunType == "AWM" || gunType == "AX50" || gunType == "Mosin") {
        // 狙击步枪：响亮震撼
        params.baseFreq = 60.0f;
        params.decayRate = 0.96f;
        params.noiseAmount = 0.7f;
        params.duration = 0.5f;
        params.lowFreqRatio = 0.5f;
        params.mechanicalClick = 0.2f;
    } else if (gunType == "Shotgun" || gunType == "M870" || gunType == "MP133") {
        // 霰弹枪：厚重沉闷
        params.baseFreq = 50.0f;
        params.decayRate = 0.95f;
        params.noiseAmount = 0.9f;
        params.duration = 0.4f;
        params.lowFreqRatio = 0.6f;
        params.mechanicalClick = 0.2f;
    } else if (gunType == "Pistol" || gunType == "G17" || gunType == "M1911") {
        // 手枪：短促尖锐
        params.baseFreq = 150.0f;
        params.decayRate = 0.98f;
        params.noiseAmount = 0.5f;
        params.duration = 0.2f;
        params.lowFreqRatio = 0.2f;
        params.highFreqRatio = 0.3f;
    } else if (gunType == "SMG" || gunType == "MP5" || gunType == "Vector") {
        // 冲锋枪：高频快速
        params.baseFreq = 130.0f;
        params.decayRate = 0.99f;
        params.noiseAmount = 0.55f;
        params.duration = 0.18f;
        params.lowFreqRatio = 0.15f;
        params.highFreqRatio = 0.35f;
    } else {
        // 默认
        params.baseFreq = 100.0f;
        params.decayRate = 0.98f;
        params.noiseAmount = 0.7f;
        params.duration = 0.3f;
    }
    
    return params;
}

std::vector<float> SoundGenerator::generateGunshot(const GunSoundParams& params, int sampleRate) {
    size_t totalSamples = static_cast<size_t>(params.duration * sampleRate);
    std::vector<float> gunshot(totalSamples, 0.0f);
    
    auto& rng = getRandomEngine();
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    float baseFreq = params.baseFreq;
    float omega = 2.0f * 3.14159265359f * baseFreq / sampleRate;
    
    // 生成低频震荡 (模拟枪口爆音)
    for (size_t i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float decay = std::pow(params.decayRate, static_cast<float>(i));
        
        // 基频正弦波
        float sine = std::sin(omega * static_cast<float>(i));
        
        // 低频调制
        float lowMod = std::sin(2.0f * 3.14159265359f * 20.0f * t);
        
        // 白噪声
        float noise = dist(rng) * params.noiseAmount;
        
        // 混合
        float sample = (sine * params.lowFreqRatio + noise * (1.0f - params.lowFreqRatio) + lowMod * 0.3f) * decay;
        
        gunshot[i] = sample;
    }
    
    // 应用指数衰减
    applyExponentialDecay(gunshot, params.decayRate);
    
    // 添加机械撞击声
    size_t clickStart = totalSamples / 10;
    size_t clickLen = std::min<size_t>(totalSamples / 20, 500);
    for (size_t i = 0; i < clickLen && clickStart + i < totalSamples; ++i) {
        gunshot[clickStart + i] += dist(rng) * params.mechanicalClick * (1.0f - static_cast<float>(i) / clickLen);
    }
    
    // 淡出避免爆音
    applyFadeOut(gunshot, sampleRate / 50);
    
    return gunshot;
}

std::vector<float> SoundGenerator::generateAKGunshot(int sampleRate) {
    return generateGunshot(getGunParams("AK"), sampleRate);
}

std::vector<float> SoundGenerator::generateM4Gunshot(int sampleRate) {
    return generateGunshot(getGunParams("M4"), sampleRate);
}

std::vector<float> SoundGenerator::generateSniperGunshot(int sampleRate) {
    return generateGunshot(getGunParams("Sniper"), sampleRate);
}

std::vector<float> SoundGenerator::generateShotgunGunshot(int sampleRate) {
    return generateGunshot(getGunParams("Shotgun"), sampleRate);
}

std::vector<float> SoundGenerator::generatePistolGunshot(int sampleRate) {
    return generateGunshot(getGunParams("Pistol"), sampleRate);
}

std::vector<float> SoundGenerator::generateSMGGunshot(int sampleRate) {
    return generateGunshot(getGunParams("SMG"), sampleRate);
}

// ==================== 脚步声生成 ====================

std::vector<float> SoundGenerator::generateFootstep(SurfaceType surface, bool isRun, int sampleRate) {
    float duration = isRun ? 0.15f : 0.2f;
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    // 基础噪声
    std::vector<float> step = generatePinkNoise(samples, 0.5f);
    
    // 根据表面类型滤波
    switch (surface) {
        case SurfaceType::Concrete:
            step = highpassFilter(step, 800.0f, sampleRate);
            break;
        case SurfaceType::Dirt:
            step = lowpassFilter(step, 500.0f, sampleRate);
            break;
        case SurfaceType::Metal:
            step = highpassFilter(step, 400.0f, sampleRate);
            // 添加金属余音
            {
                auto ring = generateSineWave(2000.0f, 0.3f, 0.2f, 0.0f, sampleRate);
                applyExponentialDecay(ring, 0.95f);
                step = mixAudio(step, ring, 1.0f, 0.5f);
            }
            break;
        case SurfaceType::Wood:
            step = bandPassFilter(step, 300.0f, 800.0f, sampleRate);
            break;
        case SurfaceType::Grass:
            step = lowpassFilter(step, 400.0f, sampleRate);
            break;
        case SurfaceType::Gravel:
            step = bandPassFilter(step, 500.0f, 1500.0f, sampleRate);
            break;
    }
    
    // 应用包络
    applyEnvelope(step, 0.01f, 0.05f, 0.3f, 0.1f);
    
    return step;
}

// 带通滤波辅助函数
std::vector<float> SoundGenerator::bandPassFilter(const std::vector<float>& input, float lowCut, float highCut, int sampleRate) {
    auto hp = highpassFilter(input, lowCut, sampleRate);
    return lowpassFilter(hp, highCut, sampleRate);
}

// ==================== 环境音生成 ====================

std::vector<float> SoundGenerator::generateWind(float duration, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    // 生成低频噪声
    auto wind = generatePinkNoise(samples, 0.3f);
    wind = lowpassFilter(wind, 400.0f, sampleRate);
    
    // LFO 调制音量
    auto lfo = generateSineWave(0.5f, duration, 1.0f, 0.0f, sampleRate);
    
    for (size_t i = 0; i < samples; ++i) {
        wind[i] *= (0.5f + 0.5f * lfo[i]);
    }
    
    applyFadeIn(wind, sampleRate / 10);
    applyFadeOut(wind, sampleRate / 10);
    
    return wind;
}

std::vector<float> SoundGenerator::generateBirdChirp(int sampleRate) {
    float duration = 0.3f;
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    // FM 合成鸟鸣
    std::vector<float> chirp(samples, 0.0f);
    
    // 频率从 2000Hz 滑到 3000Hz 再滑回
    for (size_t i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float freq = 2000.0f + 1000.0f * std::sin(3.14159265359f * t);
        float omega = 2.0f * 3.14159265359f * freq / sampleRate;
        
        chirp[i] = 0.3f * std::sin(omega * static_cast<float>(i));
    }
    
    applyEnvelope(chirp, 0.02f, 0.05f, 0.5f, 0.15f);
    
    return chirp;
}

std::vector<float> SoundGenerator::generateDistantGunshot(const std::vector<float>& original, int sampleRate) {
    // 低通滤波
    auto distant = lowpassFilter(original, 800.0f, sampleRate);
    
    // 降低音量
    for (auto& s : distant) s *= 0.3f;
    
    // 添加混响
    return addReverb(distant, 0.5f, 0.6f);
}

std::vector<float> SoundGenerator::generateRain(float duration, float intensity, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    // 多层噪声叠加
    auto rain = generateWhiteNoise(samples, intensity * 0.3f);
    rain = lowpassFilter(rain, 1000.0f, sampleRate);
    
    // 添加高频细节
    auto detail = generateWhiteNoise(samples, intensity * 0.1f);
    detail = highpassFilter(detail, 2000.0f, sampleRate);
    
    rain = mixAudio(rain, detail, 1.0f, 0.5f);
    
    applyFadeIn(rain, sampleRate / 5);
    applyFadeOut(rain, sampleRate / 5);
    
    return rain;
}

// ==================== UI 音效生成 ====================

std::vector<float> SoundGenerator::generateButtonClick(int sampleRate) {
    // 800Hz, 50ms 正弦波
    return generateSineWave(800.0f, 0.05f, 0.5f, 0.0f, sampleRate);
}

std::vector<float> SoundGenerator::generateConfirmSound(int sampleRate) {
    // C5 (523.25Hz) → E5 (659.25Hz)
    float duration = 0.1f;
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    std::vector<float> sound(samples);
    for (size_t i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float freq = 523.25f + (659.25f - 523.25f) * t;
        float omega = 2.0f * 3.14159265359f * freq / sampleRate;
        sound[i] = 0.4f * std::sin(omega * static_cast<float>(i));
    }
    
    applyEnvelope(sound, 0.01f, 0.02f, 0.8f, 0.05f);
    
    return sound;
}

std::vector<float> SoundGenerator::generateCancelSound(int sampleRate) {
    // E5 (659.25Hz) → C5 (523.25Hz)
    float duration = 0.1f;
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    std::vector<float> sound(samples);
    for (size_t i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float freq = 659.25f - (659.25f - 523.25f) * t;
        float omega = 2.0f * 3.14159265359f * freq / sampleRate;
        sound[i] = 0.4f * std::sin(omega * static_cast<float>(i));
    }
    
    applyEnvelope(sound, 0.01f, 0.02f, 0.8f, 0.05f);
    
    return sound;
}

std::vector<float> SoundGenerator::generateWarningSound(int sampleRate) {
    // 200Hz 锯齿波，300ms
    auto sound = generateSawWave(200.0f, 0.3f, 0.4f, sampleRate);
    applyEnvelope(sound, 0.01f, 0.05f, 0.7f, 0.1f);
    return sound;
}

std::vector<float> SoundGenerator::generatePickupSound(int sampleRate) {
    // 轻快的叮当声：两个高频率正弦波
    auto tone1 = generateSineWave(1200.0f, 0.1f, 0.3f, 0.0f, sampleRate);
    auto tone2 = generateSineWave(1800.0f, 0.1f, 0.2f, 0.0f, sampleRate);
    
    auto mixed = mixAudio(tone1, tone2, 1.0f, 1.0f);
    applyEnvelope(mixed, 0.01f, 0.02f, 0.5f, 0.05f);
    
    return mixed;
}

// ==================== 爆炸音效 ====================

std::vector<float> SoundGenerator::generateExplosion(float duration, int sampleRate) {
    size_t samples = static_cast<size_t>(duration * sampleRate);
    
    // 白噪声爆发
    auto blast = generateWhiteNoise(samples, 1.0f);
    blast = lowpassFilter(blast, 500.0f, sampleRate);
    
    // 低频 rumble
    auto rumble = generateSineWave(40.0f, duration, 0.5f, 0.0f, sampleRate);
    rumble = mixAudio(rumble, generatePinkNoise(samples, 0.3f), 1.0f, 0.5f);
    
    // 混合
    auto explosion = mixAudio(blast, rumble, 0.7f, 0.8f);
    
    // 强衰减
    applyExponentialDecay(explosion, 0.99f);
    
    // 添加回响
    return addReverb(explosion, 0.4f, 0.7f);
}

// ==================== 其他音效 ====================

std::vector<float> SoundGenerator::generateReloadSound(int sampleRate) {
    // 金属滑动声
    auto slide = generatePinkNoise(DEFAULT_SAMPLE_RATE / 10, 0.4f);
    slide = lowpassFilter(slide, 1000.0f, sampleRate);
    applyEnvelope(slide, 0.01f, 0.05f, 0.5f, 0.05f);
    
    // 卡扣声
    auto click = generateSineWave(800.0f, 0.03f, 0.3f, 0.0f, sampleRate);
    applyEnvelope(click, 0.005f, 0.01f, 0.3f, 0.01f);
    
    return concatenateAudio(slide, click);
}

std::vector<float> SoundGenerator::generateEmptyClipSound(int sampleRate) {
    // 空仓挂起的金属声
    return generateSineWave(600.0f, 0.05f, 0.4f, 0.0f, sampleRate);
}

std::vector<float> SoundGenerator::generateBoltActionSound(int sampleRate) {
    // 拉栓声：后拉 + 前推
    auto back = generatePinkNoise(DEFAULT_SAMPLE_RATE / 8, 0.5f);
    back = lowpassFilter(back, 800.0f, sampleRate);
    applyEnvelope(back, 0.01f, 0.05f, 0.4f, 0.05f);
    
    auto silence = std::vector<float>(DEFAULT_SAMPLE_RATE / 20, 0.0f);
    
    auto forward = generatePinkNoise(DEFAULT_SAMPLE_RATE / 8, 0.4f);
    forward = lowpassFilter(forward, 600.0f, sampleRate);
    applyEnvelope(forward, 0.01f, 0.05f, 0.4f, 0.05f);
    
    auto result = concatenateAudio(back, silence);
    result = concatenateAudio(result, forward);
    
    return result;
}

std::vector<float> SoundGenerator::generateHeartbeat(float bpm, int sampleRate) {
    float beatDuration = 60.0f / bpm;
    size_t beatSamples = static_cast<size_t>(beatDuration * sampleRate);
    
    // 心跳声：低频"咚"声
    std::vector<float> heartbeat(beatSamples, 0.0f);
    
    // 第一声 (较强)
    size_t firstBeat = beatSamples / 4;
    for (size_t i = 0; i < firstBeat && i < heartbeat.size(); ++i) {
        float t = static_cast<float>(i) / firstBeat;
        heartbeat[i] = 0.6f * std::sin(2.0f * 3.14159265359f * 50.0f * t / beatDuration) * (1.0f - t);
    }
    
    // 第二声 (较弱)
    size_t secondBeat = beatSamples / 2;
    for (size_t i = 0; i < firstBeat && secondBeat + i < heartbeat.size(); ++i) {
        float t = static_cast<float>(i) / firstBeat;
        heartbeat[secondBeat + i] = 0.3f * std::sin(2.0f * 3.14159265359f * 40.0f * t / beatDuration) * (1.0f - t);
    }
    
    applyFadeOut(heartbeat, beatSamples / 10);
    
    return heartbeat;
}

} // namespace audio
} // namespace arena
