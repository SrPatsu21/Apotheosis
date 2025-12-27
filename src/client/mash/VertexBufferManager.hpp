#pragma once

#include "../CoreVulkan.hpp"
#include "Vertex.hpp"
#include "BufferManager.hpp"

#include <cstring>

class VertexBufferManager
{
private:
    /*
    A GPU buffer that holds the mesh's vertex data (positions, normals, etc.).
    A vertex, also called a corner, is a point where two or more curves, lines, or line segments meet or intersect.
    more <https://en.wikipedia.org/wiki/Vertex_(geometry)>
    */
    VkBuffer vertexBuffer;

    /*
    The actual memory allocation on the GPU that backs the vertexBuffer.
    (Vulkan separates buffer objects and memory allocations explicitly.)
    */
    VkDeviceMemory vertexBufferMemory;
public:
    /*
    @brief Constructs a VertexBufferManager and creates a vertex buffer on the GPU.

    This constructor:
    - creates a Vulkan vertex buffer of appropriate size,
    - finds a suitable GPU memory type (host-visible & coherent),
    - allocates and binds GPU memory for the buffer,
    - copies the provided vertex data into the GPU memory.

    The created buffer and its memory are stored in `this->vertexBuffer` and `this->vertexBufferMemory`.

    @param vertices Pointer to the vector of Vertex structs to upload.

    @throws std::runtime_error if:
            - buffer creation fails,
            - memory allocation fails,
            - suitable memory type is not found,
            - memory mapping fails.
    */
    VertexBufferManager(const std::vector<Vertex> vertices, VkCommandPool commandPool);
    /*
    @brief Destructor. Cleans up the vertex buffer and its allocated memory.

    This releases the GPU resources allocated for the vertex buffer
    by destroying the buffer and freeing its memory.
    */
    ~VertexBufferManager();

    VkBuffer getVertexBuffer() const {return vertexBuffer;}
    VkDeviceMemory getVertexBufferMemory() const {return vertexBufferMemory;}
};