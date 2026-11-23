#pragma once

#include "SkinnedVertexBufferManager.hpp"
#include "IndexBufferManager.hpp"
#include "../animations/skeleton/Skeleton.hpp"
#include "../animations/Animation.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SkinnedMesh {
public:

    struct MaterialData {
        std::string baseColorPath;
        std::string normalPath;
        std::string metallicRoughnessPath;
    };

    struct SubMesh {
        uint32_t firstIndex;
        uint32_t indexCount;
        int32_t  vertexOffset;
        uint32_t materialIndex;
    };

protected:
    std::unique_ptr<SkinnedVertexBufferManager> skinnedVertexBufferManager;
    std::unique_ptr<IndexBufferManager> indexBufferManager;

    std::vector<SubMesh> subMeshes;
    std::vector<MaterialData> materials;

    uint32_t indexCount = 0;

    std::shared_ptr<Skeleton> skeleton;

    std::vector<Animation> animations;


    void load(
        const std::string& path,
        const Skeleton* skeleton,
        std::vector<SkinnedVertex>& vertices,
        std::vector<uint32_t>& indices
    );

public:
    explicit SkinnedMesh(
        const std::string& path,
        VkDevice device,
        BufferManager* bufferManager
    );

    ~SkinnedMesh() = default;

    SkinnedMesh(const SkinnedMesh&) = delete;
    SkinnedMesh& operator=(const SkinnedMesh&) = delete;

    SkinnedMesh(SkinnedMesh&&) noexcept = delete;
    SkinnedMesh& operator=(SkinnedMesh&&) noexcept = delete;


    VkBuffer getIndexBuffer() const { return indexBufferManager->getIndexBuffer(); }
    VkBuffer getSkinnedVertexBufferManager() const { return skinnedVertexBufferManager->getVertexBuffer(); }

    const std::vector<SkinnedMesh::SubMesh>& getSubMeshes() const { return subMeshes; }
    const std::vector<SkinnedMesh::MaterialData>& getMaterials() const { return materials; }

    uint32_t getIndexCount() const { return indexCount; }

    const Skeleton* getSkeleton() const { return skeleton.get(); }
    Skeleton* getSkeleton() { return skeleton.get(); }

    const std::vector<Animation>& getAnimations() const { return animations; }
    std::vector<Animation>& getAnimations() { return animations; }
};
