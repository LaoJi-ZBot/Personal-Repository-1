#include "SoundBuffer.h"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace arena {
namespace audio {

// ==================== SoundBuffer ====================

SoundBuffer::SoundBuffer() = default;

SoundBuffer::~SoundBuffer() {
    if (m_bufferId != 0) {
        alDeleteBuffers(1, &m_bufferId);
        m_bufferId = 0;
    }
}

SoundBuffer::SoundBuffer(SoundBuffer&& other) noexcept
    : m_bufferId(other.m_bufferId)
    , m_duration(other.m_duration)
    , m_sampleRate(other.m_sampleRate)
    , m_stereo(other.m_stereo)
{
    other.m_bufferId = 0;
}

SoundBuffer& SoundBuffer::operator=(SoundBuffer&& other) noexcept {
    if (this != &other) {
        if (m_bufferId != 0) {
            alDeleteBuffers(1, &m_bufferId);
        }
        m_bufferId = other.m_bufferId;
        m_duration = other.m_duration;
        m_sampleRate = other.m_sampleRate;
        m_stereo = other.m_stereo;
        other.m_bufferId = 0;
    }
    return *this;
}

bool SoundBuffer::createFromPCM(const std::vector<float>& samples, int sampleRate, bool stereo) {
    if (samples.empty()) {
        std::cerr << "[SoundBuffer] Empty samples" << std::endl;
        return false;
    }

    // 清理旧缓冲区
    if (m_bufferId != 0) {
        alDeleteBuffers(1, &m_bufferId);
        m_bufferId = 0;
    }

    // 生成缓冲区
    ALuint bufferId;
    alGenBuffers(1, &bufferId);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[SoundBuffer] Failed to generate buffer" << std::endl;
        return false;
    }

    // 将 float [-1, 1] 转换为 int16
    std::vector<short> pcmData(samples.size());
    for (size_t i = 0; i < samples.size(); ++i) {
        float s = std::clamp(samples[i], -1.0f, 1.0f);
        pcmData[i] = static_cast<short>(s * 32767.0f);
    }

    // 应用 Hann 窗淡出避免爆音
    size_t fadeSamples = std::min<size_t>(pcmData.size(), sampleRate / 100); // 10ms 淡出
    for (size_t i = 0; i < fadeSamples; ++i) {
        size_t idx = pcmData.size() - 1 - i;
        float window = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * static_cast<float>(i) / fadeSamples));
        pcmData[idx] = static_cast<short>(pcmData[idx] * window);
    }

    // 上传到 OpenAL
    ALenum format = stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    ALsizei size = static_cast<ALsizei>(pcmData.size() * sizeof(short));
    
    alBufferData(bufferId, format, pcmData.data(), size, sampleRate);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[SoundBuffer] Failed to upload buffer data" << std::endl;
        alDeleteBuffers(1, &bufferId);
        return false;
    }

    m_bufferId = bufferId;
    m_sampleRate = sampleRate;
    m_stereo = stereo;
    m_duration = static_cast<float>(samples.size()) / (sampleRate * (stereo ? 2 : 1));

    return true;
}

bool SoundBuffer::createFromPCM16(const std::vector<short>& samples, int sampleRate, bool stereo) {
    if (samples.empty()) {
        std::cerr << "[SoundBuffer] Empty samples" << std::endl;
        return false;
    }

    // 清理旧缓冲区
    if (m_bufferId != 0) {
        alDeleteBuffers(1, &m_bufferId);
        m_bufferId = 0;
    }

    // 生成缓冲区
    ALuint bufferId;
    alGenBuffers(1, &bufferId);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[SoundBuffer] Failed to generate buffer" << std::endl;
        return false;
    }

    // 复制样本并应用淡出
    std::vector<short> pcmData = samples;
    size_t fadeSamples = std::min<size_t>(pcmData.size(), sampleRate / 100);
    for (size_t i = 0; i < fadeSamples; ++i) {
        size_t idx = pcmData.size() - 1 - i;
        float window = 0.5f * (1.0f - std::cos(2.0f * 3.14159265f * static_cast<float>(i) / fadeSamples));
        pcmData[idx] = static_cast<short>(pcmData[idx] * window);
    }

    // 上传到 OpenAL
    ALenum format = stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    ALsizei size = static_cast<ALsizei>(pcmData.size() * sizeof(short));
    
    alBufferData(bufferId, format, pcmData.data(), size, sampleRate);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[SoundBuffer] Failed to upload buffer data" << std::endl;
        alDeleteBuffers(1, &bufferId);
        return false;
    }

    m_bufferId = bufferId;
    m_sampleRate = sampleRate;
    m_stereo = stereo;
    m_duration = static_cast<float>(samples.size()) / (sampleRate * (stereo ? 2 : 1));

    return true;
}

// ==================== SoundBufferCache ====================

SoundBufferCache& SoundBufferCache::getInstance() {
    static SoundBufferCache instance;
    return instance;
}

unsigned int SoundBufferCache::getOrCreate(const std::string& name, const std::vector<float>& samples, int sampleRate) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return it->second;
    }

    auto buffer = std::make_unique<SoundBuffer>();
    if (!buffer->createFromPCM(samples, sampleRate)) {
        return 0;
    }

    unsigned int id = m_nextId++;
    m_nameToId[name] = id;
    m_buffers[id] = std::move(buffer);

    return id;
}

ALuint SoundBufferCache::getBuffer(unsigned int id) const {
    auto it = m_buffers.find(id);
    if (it != m_buffers.end() && it->second) {
        return it->second->getBufferId();
    }
    return 0;
}

unsigned int SoundBufferCache::getBufferIdByName(const std::string& name) const {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return it->second;
    }
    return 0;
}

bool SoundBufferCache::hasBuffer(const std::string& name) const {
    return m_nameToId.find(name) != m_nameToId.end();
}

} // namespace audio
} // namespace arena
