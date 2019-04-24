/**
 * @file win32_vulkan_setup.cpp
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-04-20
 *
 * This file contains and performs all of our Vulkan setup. The reason for
 * putting this all in one file is so that we can seperate our Windows
 * interaction code from our Vulkan setup code.
 */

#include "platform.h"
#include "render.h"

// Declare handles to Vulkan functions that we will load later.
static PFN_vkCreateInstance vkCreateInstance;
static PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
static PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
static PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
static PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
static PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
static PFN_vkCreateDevice vkCreateDevice;
static PFN_vkGetDeviceQueue vkGetDeviceQueue;
static PFN_vkCreateCommandPool vkCreateCommandPool;
static PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
static PFN_vkCreateFence vkCreateFence;
static PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
static PFN_vkEndCommandBuffer vkEndCommandBuffer;
static PFN_vkQueueSubmit vkQueueSubmit;
static PFN_vkWaitForFences vkWaitForFences;
static PFN_vkResetFences vkResetFences;
static PFN_vkResetCommandBuffer vkResetCommandBuffer;
static PFN_vkCreateImageView vkCreateImageView;
static PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
static PFN_vkCreateSemaphore vkCreateSemaphore;
static PFN_vkDestroySemaphore vkDestroySemaphore;
static PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
static PFN_vkCreateImage vkCreateImage;
static PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
static PFN_vkAllocateMemory vkAllocateMemory;
static PFN_vkBindImageMemory vkBindImageMemory;
static PFN_vkCreateRenderPass vkCreateRenderPass;
static PFN_vkCreateBuffer vkCreateBuffer;
static PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
static PFN_vkMapMemory vkMapMemory;
static PFN_vkUnmapMemory vkUnmapMemory;
static PFN_vkBindBufferMemory vkBindBufferMemory;
static PFN_vkCreateShaderModule vkCreateShaderModule;

// Vulkan surface extension functions.
static PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
static PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
static PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
static PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
static PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
static PFN_vkQueuePresentKHR vkQueuePresentKHR;

// Vulkan debug extension functions.
static PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
static PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
static PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;

/** Loads the Vulkan DLL and retrieves the functions we need from it. */
static
void win32_LoadVulkan()
{
    HMODULE Vulkan = LoadLibrary("vulkan-1.dll");

    if (Vulkan)
    {
        // NOTE[joe] Welcome to the land of Way-Too-Long-Names.
        // May your all your lines be long and obscure.
        vkCreateInstance = (PFN_vkCreateInstance)
            GetProcAddress(Vulkan, "vkCreateInstance");

        vkEnumerateInstanceLayerProperties =
            (PFN_vkEnumerateInstanceLayerProperties)
            GetProcAddress(Vulkan, "vkEnumerateInstanceLayerProperties");

        vkEnumerateInstanceExtensionProperties =
            (PFN_vkEnumerateInstanceExtensionProperties)
            GetProcAddress(Vulkan, "vkEnumerateInstanceExtensionProperties");

        vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
            GetProcAddress(Vulkan, "vkGetInstanceProcAddr");

        vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)
            GetProcAddress(Vulkan, "vkEnumeratePhysicalDevices");

        vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)
            GetProcAddress(Vulkan, "vkGetPhysicalDeviceProperties");

        vkGetPhysicalDeviceQueueFamilyProperties =
            (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
            GetProcAddress(Vulkan, "vkGetPhysicalDeviceQueueFamilyProperties");

        vkCreateDevice = (PFN_vkCreateDevice) GetProcAddress(Vulkan,
                                                             "vkCreateDevice");

        vkGetDeviceQueue = (PFN_vkGetDeviceQueue)
            GetProcAddress(Vulkan, "vkGetDeviceQueue");

        vkCreateCommandPool = (PFN_vkCreateCommandPool)
            GetProcAddress(Vulkan, "vkCreateCommandPool");

        vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)
            GetProcAddress(Vulkan, "vkAllocateCommandBuffers");

        vkCreateFence = (PFN_vkCreateFence)
            GetProcAddress(Vulkan, "vkCreateFence");

        vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)
            GetProcAddress(Vulkan, "vkBeginCommandBuffer");

        vkEndCommandBuffer = (PFN_vkEndCommandBuffer)
            GetProcAddress(Vulkan, "vkEndCommandBuffer");

        vkQueueSubmit = (PFN_vkQueueSubmit)
            GetProcAddress(Vulkan, "vkQueueSubmit");

        vkWaitForFences = (PFN_vkWaitForFences)
            GetProcAddress(Vulkan, "vkWaitForFences");

        vkResetFences = (PFN_vkResetFences)
            GetProcAddress(Vulkan, "vkResetFences");

        vkResetCommandBuffer = (PFN_vkResetCommandBuffer)
            GetProcAddress(Vulkan, "vkResetCommandBuffer");

        vkCreateImageView = (PFN_vkCreateImageView)
            GetProcAddress(Vulkan, "vkCreateImageView");

        vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)
            GetProcAddress(Vulkan, "vkCmdPipelineBarrier");

        vkCreateSemaphore = (PFN_vkCreateSemaphore)
            GetProcAddress(Vulkan, "vkCreateSemaphore");

        vkDestroySemaphore = (PFN_vkDestroySemaphore)
            GetProcAddress(Vulkan, "vkDestroySemaphore");

        vkGetPhysicalDeviceMemoryProperties =
            (PFN_vkGetPhysicalDeviceMemoryProperties)
            GetProcAddress(Vulkan, "vkGetPhysicalDeviceMemoryProperties");

        vkCreateImage = (PFN_vkCreateImage) GetProcAddress(Vulkan,
                                                           "vkCreateImage");

        vkGetImageMemoryRequirements =
            (PFN_vkGetImageMemoryRequirements)
            GetProcAddress(Vulkan, "vkGetImageMemoryRequirements");

        vkAllocateMemory = (PFN_vkAllocateMemory)
            GetProcAddress(Vulkan, "vkAllocateMemory");

        vkBindImageMemory = (PFN_vkBindImageMemory)
            GetProcAddress(Vulkan, "vkBindImageMemory");

        vkCreateRenderPass = (PFN_vkCreateRenderPass)
            GetProcAddress(Vulkan, "vkCreateRenderPass");

        vkCreateBuffer = (PFN_vkCreateBuffer)
            GetProcAddress(Vulkan, "vkCreateBuffer");

        vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)
            GetProcAddress(Vulkan, "vkGetBufferMemoryRequirements");

        vkMapMemory = (PFN_vkMapMemory) GetProcAddress(Vulkan, "vkMapMemory");

        vkUnmapMemory = (PFN_vkUnmapMemory) GetProcAddress(Vulkan,
                                                           "vkUnmapMemory");

        vkBindBufferMemory = (PFN_vkBindBufferMemory)
            GetProcAddress(Vulkan, "vkBindBufferMemory");

        vkCreateShaderModule = (PFN_vkCreateShaderModule)
            GetProcAddress(Vulkan, "vkCreateShaderModule");
    }
    else
    {
#ifdef DEBUG
        Assert(false, "Could not find vulkan-1.dll.\n");
#else
        Abort("Could not load Vulkan!\n");
#endif
    }
}

