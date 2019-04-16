/**
 * @file win32.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-04-15
 *
 * This is our Windows platform layer and entry point. When compiling this game
 * project on Windows, you want to target this file as your main file.
 */

// Include the Windows system specific header, with Unicode support.
#define UNICODE
#include <windows.h>

// Include 3rd party headers.
#include <vulkan/vulkan.h>

/** Window's entry point into our game. */
int WINAPI wWinMain(HINSTANCE Instance,     // Current application instance.
                    HINSTANCE PrevInstance, // Previous instance (unused).
                    PWSTR CommandLineArgs,  // Commandline arguments.
                    int ShowCommand)        // Undocumented (unused).
{
    return 0;
}
