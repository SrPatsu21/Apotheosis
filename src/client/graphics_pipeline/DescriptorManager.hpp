#pragma once

#include "../CoreVulkan.hpp"
#include "../camera/CameraBufferManager.hpp"
#include "../image/TextureImage.hpp"
#include <glm/glm.hpp>

/*
    This class is responsible for creating and managing Vulkan descriptor-related objects:
    - Descriptor set layout
    - Descriptor pool
    - Descriptor set

    Descriptors are how you tell shaders which GPU resources to use (like uniform buffers or textures).
    This class encapsulates the Vulkan API calls needed to set up these descriptors,
    simplifying the process of binding resources to shaders.

    Typical usage:
        DescriptorManager descriptors;
        descriptors.createDescriptorSetLayout(device);
        descriptors.createDescriptorPoolAndSet(device, uniformBuffer);

    The class owns and cleans up all Vulkan handles it creates.
*/
class DescriptorManager
{
private:
    VkDevice device;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
public:
    /**
    Constructor.
    Initializes the descriptor management system for Vulkan.

    This constructor creates the descriptor set layout and the descriptor pool,
    allocates the descriptor set, and writes the provided uniform buffer into the descriptor set.
    After construction, the DescriptorManager holds all the Vulkan descriptor resources
    needed to bind the uniform buffer to the shaders.

    @param uniformBuffer Pointer to the Vulkan buffer that holds uniform data (e.g., camera matrices).

    @throws std::runtime_error if descriptor set layout creation, pool creation, or descriptor set allocation fails.
    */
    DescriptorManager(
        VkDevice device,
        CameraBufferManager* cameraBufferManager,
        TextureImage* textureImage,
        uint32_t max_frames_in_flight
    );
    /*
    defines what kind of resources your shaders expect to be bound, and where
    @return VkDescriptorSetLayout* descriptorSetLayout
    @throws "failed to create descriptor pool!"
    */
    void createDescriptorSetLayout();
    void createDescriptorPool(uint32_t max_frames_in_flight);
    void createDescriptorSets(uint32_t max_frames_in_flight);
    void populateDescriptorSets(
        CameraBufferManager* cameraBufferManager,
        TextureImage* textureImage,
        uint32_t max_frames_in_flight
    );

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getPool() const { return descriptorPool; }
    std::vector<VkDescriptorSet> getSets() const { return descriptorSets; }

    /*
    */
    ~DescriptorManager();
};