/** Loads extension functions from the Vulkan instance we created. */
static
void win32_LoadVulkanExtensions(vulkan_context Context)
{
    /** Load Vulkan surface extension functions. */
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)
        vkGetInstanceProcAddr(Context.Instance, "vkCreateWin32SurfaceKHR");

    vkGetPhysicalDeviceSurfaceSupportKHR =
        (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkGetPhysicalDeviceSurfaceSupportKHR");

    vkGetPhysicalDeviceSurfaceFormatsKHR =
        (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkGetPhysicalDeviceSurfaceFormatsKHR");

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    vkGetPhysicalDeviceSurfacePresentModesKHR =
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkGetPhysicalDeviceSurfacePresentModesKHR");

    vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)
        vkGetInstanceProcAddr(Context.Instance, "vkCreateSwapchainKHR");

    /** Load Vulkan debug extension functions. */
    vkCreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkCreateDebugReportCallbackEXT");

    vkDestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkDestroyDebugReportCallbackEXT");

    vkDebugReportMessageEXT =
        (PFN_vkDebugReportMessageEXT)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkDebugReportMessageEXT");

    vkGetSwapchainImagesKHR =
        (PFN_vkGetSwapchainImagesKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkGetSwapchainImagesKHR");

    vkAcquireNextImageKHR =
        (PFN_vkAcquireNextImageKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkAcquireNextImageKHR");

    vkQueuePresentKHR =
        (PFN_vkQueuePresentKHR)
        vkGetInstanceProcAddr(Context.Instance,
                              "vkQueuePresentKHR");
}

/** Our debug callback for Vulkan. */
static
VKAPI_ATTR VkBool32 VKAPI_CALL win32_VulkanDebugReportCallback(
    VkDebugReportFlagsEXT Flags,
    VkDebugReportObjectTypeEXT ObjectType,
    unsigned long long Object,
    unsigned long long Location,
    int MessageCode,
    const char* LayerPrefix,
    const char* Message,
    void* UserData)
{
    OutputDebugStringA(LayerPrefix);
    OutputDebugStringA(" ");
    OutputDebugStringA(Message);
    OutputDebugStringA("\n");

    return VK_FALSE;
}

/** Initializes Vulkan while also populating and eventually returning a
 * vulkan_context struct that contains all the info we need to deal with
 * Vulkan. */
static
vulkan_context win32_InitializeVulkan(HINSTANCE Instance, HWND Window)
{
    // NOTE[joe] The vulkan_context struct we return.
    vulkan_context Context = {};

    /** Load Vulkan functions from Vulkan DLL */
    win32_LoadVulkan();

    /** Find number of layers and extensions. */

#ifdef DEBUG
    const char *Layers[] = { "VK_LAYER_LUNARG_standard_validation" };
    unsigned int ExpectedLayerCount = sizeof(Layers)/sizeof(char *);

    unsigned int TotalLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&TotalLayerCount, 0);

    VkLayerProperties AvailableLayers[TotalLayerCount];
    vkEnumerateInstanceLayerProperties(&TotalLayerCount,
                                       AvailableLayers);

    int FoundLayers = 0;
    for (unsigned int i = 0; i < TotalLayerCount; i++)
    {
        // FIXME[joe] Implicitly included strcmp()!
        for (unsigned int j = 0; j < ExpectedLayerCount; j++)
        {
            if (strcmp(AvailableLayers[i].layerName, Layers[j]) == 0)
            {
                FoundLayers++;
            }
        }
    }

    // FIXME[joe] Layer discovery has been generalized.
    // This means that in the future, we won't know if which validation
    // layer is missing, only that one is. For now this is tolerable as
    // there is only one layer being loaded.
    Assert(FoundLayers == 1, "Could not find validation layer.\n");
#endif

#ifndef DEBUG
    const char *Extensions[] = { "VK_KHR_surface",
                                 "VK_KHR_win32_surface" };
#else
    const char *Extensions[] = { "VK_KHR_surface",
                                 "VK_KHR_win32_surface",
                                 "VK_EXT_debug_report" };
#endif
    unsigned int ExpectedExtensionCount = sizeof(Extensions)/sizeof(char *);
    
    unsigned int VulkanExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL,
                                           &VulkanExtensionCount,
                                           NULL);

    VkExtensionProperties AvailableExtensions[VulkanExtensionCount];
    vkEnumerateInstanceExtensionProperties(NULL,
                                           &VulkanExtensionCount,
                                           AvailableExtensions);

    unsigned int FoundExtensions = 0;
    for (unsigned int i = 0; i < VulkanExtensionCount; i++)
    {
        for (int j = 0; j < ExpectedExtensionCount; j++)
        {
            // TODO[joe] Flag which extensions we're missing.
            if (strcmp(AvailableExtensions[i].extensionName,
                       Extensions[j]) == 0)
            {
                FoundExtensions++;
            }
        }
    }

    Assert(FoundExtensions == ExpectedExtensionCount,
           "Failed to find all Vulkan extensions.");

    /** Create Vulkan instance. */

    VkApplicationInfo ApplicationInfo = {};
    ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ApplicationInfo.pApplicationName = "Full Metal Jacket";
    ApplicationInfo.engineVersion = 1;
    ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo InstanceInfo = {};
    InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceInfo.pApplicationInfo = &ApplicationInfo;
#ifdef DEBUG
    InstanceInfo.enabledLayerCount = 1;
    InstanceInfo.ppEnabledLayerNames = Layers;
#endif
    // TODO[joe] I don't like how unstable this is.
    // Since we don't know what extensions are present, we are just
    // hoping that Vulkan attempts to load the right ones.
    // This is particularly unstable since we don't abort when the
    // number of expected extensions does not equal the number found.
    InstanceInfo.enabledExtensionCount = FoundExtensions;
    InstanceInfo.ppEnabledExtensionNames = Extensions;


    // TODO[joe] Create allocator for vkCreateInstance?
    VkResult Result = vkCreateInstance(&InstanceInfo,
                                       0,
                                       &Context.Instance);

    Assert(Result == VK_SUCCESS, "Failed to create Vulkan instance.\n");

    /** Load extensions functions. */

    win32_LoadVulkanExtensions(Context);

    /** Create Vulkan debug callback. */
#ifdef DEBUG
    VkDebugReportCallbackCreateInfoEXT CallbackCreateInfo = {};
    CallbackCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    CallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                               VK_DEBUG_REPORT_WARNING_BIT_EXT |
                               VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    CallbackCreateInfo.pfnCallback = &win32_VulkanDebugReportCallback;

    Result = vkCreateDebugReportCallbackEXT(Context.Instance,
                                            &CallbackCreateInfo,
                                            0,
                                            &Context.Callback);

    Assert(Result == VK_SUCCESS, "Failed to create debug report callback.\n");
#endif

    /** Create rendering surface. */

    VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
    SurfaceCreateInfo.sType =
                    VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    SurfaceCreateInfo.hinstance = Instance;
    SurfaceCreateInfo.hwnd = Window;

    Result = vkCreateWin32SurfaceKHR(Context.Instance,
                                     &SurfaceCreateInfo,
                                     0,
                                     &Context.Surface);

    Assert(Result == VK_SUCCESS, "Failed to create surface.\n");

    /** Get physical display device. */

    unsigned int PhysicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(Context.Instance,
                               &PhysicalDeviceCount,
                               0);
    VkPhysicalDevice PhysicalDevices[PhysicalDeviceCount];
    vkEnumeratePhysicalDevices(Context.Instance,
                               &PhysicalDeviceCount,
                               PhysicalDevices);

    for (unsigned int i = 0; i < PhysicalDeviceCount; i++)
    {
        VkPhysicalDeviceProperties DeviceProps = {};
        vkGetPhysicalDeviceProperties(PhysicalDevices[i],
                                      &DeviceProps);

        unsigned int QueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[i],
                                                 &QueueFamilyCount,
                                                 0);

        VkQueueFamilyProperties QueueFamilyProperties[QueueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[i],
                                                 &QueueFamilyCount,
                                                 QueueFamilyProperties);

        for (unsigned int j = 0; j < QueueFamilyCount; j++)
        {
            VkBool32 SupportsPresent;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevices[i],
                                                 j,
                                                 Context.Surface,
                                                 &SupportsPresent);

            if (SupportsPresent &&
                (QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                Context.PhysicalDevice = PhysicalDevices[i];
                Context.PhysicalDeviceProperties = DeviceProps;
                Context.PresentQueueIndex = j;

                break;
            }
        }

        if (Context.PhysicalDevice)
            break;
    }

    // TODO[joe] This is a big issue. Should we abort in release mode?
    Assert(Context.PhysicalDevice, "No physical device detected.\n");

    /** Get physical device memory. */

    vkGetPhysicalDeviceMemoryProperties(Context.PhysicalDevice,
                                        &Context.MemoryProperties);

    /** Create logical display device. */

    VkDeviceQueueCreateInfo QueueCreateInfo = {};
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.queueFamilyIndex = Context.PresentQueueIndex;
    QueueCreateInfo.queueCount = 1;

    // NOTE[joe] Queue priority range is [0, 1].
    float QueuePriorities[] = { 1.0f };
    QueueCreateInfo.pQueuePriorities = QueuePriorities;

    VkDeviceCreateInfo DeviceInfo = {};
    DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceInfo.queueCreateInfoCount = 1;
    DeviceInfo.pQueueCreateInfos = &QueueCreateInfo;
