/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"

#include "GUI/GUI.hpp"

#include "3D/Model/Model.hpp"


struct AssimpObject{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string objectTitle;
    unsigned int materialIndex;

    AssimpObject(){}

    AssimpObject(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string objectTitle, unsigned int materialIndex){
        this->vertices = vertices;
        this->indices = indices;
        this->objectTitle = objectTitle;
        this->materialIndex = materialIndex;
    }
};

// Forward declerations for the utility functions
static AssimpObject processMesh(aiMesh *mesh, std::string title);
static void processNode(aiNode *node, const aiScene *scene, std::vector<AssimpObject> &meshes);
static void parseMeshData(std::vector<AssimpObject> &objects, std::vector<Mesh> &meshes, const aiScene* scene, bool initTxtrs);

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
bool Model::loadModel(std::string path, bool triangulate, bool initTxtrs)
{
    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading 3D model : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }

    UTIL::correctFolderDistinguishers(path);

    this->path = path;

    title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    title = UTIL::removeExtension(title);

    this->newModelAdded = true;
 
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        //cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return false;
    }

    std::vector<AssimpObject> assimpObjects;

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, assimpObjects);

    // Applies the objects to the this->meshes according to the material data
    parseMeshData(assimpObjects, this->meshes, scene, initTxtrs);

    this->newModelAdded = true;
    this->resize_meshes();
    
    this->generateDisplayingTexture();

    return true;
}



static AssimpObject processMesh(aiMesh *mesh, std::string title)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if(mesh->HasTangentsAndBitangents()){
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }

        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces (a face is a mesh's triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    
    // return a mesh object created from the extracted mesh data
    return AssimpObject(vertices, indices, title, mesh->mMaterialIndex);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
static void processNode(aiNode *node, const aiScene *scene, std::vector<AssimpObject> &meshes)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, node->mName.C_Str()));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshes);
    }
}

static void parseMeshData(std::vector<AssimpObject> &objects, std::vector<Mesh> &meshes, const aiScene* scene, bool initTxtrs){
    
    unsigned int biggestMatI = 0;
    for (size_t i = 0; i < objects.size(); i++)
    {
        if(biggestMatI < objects[i].materialIndex){
            biggestMatI = objects[i].materialIndex;
        }
    }

    for (size_t i = 0; i < biggestMatI + 1; i++)
    {
        if(i < meshes.size())
            meshes[i] = Mesh();
        else
            meshes.push_back(Mesh());
    }

    for (size_t i = 0; i < objects.size(); i++)
    {
        size_t prevSize = meshes[objects[i].materialIndex].indices.size(); 
        for (size_t ii = 0; ii < objects[i].indices.size(); ii++)
        {
            meshes[objects[i].materialIndex].indices.push_back(objects[i].indices[ii] + meshes[objects[i].materialIndex].vertices.size());
        }
        for (size_t ii = 0; ii < objects[i].vertices.size(); ii++)
        {
            meshes[objects[i].materialIndex].vertices.push_back(objects[i].vertices[ii]);
        }
    
        meshes[objects[i].materialIndex].materialName = scene->mMaterials[objects[i].materialIndex]->GetName().C_Str(); 
        meshes[objects[i].materialIndex].objects.push_back(MeshObject(objects[i].objectTitle, glm::ivec2(prevSize, meshes[objects[i].materialIndex].indices.size())));
    }

    if(meshes.size()){
        if(!meshes[0].vertices.size())
            meshes.erase(meshes.begin());
    }

    for (size_t i = 0; i < meshes.size(); i++)
    {
        std::vector<MeshObject> prevObjectData = meshes[i].objects; 
 
        meshes[i] = Mesh(meshes[i].vertices, meshes[i].indices, meshes[i].materialName, initTxtrs);
        meshes[i].objects = prevObjectData;
    }
}