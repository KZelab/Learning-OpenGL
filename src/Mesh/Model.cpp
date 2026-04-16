#include "Model.h"

#include <iostream>
#include <stdexcept>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// Note: stb_image is NOT included or defined here.  Texture.cpp already
// defines STB_IMAGE_IMPLEMENTATION (or equivalent) and handles all image
// loading.  Including it again in this translation unit would cause a
// duplicate symbol linker error.

// ============================================================================
// Constructor
// ============================================================================

Model::Model(const std::string& path, bool flipUVs)
{
    loadModel(path, flipUVs);
}

// ============================================================================
// Draw
// ============================================================================

void Model::Draw(Shader& shader)
{
    for (ModelMesh& mesh : m_Meshes)
        mesh.Draw(shader);
}

// ============================================================================
// Transform helpers
// ============================================================================

void Model::setPosition(const glm::vec3& position)
{
    m_Position = position;
    for (ModelMesh& mesh : m_Meshes)
        mesh.setPosition(position);
}

void Model::setRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    for (ModelMesh& mesh : m_Meshes)
        mesh.setRotation(rotation);
}

void Model::setScale(const glm::vec3& scale)
{
    m_Scale = scale;
    for (ModelMesh& mesh : m_Meshes)
        mesh.setScale(scale);
}

glm::mat4 Model::getTransformMatrix() const
{
    glm::mat4 transform = glm::mat4(1.0f);

    transform = glm::translate(transform, m_Position);

    transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    transform = glm::scale(transform, m_Scale);

    return transform;
}

// ============================================================================
// loadModel
// ============================================================================

void Model::loadModel(const std::string& path, bool flipUVs)
{
    Assimp::Importer importer;

    // Post-processing flags:
    //   aiProcess_Triangulate      - Convert all polygons to triangles.
    //   aiProcess_GenSmoothNormals - Generate per-vertex normals for meshes
    //                                that do not already have them.
    //   aiProcess_CalcTangentSpace - Populate tangent/bitangent vectors,
    //                                useful if normal mapping is added later.
    //
    // aiProcess_FlipUVs is intentionally omitted by default because
    // Texture::Texture() calls stbi_set_flip_vertically_on_load(1), which
    // already corrects the V coordinate at image-decode time.  See the
    // Model.h header comment for a full explanation.
    unsigned int flags = aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_PreTransformVertices;

    if (flipUVs)
        flags |= aiProcess_FlipUVs;

    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cerr << "Model::loadModel() - Assimp error: "
            << importer.GetErrorString() << "\n";
        throw std::runtime_error("Failed to load model: " + path);
    }

    m_Directory = path.substr(0, path.find_last_of("/\\"));

    processNode(scene->mRootNode, scene);

    std::cout << "Model::loadModel() - loaded \"" << path
        << "\": " << m_Meshes.size() << " mesh(es).\n";
}

// ============================================================================
// processNode
// ============================================================================
//
// Assimp stores geometry in a tree of aiNode objects.  Each node holds indices
// into aiScene::mMeshes rather than the mesh data directly.  The full tree is
// traversed recursively so that geometry nested in child nodes (common in FBX
// files) is not missed.
// ============================================================================

void Model::processNode(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

// ============================================================================
// processMesh
// ============================================================================
//
// Converts a single aiMesh into a ModelMesh by:
//   1. Copying vertex attributes into our Vertex layout.
//   2. Flattening face index lists into a flat unsigned int vector.
//   3. Loading textures from the referenced aiMaterial via loadMaterialTextures.
// ============================================================================

ModelMesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture>  textures;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    // ------------------------------------------------------------------
    // Vertices
    // ------------------------------------------------------------------
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        // Use the full constructor so every field is explicitly set.
        // Assimp aiVector3D members map directly to our float array positions.

        float nx = 0.0f, ny = 0.0f, nz = 1.0f;
        if (mesh->HasNormals())
        {
            nx = mesh->mNormals[i].x;
            ny = mesh->mNormals[i].y;
            nz = mesh->mNormals[i].z;
        }

        float r = 1.0f, g = 1.0f, b = 1.0f;
        if (mesh->mColors[0])
        {
            r = mesh->mColors[0][i].r;
            g = mesh->mColors[0][i].g;
            b = mesh->mColors[0][i].b;
        }

        float u = 0.0f, v = 0.0f;
        if (mesh->mTextureCoords[0])
        {
            u = mesh->mTextureCoords[0][i].x;
            v = mesh->mTextureCoords[0][i].y;
        }

		float tx = 1.0f, ty = 0.0f, tz = 0.0f; //add this to support gltf tangent space normal maps, 
        //which require tangents and bitangents in addition to normals.  
        // Assimp's aiProcess_CalcTangentSpace populates the mTangents array, so we check for its presence here.
        if (mesh->mTangents)
        {
            tx = mesh->mTangents[i].x;
            ty = mesh->mTangents[i].y;
            tz = mesh->mTangents[i].z;
        }

        vertices.emplace_back(
            mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,
            nx, ny, nz,
            r, g, b,
            u, v,
            tx, ty, tz
        );
    }

    // ------------------------------------------------------------------
    // Indices
    // ------------------------------------------------------------------
    // After aiProcess_Triangulate every face has exactly 3 indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // ------------------------------------------------------------------
    // Textures
    // ------------------------------------------------------------------
    if (mesh->mMaterialIndex >= 0)
    {
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<MeshTexture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

		if (diffuseMaps.empty()) // Some models (e.g. glTF) use aiTextureType_BASE_COLOR instead of aiTextureType_DIFFUSE for the main color texture.
            diffuseMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<MeshTexture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<MeshTexture> normalMaps =
            loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    return ModelMesh(vertices, indices, textures);
}

// ============================================================================
// loadMaterialTextures
// ============================================================================
//
// Queries an aiMaterial for all textures of a given aiTextureType.  For each
// one it checks the cache first; if the image has not been seen before it
// constructs a Texture object (which uploads to the GPU) and stores it in the
// cache.  Returns a vector of MeshTexture descriptors for the caller to attach
// to a ModelMesh.
// ============================================================================

std::vector<MeshTexture> Model::loadMaterialTextures(const aiMaterial* material,
    aiTextureType     type,
    const std::string& typeName)
{
    std::vector<MeshTexture> textures;

    unsigned int textureCount = material->GetTextureCount(type);

    for (unsigned int i = 0; i < textureCount; ++i)
    {
        aiString relativePath;
        material->GetTexture(type, i, &relativePath);

        std::string fullPath = m_Directory + "/" + relativePath.C_Str();

        // Return cached entry if this image has already been uploaded.
        auto it = m_TexturesLoaded.find(fullPath);
        if (it != m_TexturesLoaded.end())
        {
            MeshTexture meshTexture;
            meshTexture.texture = it->second;
            meshTexture.type = typeName;
            meshTexture.path = fullPath;
            textures.push_back(meshTexture);
            continue;
        }

        // Not cached: construct a Texture, which loads the image via stb_image
        // and uploads it to the GPU.
        MeshTexture meshTexture;
        meshTexture.texture = std::make_shared<Texture>(fullPath);
        meshTexture.type = typeName;
        meshTexture.path = fullPath;

        textures.push_back(meshTexture);
        m_TexturesLoaded[fullPath] = meshTexture.texture;
    }

    return textures;
}