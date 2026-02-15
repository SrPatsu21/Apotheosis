#pragma once

#include "CoreVulkan.hpp"
#include "CoreVulkan.hpp"
#include "ui/UI.hpp"
#include "swapchain&framebuffer/SwapchainManager.hpp"
#include "graphics_pipeline/RenderPass.hpp"
#include "graphics_pipeline/GlobalDescriptorManager.hpp"
#include "camera/CameraBufferManager.hpp"
#include "graphics_pipeline/GraphicsPipeline.hpp"
#include "swapchain&framebuffer/DepthBufferManager.hpp"
#include "swapchain&framebuffer/FramebufferManager.hpp"
//todo fix mash name :)
#include "swapchain&framebuffer/CommandManager.hpp"
#include "camera/UniformBufferGlobal.hpp"
#include "image/ImageColor.hpp"
#include "batch/material/MaterialDescriptorManager.hpp"
#include "batch/RenderBatchManager.hpp"
#include "batch/ResourceManager.hpp"
#include "batch/instance/RenderInstance.hpp"

class Render {
public:
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    bool framebufferResized = false;

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
    CoreVulkan* coreVulkan;
    SwapchainManager* swapchainManager;
    UI* ui;

    RenderPass* renderPass;
    CameraBufferManager* cameraBufferManager;
    GlobalDescriptorManager* globalDescriptorManager;
    MaterialDescriptorManager* materialDescriptorManager;
    GraphicsPipeline* graphicsPipeline;
    ImageColor* imageColor;
    DepthBufferManager* depthBufferManager;
    FramebufferManager* framebufferManager;
    CommandManager* commandManager;
    CameraBufferManager::ICameraProvider* iCameraProvider;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    RenderBatchManager* renderBatchManager;
    ResourceManager* resourceManager;
    RenderInstance* renderInstance;
    BufferManager* bufferManager;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    void initWindow();
    void initVulkan();
    void initImGui();
    void initInstances();
    void drawFrame();
    void cleanup();

    void createSyncObjects();
    void initImagesInFlight(uint32_t swapchainImageCount);

    void cleanupSwapChain();
    void recreateSwapChain();
};