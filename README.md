# DGame.Framework
A cross-platform framework that bundles elementary open source libraries for webgpu game development

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

### License
DGame.Framework is released under the [MIT License](LICENSE)
