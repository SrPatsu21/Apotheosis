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

    uint32_t currentFrame = 0;

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

    std::vector<VkSemaphore> imageAvailableSemaphores;

    std::vector<VkSemaphore> renderFinishedSemaphores;

    std::vector<VkFence> inFlightFences;

    void initWindow();
    void initVulkan();
    void drawFrame();
    void cleanup();

    void createSyncObjects(std::vector<VkFramebuffer> swapchainFramebuffers);
};