#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

// Step 1: Simple sound loading and playing
class SimpleSoundManager {
public:
  // Singleton pattern - only one sound manager
  static SimpleSoundManager &Instance() {
    static SimpleSoundManager instance;
    return instance;
  }

  // Core functions - keep it minimal
  bool Initialize();
  bool IsInitalized();
  void Shutdown();

  bool LoadSound(const std::string &name, const std::string &filepath);
  void PlaySound(const std::string &name, float volume = 1.0f);
  void SetMasterVolume(float volume);

  void PlaySound(const std::string &name, bool loop);
  void StopSound(const std::string &name);
  bool IsSoundPlaying(const std::string &name);
  void SetSoundVolume(const std::string &name, float volume);

  void PlayBackgroundMusic(const std::string &name, float volume = 0.7f);
  void StopBackgroundMusic();
  void SetMusicVolume(float volume);

private:
  // Private constructor for singleton
  SimpleSoundManager() = default;
  ~SimpleSoundManager() = default;

  bool _is_initalized = false;
  std::mutex _lock;

  // OpenAL context
  ALCdevice *device = nullptr;
  ALCcontext *context = nullptr;

  struct Sound {
    ALuint buffer = 0;
    ALuint source = 0;
    bool isLooping = false;
    float volume = 1.0f;
    // one-shot retrigger guard: same sound within this window is skipped,
    // so rapid repeats (tool spam, machine bursts) can't comb into static
    std::chrono::steady_clock::time_point last_played{};
  };

  static constexpr float MIN_RETRIGGER_SECONDS = 0.15f;

  std::unordered_map<std::string, Sound> sounds;
  float masterVolume = 1.0f;
  float musicVolume = 0.7f; // Separate volume for music

  std::string currentBackgroundMusic = ""; // Track what's playing

  static const int ONESHOT_POOL = 32;
  ALuint m_pool[ONESHOT_POOL] = {};
  int m_pool_next = 0;

  bool LoadWAV(const std::string &filepath, ALuint buffer);
  bool LoadOGG(const std::string &filepath, ALuint buffer);
  void UpdateSoundVolume(const std::string &name);
};
