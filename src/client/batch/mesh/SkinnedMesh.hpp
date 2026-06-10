#pragma once

#include "../mesh/Mesh.hpp"

#include "../animations/skeleton/Skeleton.hpp"
#include "../animations/Animation.hpp"

#include <memory>
#include <vector>

class SkinnedMesh : public Mesh
{
private:

    std::shared_ptr<Skeleton> skeleton;

    std::vector<Animation> animations;

protected:

    void SkinnedMesh::load(
        const std::string& path,
        std::vector<SkinnedVertex>& vertices,
        std::vector<uint32_t>& indices
    );

public:

    SkinnedMesh(
        const std::string& path,
        VkDevice device,
        BufferManager* bufferManager
    );

    ~SkinnedMesh() = default;

    const Skeleton* getSkeleton() const
    {
        return skeleton.get();
    }

    Skeleton* getSkeleton()
    {
        return skeleton.get();
    }

    const std::vector<Animation>& getAnimations() const
    {
        return animations;
    }

    std::vector<Animation>& getAnimations()
    {
        return animations;
    }
};