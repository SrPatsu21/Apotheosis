#include "Render.hpp"

Render::Render(){};

int Render::run(){
    //GLFW things
    initWindow();

    // Basic all vulkan setup
    initVulkan();

    // The UI
    initImGui();

    //main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ui new frame
        this->ui->newFrame();
        this->ui->build();

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // screen config
    this->window = glfwCreateWindow(this->width, this->height, "ProjectD", nullptr, nullptr);
    glfwSetWindowUserPointer(this->window, this);
    glfwSetFramebufferSizeCallback(this->window, framebufferResizeCallback);
};

void Render::initImGui(){
    this->ui = new UI;
    this->ui->init(this->window, this->renderPass->get(), this->swapchainManager->getImages().size());
}

void Render::initVulkan(){
    BufferManager bufferManager = BufferManager();
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
    this->cameraBufferManager = new CameraBufferManager(&bufferManager, Render::MAX_FRAMES_IN_FLIGHT);

    //Create DepthResources
    this->depthBufferManager = new DepthBufferManager(this->swapchainManager->getExtent());

    //Create framebuffers
    this->framebufferManager = new FramebufferManager(this->renderPass->get(), this->swapchainManager, this->depthBufferManager);

    // create semaphore and fence
    createSyncObjects();
    initImagesInFlight(this->swapchainManager->getImages().size());

    // Create command
    this->commandManager = new CommandManager(CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily.value(), this->framebufferManager->getFramebuffers());

    textureImage = new TextureImage("./textures/bf.png", &bufferManager, commandManager->getCommandPool());

    // Create descript
    this->descriptorManager = new DescriptorManager(this->cameraBufferManager, textureImage, Render::MAX_FRAMES_IN_FLIGHT);

    // Create graphics pipeline
    this->graphicsPipeline = new GraphicsPipeline(this->swapchainManager->getExtent(), this->renderPass->get(), this->descriptorManager->getLayout());

    // VkPhysicalDeviceProperties deviceProperties;
    // vkGetPhysicalDeviceProperties(CoreVulkan::getPhysicalDevice(), &deviceProperties);

    // std::cout << "Push Constant Max Size: " << deviceProperties.limits.maxPushConstantsSize << " bytes\n";


    // Create vertex buffer
    this->vertexBufferManager = new VertexBufferManager(Render::VERTICES, this->commandManager->getCommandPool());
    this->indexBufferManager = new IndexBufferManager(Render::INDICES, this->commandManager->getCommandPool());
};

