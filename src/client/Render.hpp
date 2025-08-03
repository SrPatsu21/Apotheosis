#pragma once

#include "CoreVulkan.hpp"
#include "mash/VertexManager.hpp"

// #include <stdexcept>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <cstring>

class Render {
public:
    Render();
    int run();

    ~Render();
private:

//screen width in px
uint32_t width = 800;

// screen height in px
uint32_t height = 600;

/**
A semaphore that signals when an image from the swapchain is ready for rendering.
You wait on this before recording and submitting your draw commands.
*/
VkSemaphore imageAvailableSemaphore;

/**
A semaphore that signals when rendering is finished and the rendered image can now be presented to the screen.
The presentation queue waits on this before swapping the image onto the screen.
*/
VkSemaphore renderFinishedSemaphore;

/**
A fence that signals when the GPU has finished processing a frame.
The CPU waits on this fence to make sure the GPU is done with the previous frame before starting the next one.
Helps prevent overwriting resources still in use.
*/
VkFence inFlightFence;

void initWindow();
void initVulkan();
void mainLoop();
void cleanup();

/**
 * @brief Creates the Vulkan synchronization objects used per-frame.
 *
 * This function initializes the semaphores and fence needed to coordinate
 * rendering and presentation in a single frame:
 *
 * - `imageAvailableSemaphore`: Signaled when an image from the swapchain becomes available
 *   for rendering. It is waited on before recording drawing commands.
 *
 * - `renderFinishedSemaphore`: Signaled when rendering of the current frame finishes.
 *   It is waited on before presenting the rendered image to the screen.
 *
 * - `inFlightFence`: A fence that signals when the GPU is done processing commands
 *   for the current frame, allowing the CPU to safely reuse resources.
 *   This fence is initialized in a signaled state so the first frame can proceed without blocking.
 *
 * Throws a std::runtime_error if any Vulkan call to create these synchronization
 * objects fails.
 *
 * @note The objects are created for the current frame. In a multi-frame setup,
 * you'd create multiple sets of these per frame in flight.
 */
void createSyncObjects();
};