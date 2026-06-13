#include "SkeletonLoader.hpp"
#include <iostream>
#include <string>


static bool containsBone(
    aiNode* node,
    const std::unordered_set<std::string>& usedBones
)
{
    if (usedBones.contains(node->mName.C_Str()))
        return true;

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        if (containsBone(node->mChildren[i], usedBones))
            return true;
    }

    return false;
}

static void processNode(
    aiNode* node,
    int parent,
    Skeleton& skeleton,
    std::unordered_map<std::string, uint32_t>& boneMap,
    const std::unordered_set<std::string>& usedBones
)
{
    // ignora subárvores sem nenhum bone
    if (!containsBone(node, usedBones))
        return;

    Bone bone;

    bone.name = node->mName.C_Str();
    bone.parentIndex = parent;
    bone.inverseBindMatrix = glm::mat4(1.0f);

    uint32_t index =
        static_cast<uint32_t>(
            skeleton.bones.size()
        );

    skeleton.bones.push_back(bone);

    boneMap[bone.name] = index;

    BoneTransform bind;

    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D translation;

    node->mTransformation.Decompose(
        scaling,
        rotation,
        translation
    );

    bind.translation =
        glm::vec3(
            translation.x,
            translation.y,
            translation.z
        );

    bind.rotation =
        glm::quat(
            rotation.w,
            rotation.x,
            rotation.y,
            rotation.z
        );

    bind.scale =
        glm::vec3(
            scaling.x,
            scaling.y,
            scaling.z
        );

    skeleton.bindPose.push_back(bind);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(
            node->mChildren[i],
            index,
            skeleton,
            boneMap,
            usedBones
        );
    }
}

Skeleton SkeletonLoader::loadSkeletonFromGLTF(
    const std::string& path
)
{
    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(
            path,
            aiProcess_Triangulate
        );

    if (!scene)
    {
        throw std::runtime_error(
            importer.GetErrorString()
        );
    }

    Skeleton skeleton;

    std::unordered_map<
        std::string,
        uint32_t
    > boneMap;

    std::unordered_set<std::string> usedBones;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int b = 0; b < mesh->mNumBones; b++)
        {
            usedBones.insert(
                mesh->mBones[b]->mName.C_Str()
            );
        }
    }

    processNode(
        scene->mRootNode,
        -1,
        skeleton,
        boneMap,
        usedBones
    );


    // find rootBone
    // for (uint32_t i = 0; i < skeleton.bones.size(); i++)
    // {
    //     // std::cout << skeleton.bones[i].parentIndex << " = " << skeleton.bones[i].name << std::endl;
    //     if (skeleton.bones[i].parentIndex == -1)
    //     {
    //         skeleton.rootBoneIndex = i;
    //         break;
    //     }
    // }

    //--------------------------------------------------
    // Carrega inverse bind matrices
    //--------------------------------------------------

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh =
            scene->mMeshes[m];

        for (unsigned int b = 0; b < mesh->mNumBones; b++)
        {
            aiBone* aiBone =
                mesh->mBones[b];

            auto it =
                boneMap.find(
                    aiBone->mName.C_Str()
                );

            if (it == boneMap.end())
                continue;

            uint32_t index =
                it->second;

            const aiMatrix4x4& mat = aiBone->mOffsetMatrix;

            skeleton.bones[index].inverseBindMatrix = glm::transpose(
                    glm::mat4(
                        mat.a1, mat.a2, mat.a3, mat.a4,
                        mat.b1, mat.b2, mat.b3, mat.b4,
                        mat.c1, mat.c2, mat.c3, mat.c4,
                        mat.d1, mat.d2, mat.d3, mat.d4
                    )
                );
        }
    }

    std::cout << "==== Bones =====" << std::endl;
    for (auto &&i : skeleton.bones)
    {
        std::cout << i.name << std::endl;
    }
    std::cout << "================" << std::endl;

    return skeleton;
}