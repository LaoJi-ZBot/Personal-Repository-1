#pragma once

#include <AL/al.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace arena {
namespace audio {

// 音效缓冲区管理类
class SoundBuffer {
public:
    SoundBuffer();
    ~SoundBuffer();

    // 禁止拷贝
    SoundBuffer(const SoundBuffer&) = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;

    // 允许移动
    SoundBuffer(SoundBuffer&& other) noexcept;
    SoundBuffer& operator=(SoundBuffer&& other) noexcept;

    // 从 PCM 数据创建缓冲区
    bool createFromPCM(const std::vector<float>& samples, int sampleRate, bool stereo = false);
    bool createFromPCM16(const std::vector<short>& samples, int sampleRate, bool stereo = false);

    // 获取 OpenAL 缓冲区 ID
    ALuint getBufferId() const { return m_bufferId; }

    // 是否有效
    bool isValid() const { return m_bufferId != 0; }

    // 获取时长 (秒)
    float getDuration() const { return m_duration; }

private:
    ALuint m_bufferId = 0;
    float m_duration = 0.0f;
    int m_sampleRate = 44100;
    bool m_stereo = false;
};

// 音效资源缓存
class SoundBufferCache {
public:
    static SoundBufferCache& getInstance();

    // 获取或创建缓冲区
    unsigned int getOrCreate(const std::string& name, const std::vector<float>& samples, int sampleRate = 44100);
    
    // 通过 ID 获取缓冲区
    ALuint getBuffer(unsigned int id) const;
    
    // 通过名称获取缓冲区 ID
    unsigned int getBufferIdByName(const std::string& name) const;
    
    // 检查是否存在
    bool hasBuffer(const std::string& name) const;

private:
    SoundBufferCache() = default;
    
    std::unordered_map<std::string, unsigned int> m_nameToId;
    std::unordered_map<unsigned int, std::unique_ptr<SoundBuffer>> m_buffers;
    unsigned int m_nextId = 1;
};

} // namespace audio
} // namespace arena
