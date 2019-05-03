/**
 * @file render.h
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-04-23
 *
 * This file contains all the definitions we need for doing rendering in our
 * game.
 */

#ifndef _RENDER_H_
#define _RENDER_H_

// TODO[joe] Downsize this so that we're not carrying around all this bloat.
typedef struct {
    unsigned int    Width;
    unsigned int    Height;
    VkInstance      Instance;
    VkDevice        Device;
    VkQueue         PresentQueue;
    VkCommandBuffer SetupCommandBuffer;
    VkCommandBuffer DrawCommandBuffer;
    VkSwapchainKHR  SwapChain;
    VkImage*        PresentImages;
    VkImage         DepthImage;
    VkImageView     DepthImageView;
    VkRenderPass    RenderPass;
    VkFramebuffer*  Framebuffers;
    // TODO[joe] Do we want to keep the vertex buffer here?
    // Or would it be more prudent to keep a vertex buffer separately for each
    // model we have?
    VkBuffer        VertexInputBuffer;
    VkPipeline      Pipeline;
    VkPipelineLayout                 PipelineLayout;
    VkDebugReportCallbackEXT         Callback;
    VkSurfaceKHR                     Surface;
    VkPhysicalDevice                 PhysicalDevice;
    VkPhysicalDeviceProperties       PhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    unsigned int                     PresentQueueIndex;
} vulkan_context;

typedef struct {
    float x, y, z, w;
} vertex;

#endif
