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
#include "swapchain&framebuffer/CommandManager.hpp"
#include "camera/UniformBufferObject.hpp"

class Render {
public:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    bool framebufferResized = false;

    const std::vector<Vertex> VERTICES = {
        Vertex({-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}), // 0
        Vertex({ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}), // 1
        Vertex({ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f, 0.0f}), // 2
        Vertex({-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}), // 3

        Vertex({ 0.0f, 0.8f,  0.0f}, {1.0f, 0.0f, 1.0f, 1.0f})  // 4
    };
    const std::vector<uint16_t> INDICES = {
        0, 1, 2,
        0, 2, 3,

        0, 4, 1,
        1, 4, 2,
        2, 4, 3,
        3, 4, 0
    };

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
    UI* ui;

    RenderPass* renderPass;
    CameraBufferManager* cameraBufferManager;
    DescriptorManager* descriptorManager;
    GraphicsPipeline* graphicsPipeline;
    DepthBufferManager* depthBufferManager;
    FramebufferManager* framebufferManager;
    VertexBufferManager* vertexBufferManager;
    IndexBufferManager* indexBufferManager;
    CommandManager* commandManager;

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