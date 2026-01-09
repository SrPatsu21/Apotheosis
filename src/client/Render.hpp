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

class Render {
public:
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    bool framebufferResized = false;

    const glm::vec4 color = {1.0f, 1.0f, 0.0f, 1.0f};
    const std::vector<Vertex> VERTICES = {
        // BASE
        Vertex({-0.5f, 0.0f, -0.5f}, color, {0.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f, -0.5f}, color, {1.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f,  0.5f}, color, {1.0f, 1.0f}),
        Vertex({-0.5f, 0.0f,  0.5f}, color, {0.0f, 1.0f}),

        // Frontal Face
        Vertex({-0.5f, 0.0f, -0.5f}, color, {0.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f, -0.5f}, color, {1.0f, 0.0f}),
        Vertex({ 0.0f, 0.8f,  0.0f}, color, {0.5f, 1.0f}),

        // Right
        Vertex({ 0.5f, 0.0f, -0.5f}, color, {0.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f,  0.5f}, color, {1.0f, 0.0f}),
        Vertex({ 0.0f, 0.8f,  0.0f}, color, {0.5f, 1.0f}),

        // Back
        Vertex({ 0.5f, 0.0f,  0.5f}, color, {0.0f, 0.0f}),
        Vertex({-0.5f, 0.0f,  0.5f}, color, {1.0f, 0.0f}),
        Vertex({ 0.0f, 0.8f,  0.0f}, color, {0.5f, 1.0f}),

        // Left
        Vertex({-0.5f, 0.0f,  0.5f}, color, {0.0f, 0.0f}),
        Vertex({-0.5f, 0.0f, -0.5f}, color, {1.0f, 0.0f}),
        Vertex({ 0.0f, 0.8f,  0.0f}, color, {0.5f, 1.0f}),
    };
    const std::vector<uint32_t> INDICES = {
        // BASE
        0, 1, 2,
        2, 3, 0,

        // FACES
        4, 5, 6,     // frente
        7, 8, 9,     // direita
        10, 11, 12,  // trás
        13, 14, 15   // esquerda
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
    TextureImage* textureImage;
    MeshLoader* meshLoader;

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