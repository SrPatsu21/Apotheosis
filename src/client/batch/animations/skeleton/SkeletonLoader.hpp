#pragma once

#include "Skeleton.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>
#include <stdexcept>


#include <string>

class SkeletonLoader
{
public:

    static Skeleton loadSkeletonFromGLTF(
        const std::string& path
    );
};