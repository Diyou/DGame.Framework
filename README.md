[![Matrix.Build](https://github.com/Diyou/DGame.Framework/actions/workflows/CI.yml/badge.svg?branch=main&event=push)](https://github.com/Diyou/DGame.Framework/actions/workflows/CI.yml)
[![Page.Build](https://github.com/Diyou/DGame.Framework/actions/workflows/PublishPage.yml/badge.svg?branch=main&event=push)](https://github.com/Diyou/DGame.Framework/actions/workflows/PublishPage.yml)

# DGame.Framework (Work in Progress)

This Framework sets up a basic environment to begin game development with webgpu in c++ targeting web and native platforms.  
See [examples](Examples) on how to get started.

## Disclaimer

DGame.Framework and webgpu are both in an early experimental stage.  
Instabilities and breakages should be expected.  
The api and license of DGame.Framework might be reevaluated in the future.  
It is adviced to refrain from using this in production and may rather be used for educational purposes for the time being.

See [Implementation Status](https://github.com/gpuweb/gpuweb/wiki/Implementation-Status) for Browser Support

### Known Issues

#### VSync

- Browser requestAnimationFrame does not track the current monitors frequency (using primary monitor advised)
- High cpu usage on _X11_ windows with PresentMode::(Fifo/Mailbox) on nvidia GPUs
- Native _wayland_ windows cause heavy stutters with PresentMode::Fifo on nvidia GPUs and proprietary drivers > 545.29

### Build Requirements

- [cmake](https://cmake.org/) > 3.24
- A c++20 capable build environment (clang, msvc, gcc)
- [boost](https://www.boost.org/) installed

### Third-party Libraries

| Library                                                     | Usecase                      | License                                                                    |
| :---------------------------------------------------------- | :--------------------------- | :------------------------------------------------------------------------- |
| [SDL2](https://github.com/libsdl-org/SDL)                   | Window creation, Audio, UI   | [Zlib](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt)            |
| [OpenAL-Soft](https://github.com/kcat/openal-soft/)         | Spatial Audio                | [LGPL-2](https://github.com/kcat/openal-soft/blob/master/COPYING)(\*)      |
| [emscripten](https://github.com/emscripten-core/emscripten) | WASM webgpu implementation   | [MIT](https://github.com/emscripten-core/emscripten/blob/main/LICENSE)(\*) |
| [dawn](https://dawn.googlesource.com/)                      | Native webgpu implementation | [BSD-3](https://dawn.googlesource.com/dawn/+/HEAD/LICENSE)                 |

(\*) Has multiple or modified versions of the license. See the link for more information.

### Examples

- [HelloTriangle](https://diyou.github.io/DGame.Framework/HelloTriangle.html) Running 2 rotating Triangles in parallel (~200 Lines of Code)

### License

DGame.Framework is licensed under the [MIT License](LICENSE)
