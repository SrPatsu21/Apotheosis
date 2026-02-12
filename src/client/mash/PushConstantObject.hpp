#pragma once

#include <glm/glm.hpp>

struct PushConstantObject {
    alignas(16) glm::mat4 model;
};