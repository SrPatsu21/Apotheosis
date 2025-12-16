#pragma once

#include <glm/glm.hpp>
#include <iostream>

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;

    Vertex(const glm::vec3& p, const glm::vec4& c) : pos(p), color(c) {}
};