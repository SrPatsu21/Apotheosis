#pragma once

#include <glm/glm.hpp>

struct UniformBufferObject {
    glm::vec2 foo;          // offset 0   (8 bytes)
    glm::vec2 _padding;     // offset 8   (8 bytes)
    alignas(16) glm::mat4 model; // offset 16  (64 bytes)
    alignas(16) glm::mat4 view;  // offset 80  (64 bytes)
    alignas(16) glm::mat4 proj;  // offset 144 (64 bytes)
};