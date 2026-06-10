#include "RenderSkinnedBatchManager.hpp"
#include "mesh/SkinnedMesh.hpp"
#include "instance/SkinnedRenderInstance.hpp"
#include "instance/InstanceData.hpp"

#include <algorithm>

// ========================
// BatchKey
// ========================

bool RenderSkinnedBatchManager::BatchKey::operator==(
    const RenderSkinnedBatchManager::BatchKey& other
) const {
    return material == other.material && submesh == other.submesh && mesh == other.mesh && pipelineFlags == other.pipelineFlags;
}

bool RenderSkinnedBatchManager::BatchKey::operator<(
    const RenderSkinnedBatchManager::BatchKey& other
) const {
    if (pipelineFlags != other.pipelineFlags)
        return pipelineFlags < other.pipelineFlags;

    if (material.get() != other.material.get())
        return material.get() < other.material.get();

    if (mesh.get() != other.mesh.get())
        return mesh.get() < other.mesh.get();

    return submesh < other.submesh;
}

// ========================
// RenderSkinnedBatch
// ========================

RenderSkinnedBatch::RenderSkinnedBatch(
    RenderSkinnedBatchManager::BatchKey batchKey
)
    : batchKey(batchKey)
{}

RenderSkinnedBatch::RenderSkinnedBatch(
    RenderSkinnedBatch&& other
) noexcept :
    batchKey(std::move(other.batchKey)),
    batchRegistrations(std::move(other.batchRegistrations)),
    instancesData(std::move(other.instancesData))
{}

RenderSkinnedBatch&
RenderSkinnedBatch::operator=(
    RenderSkinnedBatch&& other
) noexcept
{
    if (this != &other)
    {
        batchKey = std::move(other.batchKey);
        batchRegistrations = std::move(other.batchRegistrations);
        instancesData = std::move(other.instancesData);
    }
    return *this;
}

RenderSkinnedBatch::~RenderSkinnedBatch() = default;

void RenderSkinnedBatch::addInstance(
    SkinnedRenderInstance* instance
)
{
    size_t index = instancesData.size();

    instancesData.emplace_back();
    boneOffsets.push_back(0);

    instance->addRegistration(this, index);

    batchRegistrations.push_back(
        &instance->registrations.back()
    );
}

void RenderSkinnedBatch::removeInstance(
    SkinnedRenderInstance* instance,
    size_t intregistrationsIndex
) {
    auto& reg = instance->registrations[intregistrationsIndex];

    size_t index = reg.indexInBatch;
    size_t lastIndex = batchRegistrations.size() - 1;

    if (index != lastIndex)
    {
        batchRegistrations[index] =
            batchRegistrations[lastIndex];

        batchRegistrations[index]->indexInBatch =
            index;

        instancesData[index] =
            instancesData[lastIndex];

        boneOffsets[index] =
            boneOffsets[lastIndex];
    }

    batchRegistrations.pop_back();
    instancesData.pop_back();
    boneOffsets.pop_back();

    reg.batch = nullptr;
}

bool RenderSkinnedBatch::empty()
{
    return batchRegistrations.empty();
}

// ========================
// RenderSkinnedBatchManager
// ========================

RenderSkinnedBatchManager::RenderSkinnedBatchManager(
    ResourceManager* resourceManager
)
    : resourceManager(resourceManager)
{
}

void RenderSkinnedBatchManager::addInstance(
    std::shared_ptr<SkinnedMesh> mesh,
    SkinnedRenderInstance* instance
) {
    // instance->getRegistrations().reserve(mesh->getSubMeshes().size());
    const std::vector<SkinnedMesh::SubMesh>& meshs = mesh->getSubMeshes();

    for (size_t i = 0; i < meshs.size(); i++)
    {
        BatchKey key = {
            mesh,
            &meshs[i],
            resourceManager->getMaterialForSubMesh(*mesh.get(), meshs[i]),
            GraphicsPipeline::PIPE_TOPO_TRIANGLES | GraphicsPipeline::PIPE_CULL_BACK | GraphicsPipeline::PIPE_DEPTH_TEST | GraphicsPipeline::PIPE_DEPTH_WRITE | GraphicsPipeline::PIPE_BLEND
        };

        auto it = batches_map.find(key);
        if (it != batches_map.end())
        {
            it->second->addInstance(
                instance
            );
        }
        else{
            auto batch = std::make_unique<RenderSkinnedBatch>(key);
            auto* batchPtr = batch.get();

            batches_map.emplace(key, std::move(batch));

            batchPtr->addInstance(
                instance
            );

            batches_dirty = true;
        }
    }
}

bool RenderSkinnedBatchManager::removeInstance(
    SkinnedRenderInstance* instance
)
{
    auto registrations = instance->getRegistrations();
    for (size_t i = 0; i < registrations.size(); i++)
    {
        auto* batch = registrations[i].batch;

        if (!batch)
            return false;

        BatchKey key = batch->getKey();

        batch->removeInstance(
            instance,
            registrations[i].indexInBatch
        );

        if (batch->empty())
        {
            batches_map.erase(key);
            batches_dirty = true;
        }
    }
    return true;

}

void RenderSkinnedBatchManager::rebuildSortedBatches()
{
    if (!batches_dirty)
        return;

    batches_sorted.clear();
    batches_sorted.reserve(batches_map.size());

    for (auto& [key, batch] : batches_map)
        batches_sorted.push_back(batch.get());

    std::sort(
        batches_sorted.begin(),
        batches_sorted.end(),
        [](RenderSkinnedBatch* a, RenderSkinnedBatch* b)
        {
            return a->getKey() < b->getKey();
        }
    );

    batches_dirty = false;
}

// ========================
// BatchKey helpers
// ========================

void RenderSkinnedBatchManager::findBatchKey(
    const std::string& meshPath,
    uint32_t submeshIndex,
    BatchKey& key
)
{
    key.mesh = resourceManager->getMesh(meshPath);
    key.submesh = &key.mesh->getSubMeshes()[submeshIndex];
    key.material = resourceManager->getMaterialForSubMesh(*key.mesh.get(), *key.submesh);
    key.pipelineFlags =
        GraphicsPipeline::PIPE_TOPO_TRIANGLES |
        GraphicsPipeline::PIPE_CULL_BACK |
        GraphicsPipeline::PIPE_DEPTH_TEST |
        GraphicsPipeline::PIPE_DEPTH_WRITE |
        GraphicsPipeline::PIPE_BLEND;
}

RenderSkinnedBatchManager::BatchKey
RenderSkinnedBatchManager::findBatchKey(
    const std::string& meshPath,
    uint32_t submeshIndex
)
{
    BatchKey key;
    key.mesh = resourceManager->getMesh(meshPath);
    key.submesh = &key.mesh->getSubMeshes()[submeshIndex];
    key.material = resourceManager->getMaterialForSubMesh(*key.mesh.get(), *key.submesh);
    key.pipelineFlags =
        GraphicsPipeline::PIPE_TOPO_TRIANGLES |
        GraphicsPipeline::PIPE_CULL_BACK |
        GraphicsPipeline::PIPE_DEPTH_TEST |
        GraphicsPipeline::PIPE_DEPTH_WRITE |
        GraphicsPipeline::PIPE_BLEND;
    return key;
}