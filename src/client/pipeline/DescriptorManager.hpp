#pragma once

#include "../CoreVulkan.hpp"
#include "../camera/CameraBufferManager.hpp"
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
    /*
    Defines the layout of resources (uniforms, samplers, etc.) that shaders will access.
    Specifies what kinds of descriptors are expected and their bindings.
    */
    VkDescriptorSetLayout descriptorSetLayout;
    /*
    A pool from which descriptor sets are allocated.
    Descriptors are lightweight GPU-side handles to resources like uniform buffers or textures.
    */
    VkDescriptorPool descriptorPool;
    /*
    A descriptor set, allocated from descriptorPool, which binds your actual resources to shader binding points as defined by descriptorSetLayout.
    */
    VkDescriptorSet descriptorSet;
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
    DescriptorManager(CameraBufferManager* cameraBufferManager);
    /*
    defines what kind of resources your shaders expect to be bound, and where
    @return VkDescriptorSetLayout* descriptorSetLayout
    @throws "failed to create descriptor pool!"
    */
    void createDescriptorSetLayout();

    /*
    creates a descriptor pool, allocates a descriptor set, and writes the uniform buffer into the descriptor set.
    So, it essentially sets up everything needed to bind your uniform buffer to your shaders at runtime.
    @param VkBuffer* uniformBuffer
    @param CameraBufferManager* cameraBufferManager
    @return VkDescriptorPool* descriptorPool, VkDescriptorSet* descriptorSet
    @throws "failed to allocate descriptor set!"
    */
    void createDescriptorPoolAndSet(CameraBufferManager* cameraBufferManager);

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getPool() const { return descriptorPool; }
    VkDescriptorSet getSet() const { return descriptorSet; }

    /*
    */
    ~DescriptorManager();
};