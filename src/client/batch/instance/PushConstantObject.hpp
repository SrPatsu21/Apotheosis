#pragma once

#include <glm/glm.hpp>

struct PushConstantObject {
    alignas(16) glm::mat4 model;

    PushConstantObject() : model(glm::mat4(1.0f)) {}

    PushConstantObject(glm::mat4 model) : model(model){}

    ~PushConstantObject() = default;

    PushConstantObject(const PushConstantObject& other) : model(other.model) {}
    PushConstantObject& operator=(const PushConstantObject& other) {
        if (this != &other) {
            model = other.model;
        }
        return *this;
    }

    PushConstantObject(PushConstantObject&& other) noexcept : model(std::move(other.model)) {}
    PushConstantObject& operator=(PushConstantObject&& other) noexcept {
        if (this != &other) {
            model = std::move(other.model);
        }
        return *this;
    }
};