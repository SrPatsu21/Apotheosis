#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexBufferManager.hpp"
#include "IndexBufferManager.hpp"

class Mesh {
private:
    std::unique_ptr<VertexBufferManager> vertexBufferManager;
    std::unique_ptr<IndexBufferManager> indexBufferManager;
    uint32_t indexCount;

    void load(
        const std::string& path,
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices
    );

public:
    explicit Mesh(
        const std::string& path,
        VkDevice device,
        BufferManager* bufferManager
    );
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = delete;
    Mesh& operator=(Mesh&&) noexcept = delete;

    VkBuffer getIndexBuffer() const {return indexBufferManager.get()->getIndexBuffer();}
    VkBuffer getVertexBuffer() const {return vertexBufferManager.get()->getVertexBuffer();}
    uint32_t getIndexCount() const {return indexCount;}
};
