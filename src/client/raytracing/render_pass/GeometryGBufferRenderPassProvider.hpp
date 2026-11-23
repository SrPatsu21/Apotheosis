#pragma once

#include "../../CoreVulkan.hpp"
#include "../../render_pass/RenderPassManager.hpp"

class GeometryGBufferRenderPassProvider
{
public:

    static void build(
        RenderPassManager::Description& description,
        VkSampleCountFlagBits msaaSamples,
        VkFormat depthFormat
    );
};