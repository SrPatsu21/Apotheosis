#include "SkinnedMesh.hpp"

#include "../animations/skeleton/SkeletonLoader.hpp"
#include "../animations/AnimationLoader.hpp"

SkinnedMesh::SkinnedMesh(
    const std::string& path,
    VkDevice device,
    BufferManager* bufferManager
)
{
    skeleton =
        std::make_shared<Skeleton>(
            SkeletonLoader::loadSkeletonFromGLTF(path)
        );

    animations =
        AnimationLoader::loadFromGLTF(
            path,
            skeleton.get()
        );

    std::vector<SkinnedVertex> vertices;
    std::vector<uint32_t> indices;

    load(
        path,
        skeleton.get(),
        vertices,
        indices
    );

    skinnedVertexBufferManager =
        std::make_unique<SkinnedVertexBufferManager>(
            device,
            bufferManager,
            vertices
        );

    indexBufferManager =
        std::make_unique<IndexBufferManager>(
            device,
            bufferManager,
            indices
        );

    indexCount =
        static_cast<uint32_t>(indices.size());
}
void SkinnedMesh::load(
    const std::string& path,
    const Skeleton* skeleton,
    std::vector<SkinnedVertex>& vertices,
    std::vector<uint32_t>& indices
) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
        throw std::runtime_error(importer.GetErrorString());
    }

    vertices.clear();
    indices.clear();
    subMeshes.clear();
    materials.clear();

    std::filesystem::path modelPath(path);
    std::filesystem::path directory = modelPath.parent_path();

    // MATERIAIS
    materials.resize(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* mat = scene->mMaterials[i];
        MaterialData material{};

        aiString pathStr;

        // Base Color / Diffuse
        if (mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
        {
            mat->GetTexture(aiTextureType_BASE_COLOR, 0, &pathStr);
            material.baseColorPath = (directory / pathStr.C_Str()).string();
        }
        else if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &pathStr);
            material.baseColorPath = (directory / pathStr.C_Str()).string();
        }

        // Normal
        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            mat->GetTexture(aiTextureType_NORMALS, 0, &pathStr);
            material.normalPath = (directory / pathStr.C_Str()).string();
        }

        // Metallic ou Specular
        if (mat->GetTextureCount(aiTextureType_METALNESS) > 0)
        {
            mat->GetTexture(aiTextureType_METALNESS, 0, &pathStr);
            material.metallicRoughnessPath = (directory / pathStr.C_Str()).string();
        }
        else if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            mat->GetTexture(aiTextureType_SPECULAR, 0, &pathStr);
            material.metallicRoughnessPath = (directory / pathStr.C_Str()).string();
        }
        materials[i] = material;
    }

    // MESHES
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        uint32_t baseVertex = static_cast<uint32_t>(vertices.size());
        uint32_t indexOffset = static_cast<uint32_t>(indices.size());

        // VERTICES
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {

            glm::vec3 position{
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            };

            glm::vec3 normal{0.0f};
            if (mesh->HasNormals()) {
                normal = {
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                };
            }

            glm::vec4 tangent{0.0f};
            if (mesh->HasTangentsAndBitangents()) {

                glm::vec3 t{
                    mesh->mTangents[v].x,
                    mesh->mTangents[v].y,
                    mesh->mTangents[v].z
                };
                glm::vec3 b{
                    mesh->mBitangents[v].x,
                    mesh->mBitangents[v].y,
                    mesh->mBitangents[v].z
                };

                float handedness =
                    (glm::dot(glm::cross(normal, t), b) < 0.0f)
                        ? -1.0f
                        : 1.0f;

                tangent = glm::vec4(t, handedness);
            }

            glm::vec2 texCoord{0.0f};
            if (mesh->HasTextureCoords(0)) {
                texCoord = {
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                };
            }

            vertices.emplace_back(
                position,
                normal,
                tangent,
                texCoord,
                glm::uvec4(0),
                glm::vec4(0.0f)
            );
        }

        // BONES
        for (unsigned int b = 0; b < mesh->mNumBones; b++)
        {
            aiBone* aiBone =
                mesh->mBones[b];

            // * could be just [b], its is the same
            uint32_t boneIndex = skeleton->findBoneIndex(aiBone->mName.C_Str());

            for (unsigned int w = 0;
                w < aiBone->mNumWeights;
                w++)
            {
                const aiVertexWeight& vw =
                    aiBone->mWeights[w];

                SkinnedVertex& vertex =
                    vertices[
                        baseVertex +
                        vw.mVertexId
                    ];

                for (int i = 0; i < 4; i++)
                {
                    if (vertex.boneWeights[i] == 0.0f)
                    {
                        vertex.boneIndices[i] = boneIndex;

                        vertex.boneWeights[i] = vw.mWeight;

                        break;
                    }
                }
            }
        }
        // BONE_WEIGHTS
        for (uint32_t i = baseVertex;
            i < vertices.size();
            i++)
        {
            float sum =
                vertices[i].boneWeights.x +
                vertices[i].boneWeights.y +
                vertices[i].boneWeights.z +
                vertices[i].boneWeights.w;

            if (sum > 0.0f)
            {
                vertices[i].boneWeights /= sum;
            }
        }

        // INDICES
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];

            indices.emplace_back(baseVertex + face.mIndices[0]);
            indices.emplace_back(baseVertex + face.mIndices[1]);
            indices.emplace_back(baseVertex + face.mIndices[2]);
        }

        SubMesh sub{};
        sub.firstIndex = indexOffset;
        sub.indexCount = mesh->mNumFaces * 3;
        sub.vertexOffset = baseVertex;
        sub.materialIndex = mesh->mMaterialIndex;

        subMeshes.push_back(sub);
    }
}
