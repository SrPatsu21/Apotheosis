#pragma once

#include "../CoreVulkan.hpp"
#include "../swapchain&framebuffer/SwapchainManager.hpp"
#include "UniformBufferGlobal.hpp"
#include "../BufferManager.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstring>
#include <vector>

/**
 * @brief Manages camera state and per-frame camera uniform buffers.
 *
 * CameraBufferManager owns:
 * - Camera position
 * - Camera rotation (yaw / pitch)
 * - Mouse input
 * - Keyboard input
 * - View matrix
 * - Projection matrix
 * - Per-frame uniform buffers
 *
 * Controls:
 * - W: Move forward
 * - S: Move backward
 * - A: Move left
 * - D: Move right
 * - Space: Move up
 * - Left Ctrl: Move down
 * - Mouse: Rotate camera
 */
class CameraBufferManager
{
private:
    VkDevice device = VK_NULL_HANDLE;
    GLFWwindow* window = nullptr;

    // -------------------------------------------------------------------------
    // Uniform buffers
    // -------------------------------------------------------------------------

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // -------------------------------------------------------------------------
    // Camera state
    // -------------------------------------------------------------------------

    glm::vec3 position = glm::vec3(
        0.0f,
        0.0f,
        3.0f
    );

    // Rotation in degrees
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Camera movement speed in world units per second
    float movementSpeed = 5.0f;

    // Mouse sensitivity
    float mouseSensitivity = 0.1f;

    // -------------------------------------------------------------------------
    // Mouse state
    // -------------------------------------------------------------------------

    bool firstMouse = true;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    // -------------------------------------------------------------------------
    // Camera direction
    // -------------------------------------------------------------------------

    glm::vec3 getForward() const;

    glm::vec3 getRight() const;

    // -------------------------------------------------------------------------
    // Input
    // -------------------------------------------------------------------------

    void processMouse();

    void processKeyboard(float deltaTime);

    void processInput(float deltaTime);

public:

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------

    CameraBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        int max_frames_in_flight,
        GLFWwindow* window
    );

    // -------------------------------------------------------------------------
    // Destructor
    // -------------------------------------------------------------------------

    ~CameraBufferManager();

    // -------------------------------------------------------------------------
    // Camera update
    // -------------------------------------------------------------------------

    void updateCamera(
        uint32_t currentFrame,
        float deltaTime,
        const VkExtent2D& extent
    );

    // -------------------------------------------------------------------------
    // Update UBO
    // -------------------------------------------------------------------------

    void update(
        uint32_t currentFrame,
        const UniformBufferGlobal& ubg
    );

    // -------------------------------------------------------------------------
    // Getters
    // -------------------------------------------------------------------------

    const std::vector<VkBuffer>& getUniformBuffers() const
    {
        return uniformBuffers;
    }

    const std::vector<VkDeviceMemory>& getUniformBufferMemorys() const
    {
        return uniformBuffersMemory;
    }

    const std::vector<void*>& getUniformBuffersMapped() const
    {
        return uniformBuffersMapped;
    }

    const glm::vec3& getPosition() const
    {
        return position;
    }

    float getYaw() const
    {
        return yaw;
    }

    float getPitch() const
    {
        return pitch;
    }

    // -------------------------------------------------------------------------
    // Setters
    // -------------------------------------------------------------------------

    void setMovementSpeed(float speed)
    {
        movementSpeed = speed;
    }

    void setMouseSensitivity(float sensitivity)
    {
        mouseSensitivity = sensitivity;
    }
};