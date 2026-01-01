#pragma once
#include "../CoreVulkan.hpp"
#include "../swapchain&framebuffer/SwapchainManager.hpp"
#include "UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include "../mash/BufferManager.hpp"

class CameraBufferManager
{
private:
    /*
    A buffer that holds uniform data, such as camera matrices or scene parameters, that are the same for all vertices/fragments in a draw call.
    */
    std::vector<VkBuffer> uniformBuffers;
    /*
    The memory backing the uniformBuffer.
    */
    std::vector<VkDeviceMemory> uniformBuffersMemory;


    std::vector<void*> uniformBuffersMapped;
    /*
    @brief Creates a Vulkan uniform buffer and allocates memory for it.

    Sets up a GPU buffer with usage `VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT`,
    allocates host-visible and host-coherent memory, and binds the memory to the buffer.

    @throws std::runtime_error if creation, allocation, or memory binding fails.
    */
    void createUniformBuffer(BufferManager* bufferManager, int max_frames_in_flight);

public:
    /*
    @brief Constructor for CameraBufferManager.

    Initializes the Vulkan uniform buffer and allocates its memory.
    This buffer is intended to store the camera's view/projection matrices or
    other per-frame uniform data.

    @throws std::runtime_error if buffer creation or memory allocation fails.
    */
    CameraBufferManager(BufferManager* bufferManager, int max_frames_in_flight);

    /*
    @brief Destructor for CameraBufferManager.
    Cleans up the Vulkan uniform buffer and frees its associated memory.
    */
    ~CameraBufferManager();

    void updateUniformBuffer(SwapchainManager* swapchainManager, uint32_t currentFrame, float time);

    const std::vector<VkBuffer>& getUniformBuffers() const { return uniformBuffers; }
    std::vector<VkDeviceMemory> getUniformBufferMemorys() const { return uniformBuffersMemory; }
    std::vector<void*> getUniformBuffersMapped() const {return uniformBuffersMapped; }
};