#include "RenderBatchManager.hpp"
#include "mesh/Mesh.hpp"
#include "instance/RenderInstance.hpp"

#include <algorithm>

// ========================
// BatchKey
// ========================

bool RenderBatchManager::BatchKey::operator==(
    const RenderBatchManager::BatchKey& other
) const {
    return mesh == other.mesh && submesh == other.submesh;
}

bool RenderBatchManager::BatchKey::operator<(
    const RenderBatchManager::BatchKey& other
) const {
    if (mesh.get() != other.mesh.get())
        return mesh.get() < other.mesh.get();

    return submesh < other.submesh;
}

// ========================
// RenderBatch
// ========================

RenderBatchManager::RenderBatch::RenderBatch(
    BatchKey batchKey
)
    : batchKey(batchKey)
{}

RenderBatchManager::RenderBatch::RenderBatch(
    RenderBatch&& other
) noexcept
    : batchKey(std::move(other.batchKey)),
      batchRegistrations(std::move(other.batchRegistrations)),
      instancesData(std::move(other.instancesData))
{}

RenderBatchManager::RenderBatch&
RenderBatchManager::RenderBatch::operator=(
    RenderBatch&& other
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

RenderBatchManager::RenderBatch::~RenderBatch() = default;

void RenderBatchManager::RenderBatch::addInstance(
    RenderInstance* instance
)
{
    size_t index = instancesData.size();

    instancesData.emplace_back();
    batchRegistrations.push_back(&instance->batchRegistration);

    instance->batchRegistration.batch = this;
    instance->batchRegistration.indexInBatch = index;

    instance->updateModelMatrix();
}

void RenderBatchManager::RenderBatch::removeInstance(
    RenderInstance* instance
)
{
    auto& reg = instance->batchRegistration;

    size_t index = reg.indexInBatch;
    size_t lastIndex = batchRegistrations.size() - 1;

    if (index != lastIndex)
    {
        batchRegistrations[index] = batchRegistrations[lastIndex];
        batchRegistrations[index]->indexInBatch = index;

        instancesData[index] = instancesData[lastIndex];
    }

    batchRegistrations.pop_back();
    instancesData.pop_back();

    reg.batch = nullptr;
}

bool RenderBatchManager::RenderBatch::empty()
{
    return batchRegistrations.empty();
}

// ========================
// RenderBatchManager
// ========================

RenderBatchManager::RenderBatchManager(
    ResourceManager* resourceManager
)
    : resourceManager(resourceManager)
{}

void RenderBatchManager::addInstance(
    std::shared_ptr<Mesh> mesh,
    RenderInstance* instance
)
{
    for (auto subm : mesh->getSubMeshes())
    {
        BatchKey key = {mesh, &subm};
        auto it = batches_map.find({});
        instance->addRegistration();
    }
    
    
    // auto it = batches_map.find(key);

    // if (it != batches_map.end())
    // {
    //     it->second->addInstance(instance);
    // }
    // else
    // {
    //     auto batch = std::make_unique<RenderBatch>(key);
    //     auto* batchPtr = batch.get();

    //     batches_map.emplace(key, std::move(batch));

    //     batchPtr->addInstance(instance);

    //     batches_dirty = true;
    // }
}

bool RenderBatchManager::removeInstance(
    RenderInstance* instance
)
{
    auto* batch = instance->batchRegistration.batch;

    if (!batch)
        return false;

    BatchKey key = batch->getKey();

    batch->removeInstance(instance);

    if (batch->empty())
    {
        batches_map.erase(key);
        batches_dirty = true;
    }

    return true;
}

bool RenderBatchManager::moveInstance(
    const BatchKey& newKey,
    RenderInstance* instance
)
{
    if (removeInstance(instance))
    {
        addInstance(newKey, instance);
        return true;
    }

    return false;
}

void RenderBatchManager::rebuildSortedBatches()
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
        [](RenderBatch* a, RenderBatch* b)
        {
            return a->getKey() < b->getKey();
        }
    );

    batches_dirty = false;
}

// ========================
// BatchKey helpers
// ========================

void RenderBatchManager::findBatchKey(
    const std::string& meshPath,
    uint32_t submeshIndex,
    BatchKey& key
)
{
    key.mesh = resourceManager->getMesh(meshPath);
    key.submesh = &key.mesh->getSubMeshes()[submeshIndex];
}

RenderBatchManager::BatchKey
RenderBatchManager::findBatchKey(
    const std::string& meshPath,
    uint32_t submeshIndex
)
{
    BatchKey key;
    key.mesh = resourceManager->getMesh(meshPath);
    auto a = &key.mesh->getSubMeshes()[submeshIndex];
    key.submesh = &key.mesh->getSubMeshes()[submeshIndex];
    return key;
}
