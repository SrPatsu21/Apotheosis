#include "Render.hpp"
#include "CoreVulkan.hpp"
#include "swapchain/SwapchainManager.hpp"
#include "pipeline/RenderPass.hpp"
#include "pipeline/DescriptorManager.hpp"
#include "camera/CameraBufferManager.hpp"
#include "ShaderLoader.hpp"
#include "pipeline/GraphicsPipeline.hpp"
#include "swapchain/DepthBufferManager.hpp"
#include "swapchain/FramebufferManager.hpp"
#include "mash/VertexManager.hpp"
#include "mash/IndexManager.hpp"
#include "swapchain/CommandManager.hpp"
#include "camera/UniformBufferObject.hpp"

#include <glm/gtc/matrix_transform.hpp>


Render::Render(){};

int Render::run(){

    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // no OpenGL
    GLFWwindow* window = glfwCreateWindow(800, 600, "ProjectD", nullptr, nullptr); // screen config

    //Create Vulkan instance
    uint32_t graphicsQueueFamily = 0;
    VkQueue graphicsQueue;
    CoreVulkan::init(graphicsQueueFamily, graphicsQueue);

    // Create surface
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(CoreVulkan::getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    // Create swapchain
    SwapchainManager* swapchainManager = new SwapchainManager(surface, graphicsQueueFamily, window);

    // Create render pass
    RenderPass* renderPass = new RenderPass(swapchainManager->getImageFormat());

    // Create camera buff with uniformBuffer
    CameraBufferManager* cameraBufferManager = new CameraBufferManager();

    // Load shaders
    ShaderLoader* shaderLoader = new ShaderLoader("shaders/vertex.glsl.spv", "shaders/fragment.glsl.spv");

    // Create descript
    DescriptorManager* descriptorManager = new DescriptorManager(cameraBufferManager);

    // Create graphics pipeline
    GraphicsPipeline* graphicsPipeline = new GraphicsPipeline(swapchainManager->getExtent(), renderPass->get(),
     shaderLoader->getVertModule(), shaderLoader->getFragModule(), descriptorManager->getLayout());

    //Create DepthResources
    DepthBufferManager* depthBufferManager = new DepthBufferManager(swapchainManager->getExtent());

    //Create framebuffers
    FramebufferManager* framebufferManager = new FramebufferManager(renderPass->get(), swapchainManager, depthBufferManager);

    // create semaphore and fence
    createSyncObjects();

    // Create vertex buffer
    const std::vector<Vertex> vertices = {
        // Base quadrada
        Vertex({-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}),
        Vertex({ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}),
        Vertex({-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 0.0f}),

        // Topo
        Vertex({0.0f, 0.8f, 0.0f}, {1.0f, 0.0f, 1.0f})
    };
    const std::vector<uint16_t> indices = {
        // Base (quadrado)
        0, 1, 2,
        2, 3, 0,

        // Lados (triângulos)
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
    VertexManager* pyramidVertex = new VertexManager(vertices);
    IndexManager* pyramidIndex = new IndexManager(indices);

    // Create command
    CommandManager* commandManager = new CommandManager(graphicsQueueFamily, renderPass->get(), graphicsPipeline->getPipeline(),
    graphicsPipeline->getLayout(), framebufferManager->getFramebuffers(), swapchainManager->getExtent(), pyramidVertex->getVertexBuffer(),
    pyramidIndex->getIndexBuffer(), indices, descriptorManager->getSet());

    std::cout << "loop \n";
    //main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float time = glfwGetTime();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, time, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time, glm::vec3(0.0f, 0.2f, 0.0f));
        model = glm::rotate(model, time, glm::vec3(0.0f, 0.0f, 0.5f));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
                                        swapchainManager->getExtent().width / (float) swapchainManager->getExtent().height,
                                        0.1f, 10.0f);
        proj[1][1] *= -1;

        UniformBufferObject ubo{};
        ubo.model = model;
        ubo.view = view;
        ubo.proj = proj;

        void* data;
        vkMapMemory(CoreVulkan::getDevice(), cameraBufferManager->getUniformBufferMemory(), 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(CoreVulkan::getDevice(), cameraBufferManager->getUniformBufferMemory());

        float aspectRatio = (float)swapchainManager->getExtent().width / (float)swapchainManager->getExtent().height;
        cameraBufferManager->updateUniformBuffer(aspectRatio);

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

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
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

        VkResult presentResult = vkQueuePresentKHR(graphicsQueue, &presentInfo);
        if (presentResult != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    //* cleanup
    vkQueueWaitIdle(graphicsQueue);
    vkDeviceWaitIdle(CoreVulkan::getDevice());
    delete(commandManager);
    delete(pyramidIndex);
    delete(pyramidVertex);
    delete(framebufferManager);
    delete(depthBufferManager);
    delete(graphicsPipeline);
    delete(shaderLoader);
    delete(descriptorManager);
    delete(cameraBufferManager);
    delete(renderPass);
    delete(swapchainManager);
    vkDestroySurfaceKHR(CoreVulkan::getInstance(), surface, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
};

void Render::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(CoreVulkan::getDevice(), &fenceInfo, nullptr, &this->inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

Render::~Render(){
    vkDestroySemaphore(CoreVulkan::getDevice(), this->renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(CoreVulkan::getDevice(), this->imageAvailableSemaphore, nullptr);
    vkDestroyFence(CoreVulkan::getDevice(), this->inFlightFence, nullptr);
}
