#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <functional>

namespace arena {
namespace audio {

// 音频引擎单例类
class AudioEngine {
public:
    static AudioEngine& getInstance();
    
    // 禁止拷贝
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
    
    // 初始化/关闭
    bool initialize();
    void shutdown();
    
    // 是否已初始化
    bool isInitialized() const { return m_initialized; }
    
    // 获取 OpenAL 设备和上下文
    ALCdevice* getDevice() const { return m_device; }
    ALCcontext* getContext() const { return m_context; }
    
    // 主音量控制 (0.0 - 1.0)
    void setMasterVolume(float volume);
    float getMasterVolume() const { return m_masterVolume; }
    
    // 分类音量控制
    void setSoundVolume(float volume) { m_soundVolume = volume; updateVolumes(); }
    void setMusicVolume(float volume) { m_musicVolume = volume; updateVolumes(); }
    void setUIVolume(float volume) { m_uiVolume = volume; updateVolumes(); }
    
    float getSoundVolume() const { return m_soundVolume; }
    float getMusicVolume() const { return m_musicVolume; }
    float getUIVolume() const { return m_uiVolume; }
    
    // 更新监听器位置和方向 (绑定到 Camera)
    void updateListener(const float pos[3], const float front[3], const float up[3], const float vel[3]);
    
    // 播放音效
    unsigned int playSound(unsigned int bufferId, float volume = 1.0f, bool loop = false);
    unsigned int playSound3D(unsigned int bufferId, const float pos[3], float volume = 1.0f, bool loop = false);
    
    // 停止音效
    void stopSound(unsigned int sourceId);
    void stopAllSounds();
    
    // 设置音源属性
    void setSourcePosition(unsigned int sourceId, const float pos[3]);
    void setSourceVelocity(unsigned int sourceId, const float vel[3]);
    void setSourceVolume(unsigned int sourceId, float volume);
    void setSourcePitch(unsigned int sourceId, float pitch);
    void setSourceLooping(unsigned int sourceId, bool loop);
    
    // 检查音源是否正在播放
    bool isPlaying(unsigned int sourceId) const;
    
private:
    AudioEngine() = default;
    ~AudioEngine();
    
    void updateVolumes();
    
    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;
    
    bool m_initialized = false;
    float m_masterVolume = 1.0f;
    float m_soundVolume = 1.0f;
    float m_musicVolume = 1.0f;
    float m_uiVolume = 1.0f;
    
    std::unordered_map<unsigned int, ALuint> m_sources; // 自定义 ID -> OpenAL source
    unsigned int m_nextSourceId = 1;
    mutable std::mutex m_sourceMutex;
};

} // namespace audio
} // namespace arena
