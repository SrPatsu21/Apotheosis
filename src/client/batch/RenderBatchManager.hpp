#pragma once

#include "ResourceManager.hpp"

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
        std::vector<std::shared_ptr<RenderInstance>> instances;
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
            const std::shared_ptr<RenderInstance>& instance
        );

        void removeInstance(
            const std::shared_ptr<RenderInstance>& instance
        );

        bool empty();

        BatchKey getKey() const { return batchKey; }

        bool isEquivalent(
            const std::shared_ptr<Mesh>& mesh,
            const std::shared_ptr<Material>& material
        ) const;

        const std::vector<std::shared_ptr<RenderInstance>>& getInstances() const{ return instances; }
    };

private:

    std::unordered_map<BatchKey, std::unique_ptr<RenderBatch>, BatchKeyHasher> batches;
    std::unique_ptr<ResourceManager> resourceManager;

public:
    void addInstance(
        const BatchKey& batchKey,
        std::shared_ptr<RenderInstance>& instance
    );

    bool removeInstance(
        std::shared_ptr<RenderInstance>& instance
    );

    bool moveInstance(
        const BatchKey& newBatchKey,
        std::shared_ptr<RenderInstance>& instance
    );

    RenderBatchManager::BatchKey findBatchKey(
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
public:

    RenderBatchManager() = default;
    ~RenderBatchManager() = default;
};