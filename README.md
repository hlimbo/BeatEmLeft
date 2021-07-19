# BeatEmLeft
A 2D Game Engine using SDL 2.0 framework. The engine currently uses an x64 build and is built using Visual Studio 2019.

## How to Install
* Used Lazy Foo Productions website to help setup and install the required header files, libraries, and dlls to run the game engine using SDL.
* Follow the installation guide here to have the project running using the base SDL2 header files and libraries for windows using Visual Studio 2019:
https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php

## Custom Dependency
* Vect2.lib and Vect2.h - is a Vector2 library class I created (The two files are included in this project). Contains helper functions such as computing Dot and Cross products.

## Build Dependencies
This project depends on multiple libs and header files for the engine to run the application successfully. For the SDL2 downloads, make sure to download the dependencies under the
Development Libraries section for Windows Visual C++ 2019 x64 (64 bit) version.
Below is a list of DLLs and libs that are required:
* SDL2.0.14 - http://libsdl.org/download-2.0.php 
* SDL2_image-2.0.5 - https://www.libsdl.org/projects/SDL_image/ (Image loading library)
* SDL2_ttf-2.0.15 - https://www.libsdl.org/projects/SDL_ttf/ (True Type Font library)

## Where to link header files to include?

* Click on Project Tab -> Project_Name Properties -> Configuration Properties -> VC++ Directories -> Include Directories
* Click on dropdown button to edit
![alt text](https://i.imgur.com/df553ms.png)

* Specify the location of where the header files are located on your machine for each build dependency above. I included the Custom Dependency for Vect2.h as well which should already be included in this project.
![alt text](https://i.imgur.com/JycrNuH.png)

**Make sure to apply changes when finished**

## Where to link the lib files?

* First, specify where the lib files are located on your machine by going to VC++ Directories -> Library Directories -> Edit
![alt text](https://i.imgur.com/cIbUYRQ.png)

* In the below window, locate and type in the file paths of where each dependency is located:
![alt text](https://i.imgur.com/w7SGW5T.png)

* Under Linker -> Input -> Additional Dependencies -> Edit, specify the lib files to link to this project:
![alt text](https://i.imgur.com/C55XyXq.png)
![alt text](https://i.imgur.com/OCf57sa.png)

**Make sure to apply changes when complete**

## How to run a demo file?
* Under the Experimental directory, there is a collection of `.cpp` files that have the `main()` function defined. Those files are various experiments I played around with
when building out this game engine.
* To run the project successfully, we will be commenting out all of the `.cpp` files in the directory except for `imguiTest.cpp`
* To build project, select Build Tab -> Build Solution
* To run project in debug mode, click on `Local Windows Debugger` button (green play button)

* **Note** Build project is located at C:\path\to\BeatEmLeft\x64\Debug

![alt text](https://i.imgur.com/mDfGetk.png)


## Runtime Dependencies
Ensure to download the required DLLs to run the engine build executable. Without it, there may be run-time errors that you encounter when trying to run the program.
Make sure to download them under the Runtime Binaries Section for 64 bit builds.
Below are the list of DLL dependencies needed:
* SDL2.0.14 - http://libsdl.org/download-2.0.php 
* SDL2_image-2.0.5 - https://www.libsdl.org/projects/SDL_image/ (Image loading library)
* SDL2_ttf-2.0.15 - https://www.libsdl.org/projects/SDL_ttf/ (True Type Font library)

* To run the executable alone, we will need to include the DLL files downloaded in the links above and store them where the .exe file is located.
* If this is not done there will be an error that shows up stating that the following DLLs could not be found which are required to run the executable successfully:

![alt text](https://i.imgur.com/wnNcXwA.png)

## TODOS
* Include installation instructions for gtest (Unit testing library)




