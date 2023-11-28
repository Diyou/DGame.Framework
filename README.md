# DGame.Framework (Work in Progress)

This Framework sets up a basic environment to begin game development with webgpu in c++ including the [essential tools](#libraries) and [examples](Examples).

## Disclaimer

DGame.Framework and webgpu are both in an early experimental stage.  
Instabilities and breakages should be expected.
The api and license of DGame.Framework might be reevaluated in the future.  
It is adviced to refrain from using this in production and may rather be used for educational purposes for the time being.

See [Implementation Status](https://github.com/gpuweb/gpuweb/wiki/Implementation-Status) for Browser Support

## Table of Content

- [Known Issues](#known-issues)
- [Build Dependencies](#build-dependencies)
- [Libraries](#libraries)
- [Examples](#examples)
- [License](#license)

### Known Issues

- Wayland window freezes with PresentMode::Fifo with nvidia proprietary drivers > 545.29

### Build Dependencies

- [cmake](https://cmake.org/) > 3.24
- A c++ build environment (clang, msvc, gcc)

### Libraries

| Library                                                     | Usecase                             | License                                                                                 |
| :---------------------------------------------------------- | :---------------------------------- | :-------------------------------------------------------------------------------------- |
| [boost](https://github.com/boostorg/boost)                  | Thread Pools, Netcode               | [BSL-1.0](https://https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt)        |
| [SDL2](https://github.com/libsdl-org/SDL)                   | Window creation, Audio, User events | [Zlib](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt)                         |
| [glm](https://github.com/g-truc/glm)                        | Vector Math                         | [MIT](https://github.com/g-truc/glm/blob/master/copying.txt)(*)                         |
| [emscripten](https://github.com/emscripten-core/emscripten) | WASM webgpu implementation          | [MIT](https://github.com/emscripten-core/emscripten/blob/main/LICENSE)(*)               |
| [dawn](https://dawn.googlesource.com/)                      | Native webgpu implementation        | [BSD-3](https://dawn.googlesource.com/dawn/+/HEAD/LICENSE)                              |
| [OpenUSD](https://github.com/PixarAnimationStudios/OpenUSD) | Scene parsing                       | [Apache-2](https://github.com/PixarAnimationStudios/OpenUSD/blob/release/LICENSE.txt)(*)|

(*) Has multiple or modified versions of the license. See the link for more information.
### Examples

- [HelloTriangle](https://diyou.github.io/DGame.Framework/HelloTriangle.html) Running two Triangles in parallel

### License

DGame.Framework is licensed under the [MIT License](LICENSE)
