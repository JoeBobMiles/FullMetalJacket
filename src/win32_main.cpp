/**
 * @file win32.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-04-15
 *
 * This is our Windows platform layer and entry point. When compiling this game
 * project on Windows, you want to target this file as your main file.
 */

// Include the Windows system specific header, with Unicode support.
#include <windows.h>

// Include Vulkan headers.
#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

// Include platform header.
#include "platform.h"

/* BEGIN struct declarations. */

// TODO[joe] Find a place for this to live that isn't here.
// Could put this in a generic "platform" header.
typedef struct {
    unsigned int   Width;
    unsigned int   Height;
    VkInstance     Instance;
    VkDevice       Device;
    VkQueue        PresentQueue;
    VkCommandBuffer SetupCommandBuffer;
    VkCommandBuffer DrawCommandBuffer;
    VkSwapchainKHR  SwapChain;
    VkImage*        PresentImages;
    VkImage         DepthImage;
    VkImageView     DepthImageView;
    VkDebugReportCallbackEXT         Callback;
    VkSurfaceKHR                     Surface;
    VkPhysicalDevice                 PhysicalDevice;
    VkPhysicalDeviceProperties       PhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    unsigned int               PresentQueueIndex;
} vulkan_context;

/* END struct declarations. */

// Include other parts of the application.
#include "win32_vulkan_setup.cpp"

// NOTE[joe] Temporary globals
static int ApplicationQuit;
static vulkan_context Context;

/** Render black to the screen instead of white. */
static
void GameRender(vulkan_context Context)
{
    unsigned int NextImageIndex;
    vkAcquireNextImageKHR(Context.Device,
                          Context.SwapChain,
                          UINT64_MAX,
                          // FIXME[joe] Can't have null semaphore _and_ fence.
                          // One of these must be a vaild handle.
                          VK_NULL_HANDLE,
                          VK_NULL_HANDLE,
                          &NextImageIndex);

    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Context.SwapChain;
    PresentInfo.pImageIndices = &NextImageIndex;

    // Submits the contents of our presnt queue to be draw to the screen.
    vkQueuePresentKHR(Context.PresentQueue, &PresentInfo);
}

// TODO[joe] Find a better place for this.
#ifdef DEBUG
static
void Assert(bool Flag, const char* Message)
{
    if (!Flag)
    {
        OutputDebugStringA("ASSERT FAILED: ");
        OutputDebugStringA(Message);

        int *_ = 0;
        *_ = 1;
    }
}
#endif

static
void Abort(const char* Message)
{
#ifdef DEBUG
    OutputDebugStringA("ABORT: ");
    OutputDebugStringA(Message);
#else
    MessageBox(0, Message, 0, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
#endif

    abort();
}

/** Callback invoked by Windows when it needs us to do something. */
LRESULT CALLBACK WindowProcedure(HWND Window,
                                 UINT Message,
                                 WPARAM WParameter,
                                 LPARAM LParameter)
{
    switch (Message)
    {
        case WM_PAINT:
        {
            GameRender(Context);
        } break;

        case WM_CLOSE:
        case WM_QUIT:
        {
            ApplicationQuit = 1;
        } break;

        default: break;
    }

    return DefWindowProc(Window, Message, WParameter, LParameter);
}

/** Window's entry point into our game. */
int WINAPI wWinMain(HINSTANCE Instance,     // Current application instance.
                    HINSTANCE PrevInstance, // Previous instance (unused).
                    PWSTR CommandLineArgs,  // Commandline arguments.
                    int ShowCommand)        // Undocumented (unused).
{
    LPCSTR WindowClassName = "FullMetalJacket_WindowClass";

    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = WindowProcedure;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = WindowClassName;

    // NOTE[joe] Is it necessary to check if the class was registered?
    if (RegisterClassEx(&WindowClass))
    {
        HWND Window = CreateWindow(WindowClassName,
                                   "Full Metal Jacket",
                                   WS_OVERLAPPEDWINDOW,
                                   // Default x, y positions.
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   // Default width, height.
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   0,
                                   0,
                                   Instance,
                                   0);

        if (Window)
        {
            Context = win32_InitializeVulkan(Instance, Window);

            ShowWindow(Window, ShowCommand);
            UpdateWindow(Window);

            while (!ApplicationQuit)
            {
                MSG Message = {};
                if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                RedrawWindow(Window, 0, 0, RDW_INTERNALPAINT);
            }
        }
        else
        {
            // TODO[joe] Error reporting.
        }
    }
    else
    {
        // TODO[joe] Error reporting.
    }

    return 0;
}
