/**
 * Audio.cpp- DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "DGame/Audio.h"

#include <format>

using namespace std;

namespace DGame {

namespace Audio2D {

  Wav::Wav(const char *fileName)
  {
    // desired
    //= {.freq = 48000, .format = AUDIO_F32, .channels = 2, .samples = 4096};

    if(SDL_LoadWAV(fileName, &spec, &wav_buffer, &wav_length) == NULL)
    {
      // File not found
      throw runtime_error(format("{}", SDL_GetError()));
    }
    spec.callback =
      [](void *userdata, Uint8 *stream, int len) {
        memset(stream, 0, len);
        auto &self = *(Wav *)userdata;

        auto remaining = self.wav_length - self.pos;

        len = len < remaining ? len : remaining;

        SDL_MixAudioFormat(
          stream,
          self.wav_buffer + self.pos,
          self.spec.format,
          len,
          SDL_MIX_MAXVOLUME / 4
        );
        self.pos += len;
      },
    spec.userdata = this;
    spec.format = AUDIO_S16;
    id = SDL_OpenAudioDevice(NULL, 0, &spec, &spec, 0);
  }

  void
  Wav::Play()
  {
    pos = spec.channels * spec.samples;
    SDL_PauseAudioDevice(id, 0);
  }

  Wav::~Wav()
  {
    SDL_FreeWAV(wav_buffer);
    SDL_CloseAudioDevice(id);
  }

} // namespace Audio2D
} // namespace DGame
