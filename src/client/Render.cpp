#include "Render.hpp"


Render::Render(){};

int Render::run(){
    //GLFW things
    initWindow();

    // Basic all vulkan setup
    initVulkan();

    //main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    //free memory (secure)
    cleanup();
    return 0;
};

void Render::initWindow(){
    if (!glfwInit()) {
        throw std::runtime_error("Failed to init GLFW");
    }

    // no OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // block resize
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // screen config
    this->window = glfwCreateWindow(this->width, this->height, "ProjectD", nullptr, nullptr);
};

void Render::initVulkan(){
    //TODO better description
    //* Core Vulkan
    //Create Vulkan instance
    CoreVulkan::init();

    // create surface
    CoreVulkan::createSurface(this->window);

    // Pick a physical GPU
    CoreVulkan::pickPhysicalDevice();

    // Create logical device
    CoreVulkan::createLogicalDevice();

    // find the Depth Format of GPU
    CoreVulkan::findDepthFormat();

    // Create swapchain
    this->swapchainManager = new SwapchainManager(CoreVulkan::getSurface(), CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily.value(), this->window);

    // Create render pass
    this->renderPass = new RenderPass(this->swapchainManager->getImageFormat());

    // Create camera buff with uniformBuffer
    this->cameraBufferManager = new CameraBufferManager();

    // Create descript
    this->descriptorManager = new DescriptorManager(this->cameraBufferManager);

    // Create graphics pipeline
    this->graphicsPipeline = new GraphicsPipeline(this->swapchainManager->getExtent(), this->renderPass->get(), this->descriptorManager->getLayout());

    //Create DepthResources
    this->depthBufferManager = new DepthBufferManager(this->swapchainManager->getExtent());

    //Create framebuffers
    this->framebufferManager = new FramebufferManager(this->renderPass->get(), this->swapchainManager, this->depthBufferManager);

    // create semaphore and fence
    createSyncObjects();

    // Create vertex buffer
    const std::vector<Vertex> vertices = {
        Vertex({-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}), // 0
        Vertex({ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}), // 1
        Vertex({ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}), // 2
        Vertex({-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 0.0f}), // 3

        Vertex({ 0.0f, 0.8f,  0.0f}, {1.0f, 0.0f, 1.0f})  // 4
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2,
        0, 2, 3,

        0, 4, 1,
        1, 4, 2,
        2, 4, 3,
        3, 4, 0
    };

    this->vertexManager = new VertexManager(vertices);
    this->indexManager = new IndexManager(indices);

    // Create command
    this->commandManager = new CommandManager(CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily.value(),
        this->renderPass->get(), this->graphicsPipeline, this->framebufferManager->getFramebuffers(),
        this->swapchainManager->getExtent(), vertexManager->getVertexBuffer(), indexManager->getIndexBuffer(),
        indices, this->descriptorManager->getSet());

    // VkPhysicalDeviceProperties deviceProperties;
    // vkGetPhysicalDeviceProperties(CoreVulkan::getPhysicalDevice(), &deviceProperties);

    // std::cout << "Push Constant Max Size: " << deviceProperties.limits.maxPushConstantsSize << " bytes\n";

};

void Render::drawFrame(){
    float time = glfwGetTime();

    cameraBufferManager->updateUniformBuffer(swapchainManager, time);

    vkWaitForFences(CoreVulkan::getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(CoreVulkan::getDevice(), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(CoreVulkan::getDevice(), swapchainManager->getSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandManager->getCommandBuffers()[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(CoreVulkan::getPresentQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchainManager->getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult presentResult = vkQueuePresentKHR(CoreVulkan::getPresentQueue(), &presentInfo);
    if (presentResult != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    // std::cout << "Recording command buffer #" << imageIndex << "\n";
    // std::cout << "Binding pipeline, vertex/index buffers...\n";
    // std::cout << "Issuing draw command with " << indices.size() << " indices.\n";
};

void Render::cleanup(){
    // 1) Stop the GPU first so nothing is in flight.
    if (CoreVulkan::getDevice() != VK_NULL_HANDLE) {
        vkQueueWaitIdle(CoreVulkan::getPresentQueue());
        vkDeviceWaitIdle(CoreVulkan::getDevice());
    }

    // 2) Per-frame sync primitives.
    if (this->renderFinishedSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(CoreVulkan::getDevice(), this->renderFinishedSemaphore, nullptr);
        this->renderFinishedSemaphore = VK_NULL_HANDLE;
    }
    if (this->imageAvailableSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(CoreVulkan::getDevice(), this->imageAvailableSemaphore, nullptr);
        this->imageAvailableSemaphore = VK_NULL_HANDLE;
    }
    if (this->inFlightFence != VK_NULL_HANDLE) {
        vkDestroyFence(CoreVulkan::getDevice(), this->inFlightFence, nullptr);
        this->inFlightFence = VK_NULL_HANDLE;
    }

    // 3) Managers: destroy in strict reverse-creation order.
    //    (Everything that depends on the swapchain must go BEFORE swapchain.)
    //    Delete pointers and null them to avoid accidental double free later.
    if (this->commandManager){ delete this->commandManager; this->commandManager = nullptr; }
    if (this->vertexManager){ delete this->vertexManager; this->vertexManager = nullptr; }
    if (this->indexManager){ delete this->indexManager; this->indexManager = nullptr; }
    if (this->framebufferManager){ delete this->framebufferManager; this->framebufferManager = nullptr; }
    if (this->depthBufferManager){ delete this->depthBufferManager; this->depthBufferManager = nullptr; }
    if (this->graphicsPipeline){ delete this->graphicsPipeline; this->graphicsPipeline = nullptr; }
    if (this->descriptorManager){ delete this->descriptorManager; this->descriptorManager = nullptr; }
    if (this->cameraBufferManager){ delete this->cameraBufferManager; this->cameraBufferManager = nullptr; }
    if (this->renderPass){ delete this->renderPass; this->renderPass = nullptr; }

    // Swapchain and resources that own VkSwapchainKHR should be last among managers.
    if (this->swapchainManager)
    {
        delete this->swapchainManager;
        this->swapchainManager = nullptr;
    }

    // 4) Vulkan core teardown (device, surface, instance, debug messenger, etc.).
    //    Ensure CoreVulkan::destroy() destroys in the order:
    //    - vkDeviceWaitIdle (if not already) -> vkDestroyDevice
    //    - vkDestroySurfaceKHR
    //    - vkDestroyInstance
    //    - Destroy debug messenger (if you use one) before vkDestroyInstance.
    CoreVulkan::destroy();

    // 5) Windowing. Destroy the window AFTER you've destroyed the VkSurfaceKHR.
    if (this->window) {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
    glfwTerminate();
}

void Render::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(CoreVulkan::getDevice(), &fenceInfo, nullptr, &this->inFlightFence) != VK_SUCCESS)
        {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

Render::~Render() {
    cleanup();
}