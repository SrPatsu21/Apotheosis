#include "Mesh.hpp"

void Mesh::load(
    const std::string& path,
    std::vector<Vertex>& vertices,
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

    // MATERIAIS
    materials.resize(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* mat = scene->mMaterials[i];
        MaterialData material{};

        aiString pathStr;

        if (mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
            mat->GetTexture(aiTextureType_BASE_COLOR, 0, &pathStr);
            material.baseColorPath = pathStr.C_Str();
        }

        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
            mat->GetTexture(aiTextureType_NORMALS, 0, &pathStr);
            material.normalPath = pathStr.C_Str();
        }

        if (mat->GetTextureCount(aiTextureType_METALNESS) > 0) {
            mat->GetTexture(aiTextureType_METALNESS, 0, &pathStr);
            material.metallicRoughnessPath = pathStr.C_Str();
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

            glm::vec3 normal = mesh->HasNormals()
                ? glm::vec3{
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                }
                : glm::vec3{0.0f};

            glm::vec4 tangent = mesh->HasTangentsAndBitangents()
                ? glm::vec4{
                    mesh->mTangents[v].x,
                    mesh->mTangents[v].y,
                    mesh->mTangents[v].z,
                    1.0f // handedness simplificada
                }
                : glm::vec4{0.0f};

            glm::vec2 texCoord = mesh->mTextureCoords[0]
                ? glm::vec2{
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                }
                : glm::vec2{0.0f};

            vertices.emplace_back(position, normal, tangent, texCoord);
        }

        // INDICES
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];

            indices.emplace_back(baseVertex + face.mIndices[0]);
            indices.emplace_back(baseVertex + face.mIndices[1]);
            indices.emplace_back(baseVertex + face.mIndices[2]);
        }

        SubMesh sub{};
        sub.firstIndex   = indexOffset;
        sub.indexCount   = mesh->mNumFaces * 3;
        sub.vertexOffset = 0;
        sub.materialIndex = mesh->mMaterialIndex;

        subMeshes.push_back(sub);
    }
}

Mesh::Mesh(
    const std::string& path,
    VkDevice device,
    BufferManager* bufferManager
) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    load(
        path,
        vertices,
        indices
    );

    vertexBufferManager = std::make_unique<VertexBufferManager>(device, bufferManager, vertices);
    indexBufferManager  = std::make_unique<IndexBufferManager>(device, bufferManager, indices);

    indexCount = static_cast<uint32_t>(indices.size());
}