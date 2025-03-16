#pragma once

/*! \mainpage SD Game Engine
*
* \author Shreyas 'Rey' Nisal
* 
* \section intro_sec Introduction
*
* Welcome to the documentation of my SD Game Engine. The engine is currently under development and uses DirectX 11 for graphics rendering and supports the development of 2D and 3D games.
*
* \section install_sec Installation and Usage
* 
* If you're using the pre-compiled version of the engine, you should have the the following files: Engine.lib, fmod.dll, fmod64.dll. Ensure that your project includes these during the linking stage and you should be able to use classes and functions from the engine directly in your game.
* Note: Any games using the engine must contain EngineBuildPreferences.hpp which optionally defines ENGINE_DISABLE_AUDIO (if your game does not use the engine AudioSystem) and ENGINE_DEBUG_RENDER in the debug render configuration to log DirectX live objects on shutdown.
* 
* 
* \subsection rights_info Acknowledgements and Rights
*
* This engine is currently under development towards partial completion of the Software Development courses for the Master of Interactive Technology in Digital Game Development at Southern Methodist University (SMU) Guildhall. My heartiest gratitude to Prof. Squirrel Eiserloh and Prof. Matt Butler for their guidance in the development of this engine. All rights to this engine are held by SMU Guildhall.
* 
*/
