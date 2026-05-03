#pragma once

#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

namespace arena {
namespace audio {

// 程序化音效生成器
class SoundGenerator {
public:
    static constexpr int DEFAULT_SAMPLE_RATE = 44100;

    // ==================== 枪声生成 ====================
    
    // 枪械音效参数
    struct GunSoundParams {
        float baseFreq = 100.0f;      // 基频 (Hz)
        float decayRate = 0.98f;      // 衰减率
        float noiseAmount = 0.7f;     // 噪声比例
        float duration = 0.3f;        // 时长 (秒)
        float lowFreqRatio = 0.3f;    // 低频比例
        float highFreqRatio = 0.2f;   // 高频比例
        float mechanicalClick = 0.1f; // 机械撞击声
    };

    // 生成枪声
    static std::vector<float> generateGunshot(const GunSoundParams& params, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 预设枪械类型
    static GunSoundParams getGunParams(const std::string& gunType);
    
    // 快捷生成函数
    static std::vector<float> generateAKGunshot(int sampleRate = DEFAULT_SAMPLE_RATE);
    static std::vector<float> generateM4Gunshot(int sampleRate = DEFAULT_SAMPLE_RATE);
    static std::vector<float> generateSniperGunshot(int sampleRate = DEFAULT_SAMPLE_RATE);
    static std::vector<float> generateShotgunGunshot(int sampleRate = DEFAULT_SAMPLE_RATE);
    static std::vector<float> generatePistolGunshot(int sampleRate = DEFAULT_SAMPLE_RATE);
    static std::vector<float> generateSMGGunshot(int sampleRate = DEFAULT_SAMPLE_RATE);

    // ==================== 脚步声生成 ====================
    
    enum class SurfaceType {
        Concrete,   // 混凝土 (高频)
        Dirt,       // 泥土 (中频)
        Metal,      // 金属 (中高频 + 余音)
        Wood,       // 木板
        Grass,      // 草地
        Gravel      // 碎石
    };

    // 生成单个脚步声
    static std::vector<float> generateFootstep(SurfaceType surface, bool isRun, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 获取步频间隔 (秒)
    static float getFootstepInterval(bool isRun) { return isRun ? 0.25f : 0.5f; }

    // ==================== 环境音生成 ====================
    
    // 风声：低频噪声 + LFO 调制
    static std::vector<float> generateWind(float duration = 2.0f, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 鸟鸣：正弦波 FM 合成
    static std::vector<float> generateBirdChirp(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 远处枪声：低通滤波 + 混响
    static std::vector<float> generateDistantGunshot(const std::vector<float>& original, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 雨声：大量低频噪声叠加
    static std::vector<float> generateRain(float duration = 3.0f, float intensity = 0.5f, int sampleRate = DEFAULT_SAMPLE_RATE);

    // ==================== UI 音效生成 ====================
    
    // 按钮点击：短促正弦波
    static std::vector<float> generateButtonClick(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 确认音：C5→E5 上升
    static std::vector<float> generateConfirmSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 取消音：E5→C5 下降
    static std::vector<float> generateCancelSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 警告音：锯齿波
    static std::vector<float> generateWarningSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 拾取物品：轻快叮当声
    static std::vector<float> generatePickupSound(int sampleRate = DEFAULT_SAMPLE_RATE);

    // ==================== 爆炸音效 ====================
    
    // 爆炸：白噪声爆发 + 低频 rumble + 延迟回响
    static std::vector<float> generateExplosion(float duration = 1.5f, int sampleRate = DEFAULT_SAMPLE_RATE);

    // ==================== 其他音效 ====================
    
    // 换弹声
    static std::vector<float> generateReloadSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 空仓挂起
    static std::vector<float> generateEmptyClipSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 拉栓声 (栓动步枪)
    static std::vector<float> generateBoltActionSound(int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 心跳声 (受伤时)
    static std::vector<float> generateHeartbeat(float bpm = 100.0f, int sampleRate = DEFAULT_SAMPLE_RATE);

private:
    // 工具函数
    
    // 生成白噪声
    static std::vector<float> generateWhiteNoise(size_t samples, float amplitude = 1.0f);
    
    // 生成粉红噪声 (1/f 噪声)
    static std::vector<float> generatePinkNoise(size_t samples, float amplitude = 1.0f);
    
    // 生成正弦波
    static std::vector<float> generateSineWave(float freq, float duration, float amplitude = 1.0f, float phase = 0.0f, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 生成锯齿波
    static std::vector<float> generateSawWave(float freq, float duration, float amplitude = 1.0f, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 生成方波
    static std::vector<float> generateSquareWave(float freq, float duration, float amplitude = 1.0f, int sampleRate = DEFAULT_SAMPLE_RATE);
    
    // 应用包络 (ADSR)
    static void applyEnvelope(std::vector<float>& samples, float attack, float decay, float sustain, float release);
    
    // 应用指数衰减
    static void applyExponentialDecay(std::vector<float>& samples, float decayFactor);
    
    // 简单低通滤波
    static std::vector<float> lowpassFilter(const std::vector<float>& input, float cutoff, int sampleRate);
    
    // 简单高通滤波
    static std::vector<float> highpassFilter(const std::vector<float>& input, float cutoff, int sampleRate);
    
    // 带通滤波
    static std::vector<float> bandPassFilter(const std::vector<float>& input, float lowCut, float highCut, int sampleRate);
    
    // 添加混响 (简易卷积)
    static std::vector<float> addReverb(const std::vector<float>& input, float mix = 0.3f, float decay = 0.5f);
    
    // 淡入淡出
    static void applyFadeIn(std::vector<float>& samples, size_t fadeSamples);
    static void applyFadeOut(std::vector<float>& samples, size_t fadeSamples);
    
    // 混合两个音频
    static std::vector<float> mixAudio(const std::vector<float>& a, const std::vector<float>& b, float mixA = 0.5f, float mixB = 0.5f);
    
    // 连接两个音频
    static std::vector<float> concatenateAudio(const std::vector<float>& a, const std::vector<float>& b);
    
    // 随机数生成器
    static std::mt19937& getRandomEngine();
};

} // namespace audio
} // namespace arena
