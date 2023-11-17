# DGame.Framework
This Framework sets up a basic environment to begin game development with webgpu in c++ including the [essential tools](#libraries) and [examples](Examples).

## Disclaimer
DGame.Framework and webgpu are both in an early experimental stage.  
Instabilities and breakages should be expected.  
It is adviced to refrain from using this in production and may rather be used for educational purposes for the time being.

## Table of Content
+ [Dependencies](#dependencies)
+ [Libraries](#libraries)
+ [License](#license)

### Dependencies
+ [cmake](https://cmake.org/) > 3.24
+ [SDL2](https://www.libsdl.org/) (portable version included)
+ [emsdk](https://github.com/emscripten-core/emsdk) (optional & portable version included)

### Libraries
Library|Usecase|License
:--- | :--- | :---
[boost](https://www.boost.org/)|Thread Pools,Netcode|[boost-1](https://www.boost.org/users/license.html)
[glm](https://github.com/g-truc/glm)|Vector Math|[MIT](https://github.com/g-truc/glm/blob/master/copying.txt)
[dawn](https://dawn.googlesource.com/)|Native webgpu implementation|[BSD-3](https://dawn.googlesource.com/dawn/+/HEAD/LICENSE)
[OpenUSD](https://github.com/PixarAnimationStudios/OpenUSD)|Scene parsing|[Apache-2](https://github.com/PixarAnimationStudios/OpenUSD/blob/release/LICENSE.txt)

### License
DGame.Framework is released under the [MIT License](LICENSE)