void Render::drawFrame(){
    float time = glfwGetTime();

    // Wait for this frame to be free
    vkWaitForFences(CoreVulkan::getDevice(), 1, &this->inFlightFences[this->currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult next_img_result = vkAcquireNextImageKHR(CoreVulkan::getDevice(), this->swapchainManager->getSwapchain(),
            UINT64_MAX, this->imageAvailableSemaphores[this->currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (next_img_result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (next_img_result != VK_SUCCESS && next_img_result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // If this swapchain image is already in flight, wait for the fence that owns it
    if (this->imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(CoreVulkan::getDevice(), 1, &this->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark this image as now owned by the current frame's fence
    this->imagesInFlight[imageIndex] = this->inFlightFences[this->currentFrame];

    // Reset the fence for the current frame
    vkResetFences(CoreVulkan::getDevice(), 1, &this->inFlightFences[this->currentFrame]);

    // Reset + record only the command buffer for this swapchain image
    VkCommandBuffer cmd = this->commandManager->getCommandBuffers()[imageIndex];
    vkResetCommandBuffer(cmd, 0);
    this->commandManager->recordCommandBuffer(imageIndex, this->renderPass->get(), this->graphicsPipeline,
            this->framebufferManager->getFramebuffers(), this->swapchainManager->getExtent(),
            this->vertexBufferManager->getVertexBuffer(), this->indexBufferManager->getIndexBuffer(), Render::INDICES,
            this->descriptorManager->getSets()[currentFrame], [this](VkCommandBuffer cmd) { this->ui->render(cmd); });

    // Update UBOs for this frame
    this->cameraBufferManager->updateUniformBuffer(this->swapchainManager, currentFrame, time);

    // --- Submit work ---
    VkSemaphore waitSemaphores[] = { this->imageAvailableSemaphores[this->currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { this->renderFinishedSemaphores[this->currentFrame] };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(CoreVulkan::getGraphicsQueue(), 1, &submitInfo, this->inFlightFences[this->currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Present image
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { this->swapchainManager->getSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult presentResult = vkQueuePresentKHR(CoreVulkan::getPresentQueue(), &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR  || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    } else if (presentResult != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    // Advance to next frame slot
    this->currentFrame = (this->currentFrame + 1) % Render::MAX_FRAMES_IN_FLIGHT;
}

void Render::cleanup(){
    // 1) Stop the GPU first so nothing is in flight.
    if (CoreVulkan::getDevice() != VK_NULL_HANDLE) {
        vkQueueWaitIdle(CoreVulkan::getPresentQueue());
        vkDeviceWaitIdle(CoreVulkan::getDevice());
    }

    // 2) Per-frame sync primitives.
    for (VkSemaphore s : this->renderFinishedSemaphores)
        if (s != VK_NULL_HANDLE) vkDestroySemaphore(CoreVulkan::getDevice(), s, nullptr);
    this->renderFinishedSemaphores.clear(); this->renderFinishedSemaphores.shrink_to_fit();

    for (VkSemaphore s : this->imageAvailableSemaphores)
        if (s != VK_NULL_HANDLE) vkDestroySemaphore(CoreVulkan::getDevice(), s, nullptr);
    this->imageAvailableSemaphores.clear(); this->imageAvailableSemaphores.shrink_to_fit();

    for (VkFence f : this->inFlightFences)
        if (f != VK_NULL_HANDLE) vkDestroyFence(CoreVulkan::getDevice(), f, nullptr);
    this->inFlightFences.clear(); this->inFlightFences.shrink_to_fit();

    // 3) Managers: destroy in strict reverse-creation order.
    //    (Everything that depends on the swapchain must go BEFORE swapchain.)
    //    Delete pointers and null them to avoid accidental double free later.
    if (this->commandManager){ delete this->commandManager; this->commandManager = nullptr; }
    if (this->vertexBufferManager){ delete this->vertexBufferManager; this->vertexBufferManager = nullptr; }
    if (this->indexBufferManager){ delete this->indexBufferManager; this->indexBufferManager = nullptr; }
    if (this->framebufferManager){ delete this->framebufferManager; this->framebufferManager = nullptr; }
    if (this->depthBufferManager){ delete this->depthBufferManager; this->depthBufferManager = nullptr; }
    if (this->graphicsPipeline){ delete this->graphicsPipeline; this->graphicsPipeline = nullptr; }
    if (this->descriptorManager){ delete this->descriptorManager; this->descriptorManager = nullptr; }
    if (this->cameraBufferManager){ delete this->cameraBufferManager; this->cameraBufferManager = nullptr; }
    if (this->textureImage){ delete this->textureImage; this->textureImage = nullptr; }
    if (this->ui) { this->ui->cleanup(); delete this->ui; this->ui = nullptr; }
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
    this->imageAvailableSemaphores.resize(Render::MAX_FRAMES_IN_FLIGHT);
    this->renderFinishedSemaphores.resize(Render::MAX_FRAMES_IN_FLIGHT);
    this->inFlightFences.resize(Render::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{ };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{ };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start signaled so first frame doesn't block

    for (uint32_t i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i) {
        if (vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(CoreVulkan::getDevice(), &fenceInfo, nullptr, &this->inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create per-frame sync objects!");
        }
    }
}

void Render::initImagesInFlight(uint32_t swapchainImageCount) {
    this->imagesInFlight.assign(swapchainImageCount, VK_NULL_HANDLE);
}

void Render::cleanupSwapChain() {

    // Command Buffers
    vkFreeCommandBuffers(
        CoreVulkan::getDevice(),
        commandManager->getCommandPool(),
        static_cast<uint32_t>(commandManager->getCommandBuffers().size()),
        commandManager->getCommandBuffers().data()
    );

    if (this->framebufferManager){ delete this->framebufferManager; this->framebufferManager = nullptr; }
    if (this->depthBufferManager){ delete this->depthBufferManager; this->depthBufferManager = nullptr; }
    if (this->graphicsPipeline){ delete this->graphicsPipeline; this->graphicsPipeline = nullptr; }
    if (this->ui) { vkDeviceWaitIdle(CoreVulkan::getDevice()); ImGui_ImplVulkan_Shutdown(); }
    if (this->renderPass){ delete this->renderPass; this->renderPass = nullptr; }

    // Swapchain itself
    this->swapchainManager->safeDestroySwapchain();
}

void Render::recreateSwapChain() {
    vkDeviceWaitIdle(CoreVulkan::getDevice());

    int width = 0, height = 0;
    glfwGetFramebufferSize(this->window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(this->window, &width, &height);
        glfwWaitEvents();
    }

    // 1. Destroy old swapchain + dependents
    cleanupSwapChain();

    // 2. Recreate swapchain
    this->swapchainManager->recreate(CoreVulkan::getSurface(), this->window, CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily.value());

    // 3. Recreate render pass (might depend on swapchain format)
    this->renderPass = new RenderPass(this->swapchainManager->getImageFormat());

    // 4. Recreate pipeline (depends on render pass + extent)
    this->graphicsPipeline = new GraphicsPipeline(this->swapchainManager->getExtent(), this->renderPass->get(), this->descriptorManager->getLayout());

    // 5. Recreate depth buffer
    this->depthBufferManager = new DepthBufferManager(this->swapchainManager->getExtent());

    // 6. Recreate framebuffers
    this->framebufferManager = new FramebufferManager(this->renderPass->get(), this->swapchainManager, this->depthBufferManager);

    // 7. Recreate command buffers
    this->commandManager->allocateCommandbuffers(this->framebufferManager->getFramebuffers());

    // 8. Resize imagesInFlight vector to match new swapchain image count
    initImagesInFlight(this->swapchainManager->getImages().size());

    // 9. Recreate ImGui resources
    this->ui->initSwapchainResources(this->renderPass->get(), this->swapchainManager->getImages().size());
}

void Render::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Render*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

Render::~Render() {
    cleanup();
}