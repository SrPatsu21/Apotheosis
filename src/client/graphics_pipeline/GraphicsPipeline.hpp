#pragma once

#include "../CoreVulkan.hpp"
#include "ShaderLoader.hpp"
#include "../batch/instance/InstanceData.hpp"
#include "../batch/mesh/Vertex.hpp"
#include <array>
#include <unordered_map>

class GraphicsPipeline {
private:
    VkDevice device;

    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkVertexInputBindingDescription bindingDescription{};
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    VkPipelineVertexInputStateCreateInfo createVertexInputState();
        VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState(
        VkPrimitiveTopology topology
    );
    VkPipelineViewportStateCreateInfo createViewportState(VkExtent2D extent);
    VkPipelineRasterizationStateCreateInfo createRasterizerState(
        VkCullModeFlags cullMode,
        VkPolygonMode polygonMode
    );
    VkPipelineMultisampleStateCreateInfo createMultisampleState(VkSampleCountFlagBits msaaSamples);
    VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    VkPipelineColorBlendStateCreateInfo createColorBlendState();

    VkPipeline createPipeline(
        VkRenderPass renderPass,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples,
        VkPipelineShaderStageCreateInfo* shaderStages,
        VkPipelineDynamicStateCreateInfo* dynamicState,
        VkPrimitiveTopology topology,
        VkCullModeFlags cullMode,
        VkPolygonMode polygonMode
    );

public:
    enum class PipelineType {
        Triangles_NoCull,
        Triangles_BackCull,
        Triangles_FrontCull,
        Lines,
        Points
    };
    std::unordered_map<PipelineType, VkPipeline> graphicsPipelines;

    GraphicsPipeline(
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
        VkSampleCountFlagBits msaaSamples
    );

    ~GraphicsPipeline();

    VkPipeline getPipeline(PipelineType type) const { return graphicsPipelines.at(type); }
    VkPipelineLayout getLayout() const { return this->pipelineLayout; }
    VkViewport& getViewport() { return this->viewport; }
    VkRect2D& getScissor() { return this->scissor; }
    VkPipelineColorBlendAttachmentState getColorBlendAttachment() const { return this->colorBlendAttachment; }
};
