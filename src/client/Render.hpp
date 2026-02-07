#pragma once

#include "CoreVulkan.hpp"
#include "CoreVulkan.hpp"
#include "ui/UI.hpp"
#include "swapchain&framebuffer/SwapchainManager.hpp"
#include "graphics_pipeline/RenderPass.hpp"
#include "graphics_pipeline/DescriptorManager.hpp"
#include "camera/CameraBufferManager.hpp"
#include "graphics_pipeline/GraphicsPipeline.hpp"
#include "swapchain&framebuffer/DepthBufferManager.hpp"
#include "swapchain&framebuffer/FramebufferManager.hpp"
#include "mash/VertexBufferManager.hpp"
#include "mash/IndexBufferManager.hpp"
#include "mash/BufferManager.hpp"
#include "mash/MeshLoader.hpp"
#include "swapchain&framebuffer/CommandManager.hpp"
#include "camera/UniformBufferObject.hpp"
#include "image/TextureImage.hpp"
#include "image/ImageColor.hpp"

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
    DescriptorManager* descriptorManager;
    GraphicsPipeline* graphicsPipeline;
    ImageColor* imageColor;
    DepthBufferManager* depthBufferManager;
    FramebufferManager* framebufferManager;
    VertexBufferManager* vertexBufferManager;
    IndexBufferManager* indexBufferManager;
    CommandManager* commandManager;
    TextureImage* textureImage;
    MeshLoader* meshLoader;
    CameraBufferManager::ICameraProvider* iCameraProvider;

    std::vector<VkSemaphore> imageAvailableSemaphores;

    std::vector<VkSemaphore> renderFinishedSemaphores;

    std::vector<VkFence> inFlightFences;

    std::vector<VkFence> imagesInFlight;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    void initWindow();
    void initVulkan();
    void initImGui();
    void drawFrame();
    void cleanup();

    void createSyncObjects();
    void initImagesInFlight(uint32_t swapchainImageCount);

    void cleanupSwapChain();
    void recreateSwapChain();
};