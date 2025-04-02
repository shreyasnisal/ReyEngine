# ReyEngine

![Static Badge](https://img.shields.io/badge/C%2B%2B-deepskyblue?style=flat)
![Static Badge](https://img.shields.io/badge/Custom%20Engine-dodgerblue?style=flat)
![Static Badge](https://img.shields.io/badge/Virtual%20Reality-darkviolet?style=flat)


This is my custom C++ game engine with math utilities, support for Virtual Reality (OpenXR), multithreading, networking (WinSock2), heat maps, XML parsing (tinyxml2), OBJ model loading using a custom parser, 2D sprite animations and more.

> This project was developed using Visual Studio, and it is recommended that Visual Studio and the MSVC toolchain be used for compiling from source. For best results, use VS2022.

I've developed several games using this engine:
- [ArchiLeapVR](https://github.com/shreyasnisal/ArchiLeapVR) : A virtual reality (VR) first-person platformer with an in-game level editor
- [ReyTD](https://github.com/shreyasnisal/ReyTD) : A 3D tower-defense game
- [SimpleMiner](https://github.com/shreyasnisal/SimpleMiner) : A minecraft-style world generator with multithreaded chunk logic and deterministic infinite worlds
- [Vaporum](https://github.com/shreyasnisal/Vaporum) : A 3D networked (WinSock2) turn-based tank strategy game on a hex tile grid
- [Doomenstein](https://github.com/shreyasnisal/Doomenstein) : A first-person shooting game
- [Starship](https://github.com/shreyasnisal/Starship) : A 2D space shooter game
- [Libra](https://github.com/shreyasnisal/Libra) : A top-down 2D tank game

This project compiles into a static library (LIB) that is used by games. To run games, clone a repository and ensure that the following directory structure is maintained:

```
Engine
└── Code
Game
├──Code
└──Run
```
