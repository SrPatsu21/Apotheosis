#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vertex.hpp" // onde seu struct Vertex está

class MeshLoader {
private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

public:
    void load(const std::string& path);

    explicit MeshLoader(const std::string& path) {
        load(path);
    }
    ~MeshLoader() = default;

    MeshLoader(const MeshLoader&) = delete;
    MeshLoader& operator=(const MeshLoader&) = delete;

    MeshLoader(MeshLoader&&) noexcept = default;
    MeshLoader& operator=(MeshLoader&&) noexcept = default;

    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }
};
