#pragma once

#include "ResourceManager.hpp"
#include "instance/InstanceData.hpp"
#include "instance/SkinnedRenderInstance.hpp"
#include "../graphics_pipeline/GraphicsPipeline.hpp"

#include <list>

class RenderSkinnedBatch;
class RenderSkinnedBatchManager
{
public:

    struct BatchKey
    {
        std::shared_ptr<SkinnedMesh> mesh;
        const SkinnedMesh::SubMesh* submesh;
        std::shared_ptr<Material> material;
        GraphicsPipeline::PipelineFlags pipelineFlags;

        bool operator==(const RenderSkinnedBatchManager::BatchKey& other) const;
        bool operator<(const RenderSkinnedBatchManager::BatchKey& other) const;
    };

    struct BatchKeyHasher
    {
        size_t operator()(const BatchKey& key) const
        {
            size_t seed = 0;

            auto hash_combine = [&seed](size_t value)
            {
                seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            };

            hash_combine(std::hash<GraphicsPipeline::PipelineFlags>()(key.pipelineFlags));
            hash_combine(std::hash<SkinnedMesh*>()(key.mesh.get()));
            hash_combine(std::hash<const SkinnedMesh::SubMesh*>()(key.submesh));
            hash_combine(std::hash<Material*>()(key.material.get()));

            return seed;
        }
    };

private:

    std::unordered_map<BatchKey, std::unique_ptr<RenderSkinnedBatch>, BatchKeyHasher> batches_map;
    std::vector<RenderSkinnedBatch*> batches_sorted;
    bool batches_dirty = true;

    std::shared_ptr<SkinnedMesh> testMesh;
    std::shared_ptr<Material> testMaterial;
    std::shared_ptr<SkinnedRenderInstance> testRInstance;

    ResourceManager* resourceManager;

public:
    void addInstance(
        std::shared_ptr<SkinnedMesh> mesh,
        SkinnedRenderInstance* instance
    );

    bool removeInstance(
        SkinnedRenderInstance* instance
    );

    void findBatchKey(
        const std::string& meshPath,
        uint32_t submeshIndex,
        BatchKey& key
    );

    RenderSkinnedBatchManager::BatchKey findBatchKey(
        const std::string& meshPath,
        uint32_t submeshIndex
    );

    template<typename Func> void forEachBatch(Func&& func)
    {
        rebuildSortedBatches();

        for (RenderSkinnedBatch* batch : batches_sorted)
        {
            func(*batch);
        }
    }


    void rebuildSortedBatches();

    RenderSkinnedBatchManager(ResourceManager* resourceManager);
    ~RenderSkinnedBatchManager() = default;

    #ifndef NDEBUG
    void batchSize(){
        std::cout << "batches map: " << batches_map.size() << " batch sorted:" << batches_sorted.size() << std::endl;
    }
    #endif
};

class RenderSkinnedBatch {
    friend class RenderSkinnedBatchManager;
private:
    RenderSkinnedBatchManager::BatchKey batchKey;
    std::vector<SkinnedRenderInstance::BatchRegistration*> batchRegistrations;
    std::vector<InstanceData> instancesData;
    std::vector<uint32_t> boneOffsets;
public:
    explicit RenderSkinnedBatch(
        RenderSkinnedBatchManager::BatchKey batchKey
    );

    ~RenderSkinnedBatch();

    RenderSkinnedBatch(const RenderSkinnedBatch& other) = delete;
    RenderSkinnedBatch& operator=(const RenderSkinnedBatch& other) = delete;

    RenderSkinnedBatch(RenderSkinnedBatch&& other) noexcept;
    RenderSkinnedBatch& operator=(RenderSkinnedBatch&& other) noexcept;

    void addInstance(
        SkinnedRenderInstance* instance
    );

    void removeInstance(
        SkinnedRenderInstance* instance,
        size_t intregistrationsIndex
    );

    bool empty();

    const RenderSkinnedBatchManager::BatchKey& getKey() const { return batchKey; }

    std::vector<SkinnedRenderInstance::BatchRegistration*> getSkinnedRenderInstance() const{ return batchRegistrations; }
    std::vector<InstanceData>& getinstancesData() { return instancesData; }
    std::vector<uint32_t>& getBoneOffsets() { return boneOffsets;}
};