#pragma once

#include "Animation.hpp"
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "skeleton/Skeleton.hpp"
#include <unordered_map>

class AnimationLoader
{
public:

    static std::vector<Animation> loadFromGLTF(
        const std::string& path,
        const Skeleton* skeleton
    );
};
