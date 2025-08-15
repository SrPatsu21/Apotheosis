#pragma once

#include "CoreVulkan.hpp"
#include "CoreVulkan.hpp"
#include "swapchain&framebuffer/SwapchainManager.hpp"
#include "graphics_pipeline/RenderPass.hpp"
#include "graphics_pipeline/DescriptorManager.hpp"
#include "camera/CameraBufferManager.hpp"
#include "graphics_pipeline/GraphicsPipeline.hpp"
#include "swapchain&framebuffer/DepthBufferManager.hpp"
#include "swapchain&framebuffer/FramebufferManager.hpp"
#include "mash/VertexManager.hpp"
#include "mash/IndexManager.hpp"
#include "swapchain&framebuffer/CommandManager.hpp"
#include "camera/UniformBufferObject.hpp"

// #include <stdexcept>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <cstring>

class Render {
public:
    Render();
    int run();

    ~Render();
private:

    //screen width in px
    uint32_t width = 800;

    // screen height in px
    uint32_t height = 600;

    GLFWwindow* window;
    SwapchainManager* swapchainManager;
    RenderPass* renderPass;
    CameraBufferManager* cameraBufferManager;
    DescriptorManager* descriptorManager;
    GraphicsPipeline* graphicsPipeline;
    DepthBufferManager* depthBufferManager;
    FramebufferManager* framebufferManager;
    VertexManager* vertexManager;
    IndexManager* indexManager;
    CommandManager* commandManager;
    /**
     * @brief Semaphore that signals when an image from the swapchain is ready for rendering.
     *
     * This semaphore is signaled by the presentation engine once an image has been acquired
     * from the swapchain and is ready to be rendered to. The graphics queue should wait on
     * this semaphore before starting to record or submit rendering commands.
     */
    VkSemaphore imageAvailableSemaphore;

    /**
     * @brief Semaphore that signals when rendering has finished.
     *
     * This semaphore is signaled once all rendering commands for the current frame have completed.
     * The presentation queue will wait on this before presenting the rendered image to the screen.
     */
    VkSemaphore renderFinishedSemaphore;

    /**
     * @brief Fence that signals when the GPU has finished processing the current frame.
     *
     * This fence is used to synchronize the CPU and GPU. The CPU waits on it to ensure that the GPU
     * has finished executing all commands for the previous frame, preventing resources from being
     * overwritten or reused prematurely.
     */
    VkFence inFlightFence;

    void initWindow();
    void initVulkan();
    void drawFrame();
    void cleanup();

    /**
     * @brief Creates the Vulkan synchronization objects used per-frame.
     *
     * This function initializes the semaphores and fence needed to coordinate
     * rendering and presentation in a single frame:
     *
     * - `imageAvailableSemaphore`: Signaled when an image from the swapchain becomes available
     *   for rendering. It is waited on before recording drawing commands.
     *
     * - `renderFinishedSemaphore`: Signaled when rendering of the current frame finishes.
     *   It is waited on before presenting the rendered image to the screen.
     *
     * - `inFlightFence`: A fence that signals when the GPU is done processing commands
     *   for the current frame, allowing the CPU to safely reuse resources.
     *   This fence is initialized in a signaled state so the first frame can proceed without blocking.
     *
     * Throws a std::runtime_error if any Vulkan call to create these synchronization
     * objects fails.
     *
     * @note The objects are created for the current frame. In a multi-frame setup,
     * you'd create multiple sets of these per frame in flight.
     */
    void createSyncObjects();
};