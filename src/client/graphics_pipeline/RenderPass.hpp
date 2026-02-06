#pragma once

#include "../CoreVulkan.hpp"

class RenderPass {
private:
    VkDevice device;
    VkRenderPass renderPass{VK_NULL_HANDLE};

public:
    struct AttachmentDesc {
        VkFormat format;
        VkSampleCountFlagBits samples;
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
        VkImageLayout initialLayout;
        VkImageLayout finalLayout;

        AttachmentDesc(
            VkFormat format,
            VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp,
            VkAttachmentStoreOp storeOp,
            VkImageLayout initialLayout,
            VkImageLayout finalLayout
        ) :
            format(format),
            samples(samples),
            loadOp(loadOp),
            storeOp(storeOp),
            initialLayout(initialLayout),
            finalLayout(finalLayout)
        {}
    };

    struct SubpassDesc {
        std::vector<uint32_t> colorAttachments;
        std::optional<uint32_t> depthAttachment;
        std::vector<uint32_t> resolveAttachments;
    };

    struct IRenderPassProvider {
        virtual ~IRenderPassProvider() = default;

        virtual void contribute(
            std::vector<AttachmentDesc>& attachments,
            std::vector<SubpassDesc>& subpasses,
            std::vector<VkSubpassDependency>& dependencies
        ) = 0;
    };

    bool equalDependency(
        const VkSubpassDependency& a,
        const VkSubpassDependency& b
    );

    void deduplicateDependencies(
        std::vector<VkSubpassDependency>& deps
    );

    RenderPass(
        VkDevice device,
        VkFormat swapchainImageFormat,
        VkSampleCountFlagBits msaaSamples,
        VkFormat depthFormat,
        const std::vector<IRenderPassProvider*> provider
    );

    ~RenderPass();

    VkRenderPass get() const { return renderPass; }
};