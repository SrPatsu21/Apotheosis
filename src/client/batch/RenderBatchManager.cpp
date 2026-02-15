#include "RenderBatchManager.hpp"
#include "material/Material.hpp"
#include "mesh/Mesh.hpp"
#include "instance/RenderInstance.hpp"

//* BatchKey
bool RenderBatchManager::BatchKey::operator==(
    const RenderBatchManager::BatchKey& other
) const {
    return mesh == other.mesh && material == other.material;
}

//* RenderBatch
RenderBatchManager::RenderBatch::RenderBatch(
    BatchKey batchKey
) :
    batchKey(batchKey)
{}

RenderBatchManager::RenderBatch::RenderBatch(
    RenderBatch&& other
) noexcept :
    batchKey(std::move(other.batchKey)),
    instances(std::move(other.instances))
{}

RenderBatchManager::RenderBatch& RenderBatchManager::RenderBatch::operator=(
    RenderBatch&& other
) noexcept {
    if (this != &other) {
        batchKey = std::move(other.batchKey);
        instances = std::move(other.instances);
    }
    return *this;
}

bool RenderBatchManager::RenderBatch::isEquivalent(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<Material>& material
) const {
    return batchKey.mesh == mesh && batchKey.material == material;
}

void RenderBatchManager::RenderBatch::addInstance(
    RenderInstance* instance
) {

    instance->ownerBatch = this;
    instance->indexInBatch = instances.size();

    instances.push_back(instance);
}

void RenderBatchManager::RenderBatch::removeInstance(RenderInstance* instance)
{
    size_t index = instance->indexInBatch;
    size_t lastIndex = instances.size() - 1;

    if (index != lastIndex)
    {
        instances[index] = instances[lastIndex];
        instances[index]->indexInBatch = index;
    }

    instances.pop_back();

    instance->ownerBatch = nullptr;
}

RenderBatchManager::RenderBatch::~RenderBatch() {
    instances.clear();
}

bool RenderBatchManager::RenderBatch::empty() {
    return instances.empty();
}

//* RenderBatchManager
RenderBatchManager::BatchKey RenderBatchManager::findBatchKey(
    const std::string& meshPath,
    const std::string& texturePath,
    BatchKey& key
) {
    key.mesh = resourceManager->getMesh(meshPath);
    key.material = resourceManager->getMaterial(texturePath);
    return key;
}

RenderBatchManager::BatchKey RenderBatchManager::findBatchKey(
    const std::string& meshPath,
    const std::string& texturePath
) {
    BatchKey key;
    key.mesh = resourceManager->getMesh(meshPath);
    key.material = resourceManager->getMaterial(texturePath);
    return key;
}
void RenderBatchManager::addInstance(
    const BatchKey& key,
    RenderInstance* instance
) {
    auto it = batches.find(key);

    if (it != batches.end())
    {
        it->second->addInstance(instance);
    }
    else
    {
        auto batch = std::make_unique<RenderBatch>(key);
        auto* batchPtr = batch.get();
        batches.emplace(key, std::move(batch));
        batchPtr->addInstance(instance);
    }
}

bool RenderBatchManager::removeInstance(
    RenderInstance* instance
) {
    RenderBatch* batch = instance->ownerBatch;

    if (!batch)
        return false;

    batch->removeInstance(instance);

    if (batch->empty())
    {
        batches.erase(batch->getKey());
    }
    return true;
}

bool RenderBatchManager::moveInstance(
    const BatchKey& newKey,
    RenderInstance* instance
) {
    if (removeInstance(instance))
    {
        addInstance(newKey, instance);
        return true;
    }

    return false;
}

RenderBatchManager::RenderBatchManager(
    ResourceManager* resourceManager
) :
    resourceManager(resourceManager)
{

};