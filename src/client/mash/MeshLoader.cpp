#include "MeshLoader.hpp"

void MeshLoader::load(const std::string& path) {
    //import model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error(importer.GetErrorString());
    }

    //mensurate and clear
    vertices.clear();
    indices.clear();
    size_t totalVertices = 0;
    size_t totalIndices  = 0;
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        totalVertices += scene->mMeshes[m]->mNumVertices;
        totalIndices  += scene->mMeshes[m]->mNumFaces * 3; // triangulado
    }
    vertices.reserve(totalVertices);
    indices.reserve(totalIndices);

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        uint32_t baseVertex = static_cast<uint32_t>(vertices.size());

        // VERTEX
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            vertices.emplace_back(Vertex{
            {
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            },
            { 1.0f, 1.0f, 1.0f, 1.0f },
            mesh->mTextureCoords[0]
                ? glm::vec2{
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                }
                : glm::vec2{ 0.0f, 0.0f }
            });
        }

        // INDEX
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];

            indices.emplace_back(baseVertex + face.mIndices[0]);
            indices.emplace_back(baseVertex + face.mIndices[1]);
            indices.emplace_back(baseVertex + face.mIndices[2]);
        }
    }
}
