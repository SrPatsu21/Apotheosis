#pragma once

#include "ResourceManager.hpp"
#include "instance/InstanceData.hpp"
#include "instance/RenderInstance.hpp"

#include <list>

class RenderBatch;
class RenderBatchManager
{
public:

    //TODO add pipeline
    struct BatchKey
    {
        std::shared_ptr<Mesh> mesh;
        const Mesh::SubMesh* submesh;

        bool operator==(const RenderBatchManager::BatchKey& other) const;
        bool operator<(const RenderBatchManager::BatchKey& other) const;
    };

    struct BatchKeyHasher
    {
        size_t operator()(const BatchKey& key) const
        {
            size_t h1 = std::hash<Mesh*>()(key.mesh.get());
            size_t h2 = std::hash<const Mesh::SubMesh*>()(key.submesh);
            return h1 ^ (h2 << 1);
        }
    };

private:

    std::unordered_map<BatchKey, std::unique_ptr<RenderBatch>, BatchKeyHasher> batches_map;
    std::vector<RenderBatch*> batches_sorted;
    bool batches_dirty = false;

    ResourceManager* resourceManager;

public:
    void addInstance(
        std::shared_ptr<Mesh> mesh,
        RenderInstance* instance
    );

    bool removeInstance(
        RenderInstance* instance
    );

    void findBatchKey(
        const std::string& meshPath,
        uint32_t submeshIndex,
        BatchKey& key
    );

    RenderBatchManager::BatchKey findBatchKey(
        const std::string& meshPath,
        uint32_t submeshIndex
    );

    template<typename Func> void forEachBatch(Func&& func)
    {
        rebuildSortedBatches();

        for (auto* batch : batches_sorted)
        {
            func(*batch);
        }
    }


    void rebuildSortedBatches();

    RenderBatchManager(ResourceManager* resourceManager);
    ~RenderBatchManager() = default;
};

class RenderBatch {
    friend class RenderBatchManager;
private:
    RenderBatchManager::BatchKey batchKey;
    std::vector<RenderInstance::BatchRegistration*> batchRegistrations;
    std::vector<InstanceData> instancesData;
public:
    explicit RenderBatch(
        RenderBatchManager::BatchKey batchKey
    );

    ~RenderBatch();

    RenderBatch(const RenderBatch& other) = delete;
    RenderBatch& operator=(const RenderBatch& other) = delete;

    RenderBatch(RenderBatch&& other) noexcept;
    RenderBatch& operator=(RenderBatch&& other) noexcept;

    void addInstance(
        RenderInstance* instance,
        size_t intregistrationsIndex,
        std::shared_ptr<Material> material
    );

    void removeInstance(
        RenderInstance* instance,
        size_t intregistrationsIndex
    );

    bool empty();

    const RenderBatchManager::BatchKey& getKey() const { return batchKey; }

    bool isEquivalent(
        const std::shared_ptr<Mesh>& mesh,
        const std::shared_ptr<Material>& material
    ) const;

    std::vector<RenderInstance::BatchRegistration*> getRenderInstance() const{ return batchRegistrations; }
    std::vector<InstanceData> getinstancesData() const { return instancesData; }
};