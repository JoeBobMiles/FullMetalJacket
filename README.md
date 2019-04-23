FULL METAL JACKET*
==================

\* Title work in progress.

This is a game that I plan to work on in my free time, building a game engine
from scratch using C and Vulkan. Currently the target platform is Windows, with
the possibility of other platforms being supported in the future.

# Building FULL METAL JACKET

Since I am currently, and for the forseeable future, on Windows, I will only
detail the process of building this project on that operating system. The
prerequisites for to build this project are:

 -  The LLVM clang compiler.
 -  Visual Studio (any version).
 -  Vulkan SDK (minimum version 1.0).

To build the project, make sure you have `clang-cl` in your path, as well as
the Visual Studio dev tools (you will need to track down and run your
`vcvarsall.bat` to do this). Once you have confirmed that both of those are
in your system path, you can run `build.bat` in the scripts folder. It will
produce an debug build of the game (with debug information) in the build
directory.

To build a release version of the game, run `build.bat release`.
