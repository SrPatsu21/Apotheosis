#pragma once

#include "../CoreVulkan.hpp"
#include <glm/glm.hpp>

class CameraBufferManager
{
private:
    /*
    A buffer that holds uniform data, such as camera matrices or scene parameters, that are the same for all vertices/fragments in a draw call.
    */
    VkBuffer* uniformBuffer;
    /*
    The memory backing the uniformBuffer.
    */
    VkDeviceMemory* uniformBufferMemory;
    /*
    @brief Creates a Vulkan uniform buffer and allocates memory for it.

    Sets up a GPU buffer with usage `VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT`,
    allocates host-visible and host-coherent memory, and binds the memory to the buffer.

    @throws std::runtime_error if creation, allocation, or memory binding fails.
    */
    void createUniformBuffer();

public:
    /*
    @brief Constructor for CameraBufferManager.

    Initializes the Vulkan uniform buffer and allocates its memory.
    This buffer is intended to store the camera's view/projection matrices or
    other per-frame uniform data.

    @throws std::runtime_error if buffer creation or memory allocation fails.
    */
    CameraBufferManager();

    /*
    @brief Destructor for CameraBufferManager.
    Cleans up the Vulkan uniform buffer and frees its associated memory.
    */
    ~CameraBufferManager();

    VkBuffer* getUniformBuffer() const { return uniformBuffer; }
    VkDeviceMemory* getUniformBufferMemory() const { return uniformBufferMemory; }
};