#include "AudioEngine.h"
#include <iostream>
#include <cstring>

namespace arena {
namespace audio {

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

AudioEngine::~AudioEngine() {
    if (m_initialized) {
        shutdown();
    }
}

bool AudioEngine::initialize() {
    if (m_initialized) {
        return true;
    }

    // 打开默认设备
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        std::cerr << "[Audio] Failed to open OpenAL device" << std::endl;
        return false;
    }

    // 创建上下文
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        std::cerr << "[Audio] Failed to create OpenAL context" << std::endl;
        alcCloseDevice(m_device);
        m_device = nullptr;
        return false;
    }

    // 激活上下文
    if (!alcMakeContextCurrent(m_context)) {
        std::cerr << "[Audio] Failed to make context current" << std::endl;
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        m_context = nullptr;
        m_device = nullptr;
        return false;
    }

    // 初始化 OpenAL
    alGetError(); // 清除错误
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "[Audio] OpenAL initialization error: " << alGetString(error) << std::endl;
    }

    // 设置距离模型 (逆距离衰减)
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    m_initialized = true;
    std::cout << "[Audio] OpenAL initialized successfully" << std::endl;
    std::cout << "[Audio] Vendor: " << alGetString(AL_VENDOR) << std::endl;
    std::cout << "[Audio] Version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "[Audio] Renderer: " << alGetString(AL_RENDERER) << std::endl;

    return true;
}

void AudioEngine::shutdown() {
    if (!m_initialized) {
        return;
    }

    // 停止所有声音
    stopAllSounds();

    // 销毁上下文
    alcMakeContextCurrent(nullptr);
    if (m_context) {
        alcDestroyContext(m_context);
        m_context = nullptr;
    }

    // 关闭设备
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }

    m_initialized = false;
    std::cout << "[Audio] Shutdown complete" << std::endl;
}

void AudioEngine::setMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
    updateVolumes();
}

void AudioEngine::updateListener(const float pos[3], const float front[3], const float up[3], const float vel[3]) {
    if (!m_initialized) return;

    ALfloat listenerPos[3] = { pos[0], pos[1], pos[2] };
    ALfloat listenerVel[3] = { vel[0], vel[1], vel[2] };
    ALfloat orientation[6] = { 
        front[0], front[1], front[2],  // Forward
        up[0], up[1], up[2]            // Up
    };

    alListenerfv(AL_POSITION, listenerPos);
    alListenerfv(AL_VELOCITY, listenerVel);
    alListenerfv(AL_ORIENTATION, orientation);
}

unsigned int AudioEngine::playSound(unsigned int bufferId, float volume, bool loop) {
    if (!m_initialized) return 0;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    ALuint source;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[Audio] Failed to generate source" << std::endl;
        return 0;
    }

    alSourcei(source, AL_BUFFER, bufferId);
    alSourcef(source, AL_GAIN, volume * m_masterVolume * m_soundVolume);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    alSourcePlay(source);

    unsigned int sourceId = m_nextSourceId++;
    m_sources[sourceId] = source;

    return sourceId;
}

unsigned int AudioEngine::playSound3D(unsigned int bufferId, const float pos[3], float volume, bool loop) {
    if (!m_initialized) return 0;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    ALuint source;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "[Audio] Failed to generate 3D source" << std::endl;
        return 0;
    }

    alSourcei(source, AL_BUFFER, bufferId);
    alSourcef(source, AL_GAIN, volume * m_masterVolume * m_soundVolume);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);
    alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(source, AL_REFERENCE_DISTANCE, 5.0f);
    alSourcef(source, AL_MAX_DISTANCE, 100.0f);

    alSourcePlay(source);

    unsigned int sourceId = m_nextSourceId++;
    m_sources[sourceId] = source;

    return sourceId;
}

void AudioEngine::stopSound(unsigned int sourceId) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSourceStop(it->second);
        alDeleteSources(1, &it->second);
        m_sources.erase(it);
    }
}

void AudioEngine::stopAllSounds() {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    for (auto& pair : m_sources) {
        alSourceStop(pair.second);
        alDeleteSources(1, &pair.second);
    }
    m_sources.clear();
}

void AudioEngine::setSourcePosition(unsigned int sourceId, const float pos[3]) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSource3f(it->second, AL_POSITION, pos[0], pos[1], pos[2]);
    }
}

void AudioEngine::setSourceVelocity(unsigned int sourceId, const float vel[3]) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSource3f(it->second, AL_VELOCITY, vel[0], vel[1], vel[2]);
    }
}

void AudioEngine::setSourceVolume(unsigned int sourceId, float volume) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSourcef(it->second, AL_GAIN, volume * m_masterVolume * m_soundVolume);
    }
}

void AudioEngine::setSourcePitch(unsigned int sourceId, float pitch) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSourcef(it->second, AL_PITCH, pitch);
    }
}

void AudioEngine::setSourceLooping(unsigned int sourceId, bool loop) {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        alSourcei(it->second, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }
}

bool AudioEngine::isPlaying(unsigned int sourceId) const {
    if (!m_initialized) return false;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    auto it = m_sources.find(sourceId);
    if (it != m_sources.end()) {
        ALint state;
        alGetSourcei(it->second, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }
    return false;
}

void AudioEngine::updateVolumes() {
    if (!m_initialized) return;

    std::lock_guard<std::mutex> lock(m_sourceMutex);

    for (auto& pair : m_sources) {
        // 这里可以扩展为根据音效类型应用不同的音量
        // 目前简化处理，实际使用时需要在播放时记录音效类型
    }
}

} // namespace audio
} // namespace arena
