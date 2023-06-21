/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

/// @brief Exports the 3D model in fbx format
/// @param Folder path 
void Model::exportModel(std::string path){

    //Remove the / at the end of the path if there are 
    if(path[path.size()-1] == '/' || path[path.size()-1] == '\\')
        path.pop_back();

    // Create an instance of the Assimp Exporter
    Assimp::Exporter exporter;

    // Set the export format to FBX
    const aiExportFormatDesc* exportFormat = exporter.GetExportFormatDescription(1);
    
    if (!exportFormat) {
        // FBX format is not available, handle the error
        return;
    }

    // Define the export file name
    std::string exportFileName = "output_file.fbx";

    //Full exporting path with the file name
    std::string destination = path + folderDistinguisher + exportFileName;

    //Create the scene
    aiScene* scene = createAssimpScene();

    // Export the scene to FBX format
    exporter.Export(scene, destination, exportFormat->id, aiProcess_Triangulate);
}

aiScene* Model::createAssimpScene() {
    // Create an instance of aiScene
    aiScene* scene = new aiScene();

    // Create an array of aiMesh pointers
    scene->mMeshes = new aiMesh*[meshes.size()];
    scene->mNumMeshes = meshes.size();

    // Convert each Mesh to Assimp-compatible structures
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        const Mesh& meshData = meshes[i];

        // Create an instance of aiMesh
        aiMesh* mesh = new aiMesh();
        scene->mMeshes[i] = mesh;

        // Convert Vertex data to Assimp-compatible structures
        std::vector<aiVector3D> positions;
        std::vector<aiVector2D> texCoords;
        std::vector<aiVector3D> normals;
        std::vector<aiVector3D> tangents;
        std::vector<aiVector3D> bitangents;
        std::vector<unsigned int> boneIDs;
        std::vector<float> weights;

        for (const auto& vertex : meshData.vertices) {
            positions.push_back(aiVector3D(vertex.Position.x, vertex.Position.y, vertex.Position.z));
            texCoords.push_back(aiVector2D(vertex.TexCoords.x, vertex.TexCoords.y));
            normals.push_back(aiVector3D(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z));
            tangents.push_back(aiVector3D(vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z));
            bitangents.push_back(aiVector3D(vertex.Bitangent.x, vertex.Bitangent.y, vertex.Bitangent.z));

            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                boneIDs.push_back(vertex.m_BoneIDs[i]);
                weights.push_back(vertex.m_Weights[i]);
            }
        }

        // Set the vertex positions in the mesh
        mesh->mVertices = new aiVector3D[positions.size()];
        std::copy(positions.begin(), positions.end(), mesh->mVertices);
        mesh->mNumVertices = positions.size();

        // Set the texture coordinates in the mesh
        mesh->mTextureCoords[0] = new aiVector3D[texCoords.size()];
        for (size_t i = 0; i < texCoords.size(); ++i) {
            mesh->mTextureCoords[0][i] = aiVector3D(texCoords[i].x, texCoords[i].y, 0.0f);
        }

        // Set the normals in the mesh
        mesh->mNormals = new aiVector3D[normals.size()];
        std::copy(normals.begin(), normals.end(), mesh->mNormals);

        // Set the tangents in the mesh
        mesh->mTangents = new aiVector3D[tangents.size()];
        std::copy(tangents.begin(), tangents.end(), mesh->mTangents);

        // Set the bitangents in the mesh
        mesh->mBitangents = new aiVector3D[bitangents.size()];
        std::copy(bitangents.begin(), bitangents.end(), mesh->mBitangents);

        // Set the bone IDs and weights in the mesh
        mesh->mBones = new aiBone*[1];
        mesh->mBones[0] = new aiBone();
        mesh->mBones[0]->mNumWeights = weights.size();
        mesh->mBones[0]->mWeights = new aiVertexWeight[weights.size()];

        for (unsigned int j = 0; j < weights.size(); ++j) {
            mesh->mBones[0]->mWeights[j].mVertexId = j;
            mesh->mBones[0]->mWeights[j].mWeight = weights[j];
        }
    }

    return scene;
}