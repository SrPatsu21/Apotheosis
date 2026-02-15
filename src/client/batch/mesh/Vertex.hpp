#pragma once

#include "../../CoreVulkan.hpp"
#include <glm/glm.hpp>
#include <array>

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;

    Vertex(const glm::vec3 p, const glm::vec4 c, const glm::vec2 t) : pos(p), color(c), texCoord(t) {}

    // bindingDescription.binding = 0;
    // bindingDescription.stride = sizeof(Vertex);
    // bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    static const VkVertexInputBindingDescription getBindingDescription() {
        static const VkVertexInputBindingDescription bindingDescription{
            0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX
        };
        return bindingDescription;
    }

    //     attributeDescriptions[0].binding = 0;
    //     attributeDescriptions[0].location = 0;
    //     attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    //     attributeDescriptions[0].offset = offsetof(Vertex, pos);
    static const std::array<VkVertexInputAttributeDescription, 3>& getAttributeDescriptions() {
        static const std::array<VkVertexInputAttributeDescription, 3> attributes{{
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT,     offsetof(Vertex, pos)},
            {0, 1, VK_FORMAT_R32G32B32A32_SFLOAT,  offsetof(Vertex, color)},
            {0, 2, VK_FORMAT_R32G32_SFLOAT,     offsetof(Vertex, texCoord)}
        }};
        return attributes;
    }
};
