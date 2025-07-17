#pragma once

#include "../CoreVulkan.hpp"

/**
@brief Manages the creation and lifetime of the Vulkan graphics pipeline.
 */
class GraphicsPipeline {
private:
    VkPipeline graphicsPipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};

    VkPipelineVertexInputStateCreateInfo createVertexInputState();
    VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState();
    VkPipelineViewportStateCreateInfo createViewportState(VkExtent2D extent);
    VkPipelineRasterizationStateCreateInfo createRasterizerState();
    VkPipelineMultisampleStateCreateInfo createMultisampleState();
    VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    VkPipelineColorBlendStateCreateInfo createColorBlendState();
public:
    /**
    @brief Creates a graphics pipeline for rendering.

    @param swapchainExtent The extent of the swapchain (viewport & scissor size).
    @param renderPass The render pass this pipeline will be used with.
    @param vertShaderModule The vertex shader module.
    @param fragShaderModule The fragment shader module.
    @param descriptorSetLayout The descriptor set layout to use.
    */
    GraphicsPipeline(
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass,
        VkShaderModule vertShaderModule,
        VkShaderModule fragShaderModule,
        VkDescriptorSetLayout descriptorSetLayout);

    ~GraphicsPipeline();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
};
