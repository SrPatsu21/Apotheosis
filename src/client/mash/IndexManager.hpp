#pragma once

#include "../CoreVulkan.hpp"

#include <cstring>

class IndexManager
{
private:
    /*
    Same as vertexBuffer, but for indices (used in indexed drawing).
    */
    VkBuffer* indexBuffer;
    /*
    GPU memory backing the indexBuffer.
    */
    VkDeviceMemory* indexBufferMemory;
public:
    /*
    @brief Constructs an IndexManager and creates an index buffer on the GPU.

    This constructor:
    - creates a Vulkan index buffer of appropriate size,
    - finds a suitable GPU memory type (host-visible & coherent),
    - allocates and binds GPU memory for the buffer,
    - copies the provided index data into the GPU memory.

    The created buffer and its memory are stored in `this->indexBuffer` and `this->indexBufferMemory`.

    @param indices Pointer to the vector of 16-bit indices to upload.

    @throws std::runtime_error if:
            - buffer creation fails,
            - memory allocation fails,
            - suitable memory type is not found,
            - memory mapping fails.
    */
    IndexManager(const std::vector<uint16_t>* indices);
    /*
    @brief Destructor. Cleans up the index buffer and its allocated memory.

    This releases the GPU resources allocated for the index buffer
    by destroying the buffer and freeing its memory.
    */
    ~IndexManager();

    VkBuffer* getIndexBuffer() const {return indexBuffer;}
    VkDeviceMemory* getIndexBufferMemory() const {return indexBufferMemory;}
};