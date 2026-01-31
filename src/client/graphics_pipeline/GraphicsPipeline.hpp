#pragma once

#include "../CoreVulkan.hpp"
#include "../mash/Vertex.hpp"
#include "ShaderLoader.hpp"
#include <array>

/**
@brief Manages the creation and lifetime of the Vulkan graphics pipeline.
*/
class GraphicsPipeline {
private:
    VkDevice device;
    /**
    @brief Vulkan graphics pipeline handle.

    Created during pipeline construction. Represents the compiled GPU pipeline
    object. Destroyed in the destructor.
    */
    VkPipeline graphicsPipeline{VK_NULL_HANDLE};

    /**
    @brief Vulkan pipeline layout handle.

    Stores information about descriptor sets and push constants used by the pipeline.
    Created during pipeline construction. Destroyed in the destructor.
    */
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};

    /**
    @brief Vertex binding description.

    Describes how vertex buffer data is bound, including stride and input rate.
    Used by VkPipelineVertexInputStateCreateInfo.
    */
    VkVertexInputBindingDescription bindingDescription{};

    /**
    @brief Vertex attribute descriptions.

    Defines the format and location of each vertex attribute (e.g. position, color).
    Used by VkPipelineVertexInputStateCreateInfo.
    */
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    /**
    @brief Viewport configuration.

    Defines the transformation of vertices from normalized device coordinates (NDC)
    to window coordinates.
    Used by VkPipelineViewportStateCreateInfo.
    */
    VkViewport viewport{};

    /**
    @brief Scissor rectangle configuration.

    Defines the rectangular region of the framebuffer where rendering is allowed.
    Pixels outside this region are discarded.
    Used by VkPipelineViewportStateCreateInfo.
    */
    VkRect2D scissor{};

    /**
    @brief Color blend attachment state.

    Defines how the fragment shader output is blended with the existing framebuffer
    contents for a single color attachment.
    Used by VkPipelineColorBlendStateCreateInfo.
    */
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    /**
    @brief Creates the vertex input state for the pipeline.

    Sets up how vertex buffers are bound and how vertex attributes are read
    (e.g., position and color formats/offsets).

    @return A fully populated VkPipelineVertexInputStateCreateInfo structure.
    */
    VkPipelineVertexInputStateCreateInfo createVertexInputState();
    /**
    @brief Creates the input assembly state for the pipeline.

    Configures how primitives are assembled from vertices (e.g., triangle list).

    @return A fully populated VkPipelineInputAssemblyStateCreateInfo structure.
    */
    VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState();
    /**
    @brief Creates the viewport and scissor state for the pipeline.

    Defines the viewport transformation and the scissor rectangle.

    @param extent The extent of the swapchain framebuffer (width and height).

    @return A fully populated VkPipelineViewportStateCreateInfo structure.
    */
    VkPipelineViewportStateCreateInfo createViewportState(VkExtent2D extent);
    /**
    @brief Creates the rasterizer state for the pipeline.

    Configures how geometry is rasterized into fragments, including culling,
    polygon mode, and front face winding order.

    @return A fully populated VkPipelineRasterizationStateCreateInfo structure.
    */
    VkPipelineRasterizationStateCreateInfo createRasterizerState();
    /**
    @brief Creates the multisampling state for the pipeline.

    Configures multisample anti-aliasing (disabled by default).

    @return A fully populated VkPipelineMultisampleStateCreateInfo structure.
    */
    VkPipelineMultisampleStateCreateInfo createMultisampleState(VkSampleCountFlagBits msaaSamples);
    /**
    @brief Creates the depth and stencil state for the pipeline.

    Enables depth testing and depth writes, with depth comparison set to LESS.
    Stencil testing is disabled by default.

    @return A fully populated VkPipelineDepthStencilStateCreateInfo structure.
    */
    VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    /**
    @brief Creates the color blending state for the pipeline.

    Configures how the fragment shader output is blended with the framebuffer.
    Blending is disabled, and all color channels are written.

    @return A fully populated VkPipelineColorBlendStateCreateInfo structure.
    */
    VkPipelineColorBlendStateCreateInfo createColorBlendState();

public:
    /**
    @brief Constructs a Vulkan graphics pipeline.

    Creates a pipeline layout and a graphics pipeline for rendering.
    Sets up all fixed-function pipeline stages and links provided shader modules.

    @param swapchainExtent The dimensions of the swapchain framebuffer (used for viewport/scissor).
    @param renderPass The Vulkan render pass this pipeline will be used with.
    @param vertShaderModule Precompiled SPIR-V vertex shader module.
    @param fragShaderModule Precompiled SPIR-V fragment shader module.
    @param descriptorSetLayout The descriptor set layout to be used in the pipeline.

    @throws std::runtime_error if pipeline layout or graphics pipeline creation fails.
    */
    GraphicsPipeline(
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass,
        VkDescriptorSetLayout descriptorSetLayout,
        VkSampleCountFlagBits msaaSamples
    );

    /**
    @brief Destructor.

    Cleans up the Vulkan graphics pipeline and pipeline layout.
    */
    ~GraphicsPipeline();

    VkPipeline getPipeline() const { return this->graphicsPipeline; }
    VkPipelineLayout getLayout() const { return this->pipelineLayout; }
    VkViewport& getViewport() { return this->viewport; }
    VkRect2D& getScissor() { return this->scissor; }
    VkPipelineColorBlendAttachmentState getColorBlendAttachment() const { return this->colorBlendAttachment; }
};
