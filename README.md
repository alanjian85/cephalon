# Cephalon
A Minecraft-like voxel game written in C++

## Feature
* Infinite World
* Block Destroy/Place
* Block Texture Array
* Different Types of Block
* Face Culling
* Frustum Culling
* Shadow Mapping
* Ambient Occulusion
* Fog Effect
* Logging
* Screenshot
* JSON Config File
* Procedural Terrain
* Multithread Meshing
* World Save/Load

## TODO
* Greedy Meshing
* Block Destroy Particle
* UI

## Dependencies
* [bgfx](https://bkaradzic.github.io/bgfx/index.html)
* [Boost](https://www.boost.org/)
* [{fmt}](https://github.com/fmtlib/fmt)
* [GLM](https://glm.g-truc.net/)
* [JSON for Modern C++](https://json.nlohmann.me/)
* [SDL2](https://www.libsdl.org/index.php)
* [spdlog](https://github.com/gabime/spdlog)
* [SQLite3](https://www.sqlite.org/index.html)

## Installation
### Linux (Ubuntu/Debian)
Install bgfx
```
sudo apt install build-essential git cmake
sudo apt-get install libgl1-mesa-dev x11proto-core-dev libx11-dev
git clone https://github.com/bkaradzic/bgfx.cmake.git 
cd bgfx.cmake
mkdir build && cd build
cmake ..
sudo make install
```
Build Cephalon
```
sudo apt install libsdl2-dev libfmt-dev
git clone https://github.com/alanjian85/cephalon.git
cd cephalon
mkdir build && cd build
cmake ..
cmake --build .
```