#ifdef DEBUG
    DeviceInfo.enabledLayerCount = 1;
    DeviceInfo.ppEnabledLayerNames = Layers;
#endif

    // NOTE[joe] Load swapchain extension so that we can do buffering.
    const char *DeviceExtensions[] = { "VK_KHR_swapchain" };
    DeviceInfo.enabledExtensionCount = 1;
    DeviceInfo.ppEnabledExtensionNames = DeviceExtensions;

    Result = vkCreateDevice(Context.PhysicalDevice,
                            &DeviceInfo,
                            0,
                            &Context.Device);

    Assert(Result == VK_SUCCESS, "Failed to create logical device.\n");

    // Get the present queue for the device we just created and store it.
    vkGetDeviceQueue(Context.Device,
                     Context.PresentQueueIndex,
                     0,
                     &Context.PresentQueue);

    /** Get our surface's preferred pixel format and colorspace. */

    unsigned int SurfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Context.PhysicalDevice,
                                         Context.Surface,
                                         &SurfaceFormatCount,
                                         0);

    VkSurfaceFormatKHR SurfaceFormats[SurfaceFormatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(Context.PhysicalDevice,
                                         Context.Surface,
                                         &SurfaceFormatCount,
                                         SurfaceFormats);

    VkFormat ColorFormat;
    // NOTE[joe] If the format list includes VK_FORMAT_UNDEFINED, we can choose.
    if (SurfaceFormatCount == 1 &&
        SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        // And we choose the most intuitive one.
        ColorFormat = VK_FORMAT_B8G8R8_UNORM;
    }
    else
    {
        // Otherwise, we pick the first format returned to us.
        ColorFormat = SurfaceFormats[0].format;
    }

    VkColorSpaceKHR ColorSpace;
    ColorSpace = SurfaceFormats[0].colorSpace;

    /** Retrieve surface capabilities (i.e. how many buffers it can support). */

    VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context.PhysicalDevice,
                                              Context.Surface,
                                              &SurfaceCapabilities);

    // NOTE[joe] We want double buffering, so we'll query for that.
    unsigned int DesiredImageCount = 2;
    // Is this even possible??? Seems ridiculous to think that we could end up
    // in a situation where we'll be asking for too _few_ images.
    if (DesiredImageCount < SurfaceCapabilities.minImageCount)
    {
        DesiredImageCount = SurfaceCapabilities.minImageCount;
    }
    else if (SurfaceCapabilities.maxImageCount != 0 &&
             DesiredImageCount > SurfaceCapabilities.maxImageCount)
    {
        DesiredImageCount = SurfaceCapabilities.maxImageCount;
    }
    else
    {
        // TODO[joe] Error reporting or abort.
        // If we get back a maxImageCount of 0, we should assume that something
        // is horribly wrong and exit as soon as possible.
    }

    /** Retrieve surface resolution (or set it if undefined). */

    VkExtent2D SurfaceResolution = SurfaceCapabilities.currentExtent;
    // NOTE[joe] Resolution is undefined when given -1!
    if (SurfaceResolution.width == -1)
    {
        // When width and height are -1 (and they are always both -1), we are
        // allowed to define whatever resolution we want.
        SurfaceResolution.width = Context.Width;
        SurfaceResolution.height = Context.Height;
    }
    else
    {
        Context.Width = SurfaceResolution.width;
        Context.Height = SurfaceResolution.height;
    }

    VkSurfaceTransformFlagBitsKHR PreTransform =
                                        SurfaceCapabilities.currentTransform;
    if (SurfaceCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }

    /** Get the presentation modes supported. */

    unsigned int PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Context.PhysicalDevice,
                                              Context.Surface,
                                              &PresentModeCount,
                                              0);

    VkPresentModeKHR PresentModes[PresentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(Context.PhysicalDevice,
                                              Context.Surface,
                                              &PresentModeCount,
                                              PresentModes);

    // NOTE[joe] This is always supported and our best option for present mode.
    // The reason why this is the best is because it keeps a queue of frames
    // and will perform v-sync, but will not screen-tear if a frame is late.
    VkPresentModeKHR PresentationMode = VK_PRESENT_MODE_FIFO_KHR;
    for (unsigned int i = 0; i < PresentModeCount; i++)
    {
        // However, if VK_PRESENT_MODE_MAILBOX_KHR is supported, we should opt
        // for this because it has lower latency. This is due to the fact that
        // this mode has a 1-entry queue. This means that when a frame is
        // committed, it overwrites the last committed frame. This eliminates
        // the device having to chew its way through a backlog of committed
        // frames. This does mean that instead of latency, we now have frame
        // skips to contend with.
        if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            PresentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }

    /** Create swap chain. */

    VkSwapchainCreateInfoKHR SwapChainCreateInfo = {};
    SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapChainCreateInfo.surface = Context.Surface;
    SwapChainCreateInfo.minImageCount = DesiredImageCount;
    SwapChainCreateInfo.imageFormat = ColorFormat;
    SwapChainCreateInfo.imageColorSpace = ColorSpace;
    SwapChainCreateInfo.imageExtent = SurfaceResolution;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapChainCreateInfo.preTransform = PreTransform;
    SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapChainCreateInfo.presentMode = PresentationMode;
    // NOTE[joe] Toggles clipping outside surface extents.
    SwapChainCreateInfo.clipped = true;

    Result = vkCreateSwapchainKHR(Context.Device,
                                  &SwapChainCreateInfo,
                                  0,
                                  &Context.SwapChain);

    Assert(Result == VK_SUCCESS, "Failed to create swapchain.\n");

    /** Create a command pool. */

    VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolCreateInfo.queueFamilyIndex = Context.PresentQueueIndex;

    VkCommandPool CommandPool;
    Result = vkCreateCommandPool(Context.Device,
                                 &CommandPoolCreateInfo,
                                 0,
                                 &CommandPool);

    Assert(Result == VK_SUCCESS, "Failed to create command pool.");

    /** Create a command buffer for setup and drawing. */

    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
    CommandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = CommandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = 1;

    Result = vkAllocateCommandBuffers(Context.Device,
                                      &CommandBufferAllocateInfo,
                                      &Context.SetupCommandBuffer);

    Assert(Result == VK_SUCCESS, "Failed to allocate setup command buffer.\n");

    Result = vkAllocateCommandBuffers(Context.Device,
                                      &CommandBufferAllocateInfo,
                                      &Context.DrawCommandBuffer);

    Assert(Result == VK_SUCCESS, "Failed to allocate draw command buffer.\n");

    /** Create and initialize color image handles. */

    unsigned int ImageCount = 0;
    vkGetSwapchainImagesKHR(Context.Device,
                            Context.SwapChain,
                            &ImageCount,
                            0);

    VkImage PresentImages[ImageCount];
    Context.PresentImages = PresentImages;

    vkGetSwapchainImagesKHR(Context.Device,
                            Context.SwapChain,
                            &ImageCount,
                            Context.PresentImages);

    VkImageViewCreateInfo PresentImagesViewCreateInfo = {};
    PresentImagesViewCreateInfo.sType =
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    PresentImagesViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    PresentImagesViewCreateInfo.format = ColorFormat;
    PresentImagesViewCreateInfo.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A
    };
    PresentImagesViewCreateInfo.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    PresentImagesViewCreateInfo.subresourceRange.levelCount = 1;
    PresentImagesViewCreateInfo.subresourceRange.layerCount = 1;

    /** Change layout of present images. */

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkFenceCreateInfo FenceCreateInfo = {};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence SubmitFence;
    vkCreateFence(Context.Device,
                  &FenceCreateInfo,
                  0,
                  &SubmitFence);

    int IsModified[ImageCount];
    for (int i = 0; i < ImageCount; i++) IsModified[i] = 0;

    unsigned int ModifiedCount = 0;
    while (ModifiedCount != ImageCount)
    {
        VkSemaphore IsCompleteSemaphore;

        VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(Context.Device,
                          &SemaphoreCreateInfo,
                          0,
                          &IsCompleteSemaphore);

        unsigned int NextImageIndex;
        vkAcquireNextImageKHR(Context.Device,
                              Context.SwapChain,
                              UINT64_MAX,
                              IsCompleteSemaphore,
                              VK_NULL_HANDLE,
                              &NextImageIndex);

        if (!IsModified[NextImageIndex])
        {
            vkBeginCommandBuffer(Context.SetupCommandBuffer, &BeginInfo);

            VkImageMemoryBarrier LayoutTransitionBarrier = {};
            LayoutTransitionBarrier.sType =
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            LayoutTransitionBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            LayoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            LayoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            LayoutTransitionBarrier.srcQueueFamilyIndex =
                VK_QUEUE_FAMILY_IGNORED;
            LayoutTransitionBarrier.dstQueueFamilyIndex =
                VK_QUEUE_FAMILY_IGNORED;
            LayoutTransitionBarrier.image =
                Context.PresentImages[NextImageIndex];
            LayoutTransitionBarrier.subresourceRange.aspectMask =
                VK_IMAGE_ASPECT_COLOR_BIT;
            LayoutTransitionBarrier.subresourceRange.levelCount = 1;
            LayoutTransitionBarrier.subresourceRange.layerCount = 1;


            vkCmdPipelineBarrier(Context.SetupCommandBuffer,
                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 0, 0, 0, 0, 0, 1,
                                 &LayoutTransitionBarrier);

            vkEndCommandBuffer(Context.SetupCommandBuffer);

            // Submit setup commands to change present image layout.

            VkPipelineStageFlags WaitStageMask[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            };

            VkSubmitInfo SubmitInfo = {};
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.waitSemaphoreCount = 1;
            SubmitInfo.pWaitSemaphores = &IsCompleteSemaphore;
            SubmitInfo.pWaitDstStageMask = WaitStageMask;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = &Context.SetupCommandBuffer;

            Result = vkQueueSubmit(Context.PresentQueue,
                                   1,
                                   &SubmitInfo,
                                   SubmitFence);

            vkWaitForFences(Context.Device,
                            1,
                            &SubmitFence,
                            VK_TRUE,
                            UINT64_MAX);
            vkResetFences(Context.Device, 1, &SubmitFence);

            vkDestroySemaphore(Context.Device, IsCompleteSemaphore, 0);

            vkResetCommandBuffer(Context.SetupCommandBuffer, 0);

            IsModified[NextImageIndex] = 1;
            ModifiedCount++;
        }

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &Context.SwapChain;
        PresentInfo.pImageIndices = &NextImageIndex;
        vkQueuePresentKHR(Context.PresentQueue, &PresentInfo);
    }

    /** Create image views from modified presentation color images. */

    VkImageView PresentImageViews[ImageCount];

    for (unsigned int i = 0; i < ImageCount; i++)
    {
        PresentImagesViewCreateInfo.image = Context.PresentImages[i];

        // Create us an image view (finally)
        Result = vkCreateImageView(Context.Device,
                                   &PresentImagesViewCreateInfo,
                                   0,
                                   &PresentImageViews[i]);

        Assert(Result == VK_SUCCESS, "Could not create image view.\n");
    }

    /** Create a depth image buffer. (The previously created image is a color
     * image buffer.) */

    VkImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = VK_FORMAT_D16_UNORM;
    // TODO[joe] Make this more explicit.
    ImageCreateInfo.extent = { Context.Width, Context.Height, 1 };
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    Result = vkCreateImage(Context.Device,
                           &ImageCreateInfo,
                           NULL,
                           &Context.DepthImage);

    Assert(Result == VK_SUCCESS, "Failed to create depth image.\n");

    /** Allocate and bind memory on physical device to the created depth
     * buffer. */

    VkMemoryRequirements MemoryRequirements = {};
    vkGetImageMemoryRequirements(Context.Device,
                                 Context.DepthImage,
                                 &MemoryRequirements);

    VkMemoryAllocateInfo ImageAllocateInfo = {};
    ImageAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    ImageAllocateInfo.allocationSize = MemoryRequirements.size;

    unsigned int MemoryTypeBits = MemoryRequirements.memoryTypeBits;
    VkMemoryPropertyFlags DesiredMemoryFlags =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    // NOTE[joe] 32 is the size of MemoryTypeBits, which we read 1b at a time.
    for (unsigned int i = 0; i < 32; i++)
    {
        VkMemoryType MemoryType = Context.MemoryProperties.memoryTypes[i];

        // NOTE[joe] If the least significant bit is set, proceed.
        if (MemoryTypeBits & 1) {
            if ((MemoryType.propertyFlags & DesiredMemoryFlags) ==
                DesiredMemoryFlags)
            {
                ImageAllocateInfo.memoryTypeIndex = i;
                break;
            }
        }

        MemoryTypeBits = MemoryTypeBits >> 1;
    }

    VkDeviceMemory ImageMemory = {};
    Result = vkAllocateMemory(Context.Device,
                              &ImageAllocateInfo,
                              0,
                              &ImageMemory);

    Assert(Result == VK_SUCCESS,
           "Failed to allocate device memory for depth image.\n");

    Result = vkBindImageMemory(Context.Device,
                               Context.DepthImage,
                               ImageMemory,
                               0);

    Assert(Result == VK_SUCCESS,
           "Failed to bind image memory for depth image.\n");

    /** Change the layout of our depth buffer image. */

    BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(Context.SetupCommandBuffer,
                         &BeginInfo);

    VkImageMemoryBarrier LayoutTransitionBarrier = {};
    LayoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    LayoutTransitionBarrier.srcAccessMask = 0;
    LayoutTransitionBarrier.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    LayoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    LayoutTransitionBarrier.newLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    LayoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    LayoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    LayoutTransitionBarrier.image = Context.DepthImage;
    LayoutTransitionBarrier.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT;
    LayoutTransitionBarrier.subresourceRange.levelCount = 1;
    LayoutTransitionBarrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(Context.SetupCommandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0, 0, 0, 0, 0, 1,
                         &LayoutTransitionBarrier);

    vkEndCommandBuffer(Context.SetupCommandBuffer);

    VkPipelineStageFlags WaitStageMask[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.pWaitDstStageMask = WaitStageMask;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &Context.SetupCommandBuffer;

    Result = vkQueueSubmit(Context.PresentQueue, 1, &SubmitInfo, SubmitFence);

    // TODO[joe] Do something with Result? Abort?

    vkWaitForFences(Context.Device, 1, &SubmitFence, VK_TRUE, UINT64_MAX);
    vkResetFences(Context.Device, 1, &SubmitFence);
    vkResetCommandBuffer(Context.SetupCommandBuffer, 0);

    /** Create the image view for our depth buffer. */

    VkImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image = Context.DepthImage;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = ImageCreateInfo.format;
    ImageViewCreateInfo.components = {
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY
    };
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    Result = vkCreateImageView(Context.Device,
                               &ImageViewCreateInfo,
                               0,
                               &Context.DepthImageView);

    Assert(Result == VK_SUCCESS, "Failed to create depth image view.\n");

    /** Create attachments for render pass. */

    VkAttachmentDescription PassAttachments[2] = {};

    PassAttachments[0].format = ColorFormat;
    PassAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    PassAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    PassAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    PassAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    PassAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    PassAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    PassAttachments[1].format = VK_FORMAT_D16_UNORM;
    PassAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    PassAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    PassAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    PassAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    PassAttachments[1].initialLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    PassAttachments[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference ColorAttachmentReference = {};
    ColorAttachmentReference.attachment = 0;
    ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference DepthAttachmentReference = {};
    DepthAttachmentReference.attachment = 1;
    DepthAttachmentReference.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    /** Create render pass and accompanying subpass. */

    VkSubpassDescription Subpass = {};
    // TODO[joe] Does this have a sType member?
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentReference;
    Subpass.pDepthStencilAttachment = &DepthAttachmentReference;

    VkRenderPassCreateInfo RenderPassCreateInfo = {};
    RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassCreateInfo.attachmentCount = 2;
    RenderPassCreateInfo.pAttachments = PassAttachments;
    RenderPassCreateInfo.subpassCount = 1;
    RenderPassCreateInfo.pSubpasses = &Subpass;

    Result = vkCreateRenderPass(Context.Device,
                                &RenderPassCreateInfo,
                                0,
                                &Context.RenderPass);

    Assert(Result == VK_SUCCESS, "Failed to create render pass.\n");

    /** Create framebuffers. */

    VkImageView FramebufferAttachments[2];
    FramebufferAttachments[1] = Context.DepthImageView;

    VkFramebufferCreateInfo FramebufferCreateInfo = {};
    FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferCreateInfo.renderPass = Context.RenderPass;
    FramebufferCreateInfo.attachmentCount = 2;
    FramebufferCreateInfo.pAttachments = FramebufferAttachments;
    FramebufferCreateInfo.width = Context.Width;
    FramebufferCreateInfo.height = Context.Height;
    FramebufferCreateInfo.layers = 1;

    VkFramebuffer Framebuffers[ImageCount];
    Context.Framebuffers = Framebuffers;

    Assert(Result == VK_SUCCESS, "Failed to create framebuffer.\n");

    /** Create a vertex buffer for a triangle mesh. */

    VkBufferCreateInfo VertexInputBufferInfo = {};
    VertexInputBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // NOTE[joe] Allocate enough memory for a triangle.
    VertexInputBufferInfo.size = sizeof(vertex) * 3;
    VertexInputBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VertexInputBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Result = vkCreateBuffer(Context.Device,
                            &VertexInputBufferInfo,
                            0,
                            &Context.VertexInputBuffer);

    Assert(Result == VK_SUCCESS, "Failed to create vertex input buffer.\n");

    /** Allocate memory on the rendering device for our vertex buffer.
     * TODO[joe] Make this a function? We've used the same code twice now...
     * TODO[joe] Do we want be allocating a vertex buffer for our context?
     * Or do we want to be doing it for every model individually? */

    VkMemoryRequirements VertexBufferMemoryRequirements = {};
    vkGetBufferMemoryRequirements(Context.Device,
                                  Context.VertexInputBuffer,
                                  &VertexBufferMemoryRequirements);

    VkMemoryAllocateInfo BufferAllocateInfo = {};
    BufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    BufferAllocateInfo.allocationSize = VertexBufferMemoryRequirements.size;

    unsigned int VertexMemoryTypeBits =
        VertexBufferMemoryRequirements.memoryTypeBits;
    VkMemoryPropertyFlags VertexDesiredMemoryFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

    // NOTE[joe] 32 is the size of VertexMemoryTypeBits, whose bits we index.
    for (unsigned int i = 0; i < 32; ++i)
    {
        VkMemoryType MemoryType = Context.MemoryProperties.memoryTypes[i];

        if (VertexMemoryTypeBits & 1)
        {
            if ((MemoryType.propertyFlags & VertexDesiredMemoryFlags) ==
                VertexDesiredMemoryFlags)
            {
                BufferAllocateInfo.memoryTypeIndex = i;
                break;
            }
        }

        VertexMemoryTypeBits = VertexMemoryTypeBits >> 1;
    }

    VkDeviceMemory VertexBufferMemory;

    Result = vkAllocateMemory(Context.Device,
                              &BufferAllocateInfo,
                              0,
                              &VertexBufferMemory);

    Assert(Result == VK_SUCCESS, "Failed to allocate vertex buffer memory.\n");

    /** Map our vertex buffer into host memory, set vertex values, then unmap
     * memory to release it back to the graphics device. */

    void *Mapped;
    Result = vkMapMemory(Context.Device,
                         VertexBufferMemory,
                         0,
                         VK_WHOLE_SIZE,
                         0,
                         &Mapped);

    Assert(Result == VK_SUCCESS, "Failed to map vertex buffer memory.\n");

    vertex *Triangle = (vertex *)Mapped;
    Triangle[0] = { -1.0f, -1.0f, 0, 1.0f };
    Triangle[1] = {  1.0f, -1.0f, 0, 1.0f };
    Triangle[2] = {  0.0f,  1.0f, 0, 1.0f };

    vkUnmapMemory(Context.Device, VertexBufferMemory);

    Result = vkBindBufferMemory(Context.Device,
                                Context.VertexInputBuffer,
                                VertexBufferMemory,
                                0);

    Assert(Result == VK_SUCCESS, "Failed to bind vertex buffer memory.\n");

    /** Load shaders.
     * TODO[joe] This should definitely be somewhere else. */

    // TODO[joe] Figure out how to better get the shader path.
    VkShaderModule VertexShader = PlatformLoadShader(Context,
                                                     "../data/spirv/vert.spv");

    VkShaderModule FragShader = PlatformLoadShader(Context,
                                                   "../data/spirv/frag.spv");

    return Context;
}
