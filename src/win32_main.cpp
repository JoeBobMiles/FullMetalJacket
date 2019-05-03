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

// Include engine headers.
#include "render.h"
#include "platform.h"

// Include Win32 specific vulkan setup.
#include "win32_vulkan_helper.cpp"

// NOTE[joe] Temporary globals
static int ApplicationQuit;
static vulkan_context Context;

/** Render black to the screen instead of white. */
static
void GameRender(vulkan_context Context)
{
    VkSemaphore PresentCompletedSemaphore, RenderingCompletedSemaphore;

    VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCreateSemaphore(Context.Device,
                      &SemaphoreCreateInfo,
                      0,
                      &PresentCompletedSemaphore);
    vkCreateSemaphore(Context.Device,
                      &SemaphoreCreateInfo,
                      0,
                      &RenderingCompletedSemaphore);

    unsigned int NextImageIndex;
    vkAcquireNextImageKHR(Context.Device,
                          Context.SwapChain,
                          UINT64_MAX,
                          PresentCompletedSemaphore,
                          VK_NULL_HANDLE,
                          &NextImageIndex);

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(Context.DrawCommandBuffer, &BeginInfo);

    /** Change image memory from the present layout to an attachment layout. */

    VkImageMemoryBarrier LayoutTransitionBarrier = {};
    LayoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    LayoutTransitionBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    LayoutTransitionBarrier.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    LayoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    LayoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    LayoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    LayoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    LayoutTransitionBarrier.image = Context.PresentImages[NextImageIndex];
    LayoutTransitionBarrier.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    LayoutTransitionBarrier.subresourceRange.levelCount = 1;
    LayoutTransitionBarrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(Context.DrawCommandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0, // Dependency flags
                         0, 0, // Memory barrier count and pointer
                         0, 0, // Buffer memory barrier count and pointer
                         1, &LayoutTransitionBarrier);

    /** Setup and initialize the render pass. */

    VkClearValue ClearValues[] = {
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f }
    };

    VkRenderPassBeginInfo RenderPassBeginInfo = {};
    RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassBeginInfo.renderPass = Context.RenderPass;
    RenderPassBeginInfo.framebuffer = Context.Framebuffers[NextImageIndex];
    RenderPassBeginInfo.renderArea = { 0, 0, Context.Width, Context.Height};
    RenderPassBeginInfo.clearValueCount = 2;
    RenderPassBeginInfo.pClearValues = ClearValues;

    vkCmdBeginRenderPass(Context.DrawCommandBuffer,
                         &RenderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(Context.DrawCommandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      Context.Pipeline);

    /* NOTE[joe] Uncomment when using dynamic state.
    VkViewport Viewport = { 0, 0, Context.Width, Context.Height };
    vkCmdSetViewport(Context.DrawCommandBuffer, 0, 1, &Viewport);

    VkRect2D Scissor = { 0, 0, Context.Width, Context.Height };
    vkCmdSetScissor(Context.DrawCommandBuffer, 0, 1, &Scissor);
     */

    /** Add draw command to command buffer. */

    VkDeviceSize Offsets = {};
    vkCmdBindVertexBuffers(Context.DrawCommandBuffer,
                           0,
                           1,
                           &Context.VertexInputBuffer,
                           &Offsets);

    vkCmdDraw(Context.DrawCommandBuffer,
              3, // Vertex count.
              1, // Instance count (what is this?)
              0, // First vertex index.
              0); // First instance index (what is this?)

    vkCmdEndRenderPass(Context.DrawCommandBuffer);

    /** Convert image from attachment layout back to present layout. */

    VkImageMemoryBarrier PrePresentBarrier = {};
    PrePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    PrePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    PrePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    PrePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    PrePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    PrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    PrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    PrePresentBarrier.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    PrePresentBarrier.subresourceRange.levelCount = 1;
    PrePresentBarrier.subresourceRange.layerCount = 1;
    PrePresentBarrier.image = Context.PresentImages[NextImageIndex];

    vkCmdPipelineBarrier(Context.DrawCommandBuffer,
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, 0, 0, 0, 1,
                         &PrePresentBarrier);

    vkEndCommandBuffer(Context.DrawCommandBuffer);

    /** Submit our draw commands and present our image. */

    VkFence RenderFence;
    VkFenceCreateInfo FenceCreateInfo = {};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(Context.Device,
                  &FenceCreateInfo,
                  0,
                  &RenderFence);

    VkPipelineStageFlags WaitStageMask = {
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
    };

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &PresentCompletedSemaphore;
    SubmitInfo.pWaitDstStageMask = &WaitStageMask;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &Context.DrawCommandBuffer;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &RenderingCompletedSemaphore;

    vkQueueSubmit(Context.PresentQueue, 1, &SubmitInfo, RenderFence);

    vkWaitForFences(Context.Device, 1, &RenderFence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(Context.Device, RenderFence, 0);

    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &RenderingCompletedSemaphore;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &Context.SwapChain;
    PresentInfo.pImageIndices = &NextImageIndex;

    // Submits the contents of our presnt queue to be draw to the screen.
    vkQueuePresentKHR(Context.PresentQueue, &PresentInfo);

    vkDestroySemaphore(Context.Device, PresentCompletedSemaphore, 0);
    vkDestroySemaphore(Context.Device, RenderingCompletedSemaphore, 0);
}

#ifdef DEBUG
static
void Assert(bool Flag, const char* Message)
{
    if (!Flag)
    {
        OutputDebugStringA("ASSERT FAILED: ");
        OutputDebugStringA(Message);

        int *_ = 0; *_ = 1;
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

/** Loads a compiled shader from the given FilePath and creates a Vulkan shader
 * module from it. */
static inline
VkShaderModule PlatformLoadShader(vulkan_context Context,
                                  const char* FilePath)
{
    unsigned int CodeSize;
    char Code[PLATFORM_MAX_SHADER_SIZE];
    HANDLE FileHandle = 0;

    FileHandle = CreateFile(FilePath,
                            GENERIC_READ,
                            0,
                            0,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            0);

    // TODO[joe] Report shader name.
    // TODO[joe] Abort elsewhere?
#if DEBUG
    Assert(FileHandle != INVALID_HANDLE_VALUE, "Failed to open shader!");
#else
    if (FileHandle != INVALID_HANDLE_VALUE)
        Abort("Failed to open shader!");
#endif

    ReadFile(FileHandle,
             Code,
             PLATFORM_MAX_SHADER_SIZE,
             (LPDWORD) &CodeSize,
             0);

    CloseHandle(FileHandle);

    VkShaderModuleCreateInfo ShaderCreationInfo = {};
    ShaderCreationInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderCreationInfo.codeSize = CodeSize;
    ShaderCreationInfo.pCode = (unsigned int *)Code;

    VkShaderModule ShaderModule;

    VkResult Result = vkCreateShaderModule(Context.Device,
                                           &ShaderCreationInfo,
                                           0,
                                           &ShaderModule);

    Assert(Result == VK_SUCCESS, "Failed to create vertex shader module.");

    return ShaderModule;
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
            // TODO[joe] Refactor so Context is passed as pointer.
            // Levi abhores that we pass this massive struct by value.
            Context = win32_InitializeVulkan(Instance, Window);

            /** BEGIN Vulkan graphics pipeline creation. */

            /** Load shaders. */

            // TODO[joe] Figure out how to better get the shader path.
            VkShaderModule VertexShader =
                PlatformLoadShader(Context, "../data/spirv/vert.spv");

            VkShaderModule FragShader =
                PlatformLoadShader(Context, "../data/spirv/frag.spv");

            /** Create our graphics pipeline. */

            VkPipelineLayoutCreateInfo LayoutCreateInfo = {};
            LayoutCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            VkResult Result = vkCreatePipelineLayout(Context.Device,
                                                     &LayoutCreateInfo,
                                                     0,
                                                     &Context.PipelineLayout);

            Assert(Result == VK_SUCCESS, "Failed to create pipeline layout.\n");

            VkPipelineShaderStageCreateInfo ShaderStageCreateInfo[2] = {};

            // Vertex shader stage
            ShaderStageCreateInfo[0].sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            ShaderStageCreateInfo[0].module = VertexShader;
            // NOTE[joe] Name of shader entry point.
            ShaderStageCreateInfo[0].pName = "main";

            // Fragment shader stage
            ShaderStageCreateInfo[1].sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            ShaderStageCreateInfo[1].module = FragShader;
            // NOTE[joe] Name of shader entry point.
            ShaderStageCreateInfo[1].pName = "main";

            VkVertexInputBindingDescription VertexBindingDescription = {};
            VertexBindingDescription.stride = sizeof(vertex);
            VertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            VkVertexInputAttributeDescription VertexAttributeDescription = {};
            VertexAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;

            VkPipelineVertexInputStateCreateInfo
                VertexInputStateCreateInfo = {};

            VertexInputStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
            VertexInputStateCreateInfo.pVertexBindingDescriptions =
                &VertexBindingDescription;
            VertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
            VertexInputStateCreateInfo.pVertexAttributeDescriptions =
                &VertexAttributeDescription;

            VkPipelineInputAssemblyStateCreateInfo
                InputAssemblyStateCreateInfo = {};

            InputAssemblyStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            InputAssemblyStateCreateInfo.topology =
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

            /** Create viewport and clipping "scissors". */

            VkViewport Viewport = {};
            Viewport.width = Context.Width;
            Viewport.height = Context.Height;
            Viewport.maxDepth = 1;

            VkRect2D Scissors = {};
            Scissors.extent = { Context.Width, Context.Height };

            VkPipelineViewportStateCreateInfo ViewportState = {};
            ViewportState.sType =
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            ViewportState.viewportCount = 1;
            ViewportState.pViewports = &Viewport;
            ViewportState.scissorCount = 1;
            ViewportState.pScissors = &Scissors;

            /** Rasterization configuration. */

            VkPipelineRasterizationStateCreateInfo
                RasterizationStateCreateInfo = {};

            RasterizationStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
            RasterizationStateCreateInfo.frontFace =
                VK_FRONT_FACE_COUNTER_CLOCKWISE;
            RasterizationStateCreateInfo.lineWidth = 1;

            /** Sampling configuration. */
            VkPipelineMultisampleStateCreateInfo MultisampleStateCreatInfo = {};
            MultisampleStateCreatInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            MultisampleStateCreatInfo.rasterizationSamples =
                VK_SAMPLE_COUNT_1_BIT;

            /** Enable depth testing and disable stenciling. */

            VkStencilOpState NoOpStencilState = {};
            NoOpStencilState.failOp = VK_STENCIL_OP_KEEP;
            NoOpStencilState.passOp = VK_STENCIL_OP_KEEP;
            NoOpStencilState.depthFailOp = VK_STENCIL_OP_KEEP;
            NoOpStencilState.compareOp = VK_COMPARE_OP_ALWAYS;

            VkPipelineDepthStencilStateCreateInfo DepthStateCreateInfo = {};
            DepthStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            DepthStateCreateInfo.depthTestEnable = VK_TRUE;
            DepthStateCreateInfo.depthWriteEnable = VK_TRUE;
            DepthStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            DepthStateCreateInfo.front = NoOpStencilState;
            DepthStateCreateInfo.back = NoOpStencilState;

            /** Disable color blending. */

            VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
            ColorBlendAttachmentState.srcColorBlendFactor =
                VK_BLEND_FACTOR_SRC_COLOR;
            ColorBlendAttachmentState.dstColorBlendFactor =
                VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
            ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
            ColorBlendAttachmentState.colorWriteMask = 0xf;

            VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo = {};
            ColorBlendStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
            ColorBlendStateCreateInfo.attachmentCount = 1;
            ColorBlendStateCreateInfo.pAttachments = &ColorBlendAttachmentState;

            /* NOTE[joe] To make pipeline state (like viewport size) dynamic:

            VkDynamicState DynamicState[2] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            };

            VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
            DynamicStateCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            DynamicStateCreateInfo.dynamicStateCount = 2;
            DynamicStateCreateInfo.pDynamicStates = dynamicState;
             */

            /** Create the graphics pipeline */

            VkGraphicsPipelineCreateInfo PipelineCreateInfo = {};
            PipelineCreateInfo.sType =
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            PipelineCreateInfo.stageCount = 2;
            PipelineCreateInfo.pStages = ShaderStageCreateInfo;
            PipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
            PipelineCreateInfo.pInputAssemblyState =
                &InputAssemblyStateCreateInfo;
            PipelineCreateInfo.pViewportState = &ViewportState;
            PipelineCreateInfo.pRasterizationState =
                &RasterizationStateCreateInfo;
            PipelineCreateInfo.pMultisampleState = &MultisampleStateCreatInfo;
            PipelineCreateInfo.pDepthStencilState = &DepthStateCreateInfo;
            PipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
            // NOTE[joe] Uncomment for dynamically changing viewport.
            // PipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
            PipelineCreateInfo.layout = Context.PipelineLayout;
            PipelineCreateInfo.renderPass = Context.RenderPass;

            Result = vkCreateGraphicsPipelines(Context.Device,
                                               VK_NULL_HANDLE,
                                               1,
                                               &PipelineCreateInfo,
                                               0,
                                               &Context.Pipeline);

            Assert(Result == VK_SUCCESS,
                   "Failed to create graphics pipeline.");

            /** END Vulkan graphics pipeline creation. */

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
