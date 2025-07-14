#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

class Renderer {
public:
    void run();

private:

//* Command Pool
/*
A Vulkan object that manages the memory used to allocate and record command buffers.
Command buffers are the actual "scripts" of GPU instructions that you submit to a queue.
You allocate command buffers from a command pool.
*/
VkCommandPool commandPool;

/*
The list of Vulkan command buffers, one for each framebuffer in the swapchain.
Each command buffer records all the rendering commands for a single frame.
These are allocated from the command pool and reused every frame.
*/
std::vector<VkCommandBuffer> commandBuffers;

//* Framebuffers
/*
A list of framebuffers — one for each swapchain image.
A framebuffer is what your render pass writes into. It combines attachments:
color (the swapchain image) and optionally depth/stencil.
You need one framebuffer per swapchain image.
*/
std::vector<VkFramebuffer> swapchainFramebuffers;

//* Swapchain Extent
/*
The dimensions (width & height) of the swapchain images, in pixels.
This usually matches the window's size and defines the resolution you render at.
*/
VkExtent2D swapchainExtent;

//* Synchronization — Semaphores and Fence
/*
A semaphore that signals when an image from the swapchain is ready for rendering.
You wait on this before recording and submitting your draw commands.
*/
VkSemaphore imageAvailableSemaphore;

/*
A semaphore that signals when rendering is finished and the rendered image can now be presented to the screen.
The presentation queue waits on this before swapping the image onto the screen.
*/
VkSemaphore renderFinishedSemaphore;

/*
A fence that signals when the GPU has finished processing a frame.
The CPU waits on this fence to make sure the GPU is done with the previous frame before starting the next one.
Helps prevent overwriting resources still in use.
*/
VkFence inFlightFence;



//TODO remove
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

};