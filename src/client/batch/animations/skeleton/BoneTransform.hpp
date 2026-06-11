#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct BoneTransform
{
    glm::vec3 translation = glm::vec3(0.0f);

    // https://www.thestembulletin.com/post/quaternions-what-are-they-and-do-we-really-need-them
    // https://youtu.be/MpyGVvC-13s?si
    glm::quat rotation = glm::quat(
        1.0f,
        0.0f,
        0.0f,
        0.0f
    );

    glm::vec3 scale = glm::vec3(1.0f);
};