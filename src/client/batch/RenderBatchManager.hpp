#pragma once

#include "ResourceManager.hpp"
#include "instance/InstanceData.hpp"

#include <list>

class RenderInstance;
class Mesh;
class Material;

class RenderBatchManager
{
public:

    struct BatchKey
    {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;

        bool operator==(const RenderBatchManager::BatchKey& other) const;
    };

    struct BatchKeyHasher
    {
        size_t operator()(const BatchKey& key) const
        {
            size_t h1 = std::hash<Mesh*>()(key.mesh.get());
            size_t h2 = std::hash<Material*>()(key.material.get());
            return h1 ^ (h2 << 1);
        }
    };

    class RenderBatch {
    private:
        BatchKey batchKey;
        std::vector<RenderInstance*> instances;
        std::vector<InstanceData> instancesData;
    public:
        explicit RenderBatch(
            BatchKey batchKey
        );

        ~RenderBatch();

        RenderBatch(const RenderBatch& other) = delete;
        RenderBatch& operator=(const RenderBatch& other) = delete;

        RenderBatch(RenderBatch&& other) noexcept;
        RenderBatch& operator=(RenderBatch&& other) noexcept;

        void addInstance(
            RenderInstance* instance
        );

        void removeInstance(
            RenderInstance* instance
        );

        bool empty();

        const BatchKey& getKey() const { return batchKey; }

        bool isEquivalent(
            const std::shared_ptr<Mesh>& mesh,
            const std::shared_ptr<Material>& material
        ) const;

        const std::vector<RenderInstance*>& getRenderInstance() const{ return instances; }
        std::vector<InstanceData>& getinstancesData() { return instancesData; }
        const std::vector<InstanceData>& getinstancesData() const { return instancesData; }
    };

private:

    std::unordered_map<BatchKey, std::unique_ptr<RenderBatch>, BatchKeyHasher> batches;
    ResourceManager* resourceManager;

public:
    void addInstance(
        const BatchKey& batchKey,
        RenderInstance* instance
    );

    bool removeInstance(
        RenderInstance* instance
    );

    bool moveInstance(
        const BatchKey& newBatchKey,
        RenderInstance* instance
    );

    void findBatchKey(
        const std::string& meshPath,
        const std::string& texturePath,
        BatchKey& key
    );

    RenderBatchManager::BatchKey findBatchKey(
        const std::string& meshPath,
        const std::string& texturePath
    );

    template<typename Func> void forEachBatch(Func&& func)
    {
        for (auto& [key, batch] : batches)
        {
            func(*batch);
        }
    }

    RenderBatchManager(ResourceManager* resourceManager);
    ~RenderBatchManager() = default;
};