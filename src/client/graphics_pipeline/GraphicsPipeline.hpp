#pragma once

#include "../CoreVulkan.hpp"
#include "ShaderLoader.hpp"
#include "../batch/instance/InstanceData.hpp"
#include "../particle/ParticleData.hpp"
#include "../batch/mesh/Vertex.hpp"
#include <array>
#include <unordered_map>

class GraphicsPipeline {
public:
    enum class PipelineType {
        Triangles_NoCull,
        Triangles_BackCull,
        Triangles_FrontCull,
        Lines,
        Points
    };

    enum class LayoutType {
        Mesh,
        Particle
    };
private:
    VkDevice device;

    std::unordered_map<PipelineType, VkPipeline> graphicsPipelines;
    std::unordered_map<LayoutType, VkPipelineLayout> pipelineLayouts;
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    VkPipelineLayout createPipelineLayout(
        uint32_t pushConstantRangeSize,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
    );

    VkPipelineVertexInputStateCreateInfo createVertexInputState(
        VkVertexInputBindingDescription& bindingDescription,
        std::array<VkVertexInputAttributeDescription, 3>& attributeDescriptions
    );
    VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState(
        VkPrimitiveTopology topology
    );
    VkPipelineViewportStateCreateInfo createViewportState(
        VkViewport& viewport,
        VkRect2D& scissor
    );
    VkPipelineRasterizationStateCreateInfo createRasterizerState(
        VkCullModeFlags cullMode,
        VkPolygonMode polygonMode
    );
    VkPipelineMultisampleStateCreateInfo createMultisampleState(
        VkSampleCountFlagBits msaaSamples
    );
    VkPipelineDynamicStateCreateInfo createDynamicState(
        const std::vector<VkDynamicState>& dynamicState
    );
    VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    VkPipelineColorBlendStateCreateInfo createColorBlendState(
        VkPipelineColorBlendAttachmentState& colorBlendAttachment
    );

    VkPipeline createPipeline(
        const VkRenderPass renderPass,
        const VkPipelineLayout& pipelineLayout,
        const VkPipelineShaderStageCreateInfo* shaderStages,
        const VkPipelineVertexInputStateCreateInfo& vertexInput,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
        const VkPipelineViewportStateCreateInfo& viewportState,
        const VkPipelineRasterizationStateCreateInfo& rasterizer,
        const VkPipelineMultisampleStateCreateInfo& multisampling,
        const VkPipelineDepthStencilStateCreateInfo& depthStencil,
        const VkPipelineColorBlendStateCreateInfo& colorBlend,
        const VkPipelineDynamicStateCreateInfo& dynamicState
    );

public:

    GraphicsPipeline(
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalLayout,
        VkDescriptorSetLayout materialLayout,
        VkDescriptorSetLayout instanceLayout,
        VkDescriptorSetLayout particleLayout,
        VkSampleCountFlagBits msaaSamples
    );

    ~GraphicsPipeline();

    VkPipeline getPipeline(PipelineType type) const { return graphicsPipelines.at(type); }
    VkPipelineLayout getLayout(LayoutType type) const { return pipelineLayouts.at(type); }
    const VkViewport& getViewport() const  { return viewport; }
    const VkRect2D& getScissor() const { return scissor; }
    const VkPipelineColorBlendAttachmentState& getColorBlendAttachment() const { return colorBlendAttachment; }
};
