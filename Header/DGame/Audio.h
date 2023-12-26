/**
 * Audio.h - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include "al.h"
#include "alc.h"

#include <SDL_audio.h>

namespace DGame {
/**
 * @brief Audio implementation using SDL_Mixer
 */
namespace Audio2D {
  struct Device
  {
    SDL_AudioSpec spec;
    SDL_AudioDeviceID id;
    Device(const char *deviceName = NULL, const SDL_AudioSpec *desired = NULL);
    virtual ~Device();
  };

  struct Wav
  {
    SDL_AudioSpec spec;
    SDL_AudioDeviceID id;
    Uint32 wav_length;
    Uint8 *wav_buffer;
    int pos;

    Wav(const char *fileName);
    void Play();
    virtual ~Wav();
  };
} // namespace Audio2D

/**
 * @brief Audio implementation using OpenAL
 */
namespace Audio3D{
struct Device
{
  static Device &
  Default()
  {
    static Device instance(NULL);
    return instance;
  }

  static void
  Use(Device &device)
  {
    alcMakeContextCurrent(device.context);
  }

  Device(const char *DeviceName = NULL)
  {
    handle = alcOpenDevice(DeviceName);
    if(handle == NULL)
    {
      throw std::runtime_error("Could not open Audio Device");
    }
    context = alcCreateContext(handle, NULL);
  }

  ~Device()
  {
    alcDestroyContext(context);
    alcCloseDevice(handle);
  }

private:
  ALCdevice *handle;
  ALCcontext *context;
};

struct Wav
{
  Wav(const char *fileName)
  {}

  void
  Play()
  {}

  virtual ~Wav()
  {}
};
}
} // namespace DGame
