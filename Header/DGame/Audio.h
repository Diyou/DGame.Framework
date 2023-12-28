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
namespace Audio3D {
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
      if(!alcMakeContextCurrent(device.context))
      {
        switch(alGetError())
        {
        case ALC_INVALID_CONTEXT:
          throw std::runtime_error("The specified context is invalid");
          break;
        default:
          throw std::runtime_error("Unknown error.");
          break;
        }
      }
    }

    Device(const char *DeviceName = NULL)
    {
      ALCenum error;
      handle = alcOpenDevice(DeviceName);
      if(handle == NULL)
      {
        throw std::runtime_error("Could not open Audio Device");
      }
      error = alGetError();

      context = alcCreateContext(handle, NULL);
      error = alGetError();
      if(context == NULL)
      {
        switch(error)
        {
        case ALC_INVALID_DEVICE:
          throw std::runtime_error(
            "The specified device is not a valid output device."
          );
          break;
        case ALC_INVALID_VALUE:
          throw std::runtime_error(
            "An additional context can not be created for this device."
          );
          break;
        default:
          throw std::runtime_error("Unknown error.");
          break;
        }
      }
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
    ALuint Source, Buffer;

    Wav(const char *fileName);

    void Play();

    virtual ~Wav();
  };
} // namespace Audio3D
} // namespace DGame
