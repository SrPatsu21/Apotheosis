#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    Vertex(const glm::vec3& p, const glm::vec3& c) : pos(p), color(c) {}
};