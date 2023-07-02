/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------





    ! THIS SECTION & MODEL.EXPORT IS COMPLETELY USELESS USE THE FILEHANDLER INSTEAD!!!!!!!!!!!!!!!S
    





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

// dae
// collada
// 0
// X Files
// x
// x
// 1
// Step Files
// stp
// stp
// 2
// Wavefront OBJ format
// obj
// obj
// 3
// Wavefront OBJ format without material file
// obj
// objnomtl
// 4
// Stereolithography
// stl
// stl
// 5
// Stereolithography (binary)
// stl
// stlb
// 6
// Stanford Polygon Library
// ply
// ply
// 7
// Stanford Polygon Library (binary)
// ply
// plyb
// 8
// Autodesk 3DS (legacy)
// 3ds
// 3ds
// 9
// GL Transmission Format v. 2
// gltf
// gltf2
// 10
// GL Transmission Format v. 2 (binary)
// glb
// glb2
// 11
// GL Transmission Format
// gltf
// gltf
// 12
// GL Transmission Format (binary)
// glb
// glb
// 13
// Assimp Binary File
// assbin
// assbin
// 14
// Assimp XML Document
// assxml
// assxml
// 15
// Extensible 3D
// x3d
// x3d
// 16
// Autodesk FBX (binary)
// fbx
// fbx
// 17
// Autodesk FBX (ascii)
// fbx
// fbxa
// 18
// The 3MF-File-Format
// 3mf
// 3mf
// 19
// pbrt-v4 scene description file
// pbrt
// pbrt
// 20
// Assimp JSON Document
// json
// assjson
// 21

/// @brief Exports the 3D model in fbx format
/// @param path folder path
void Model::exportModel(std::string path){

    //Remove the / at the end of the path if there are 
    if(path[path.size()-1] == '/' || path[path.size()-1] == '\\')
        path.pop_back();

    // Create an instance of the Assimp Exporter
    Assimp::Exporter exporter;

    // Set the export format to FBX
    const aiExportFormatDesc* exportFormat = exporter.GetExportFormatDescription(3);
    
    if (!exportFormat) {
        // FBX format is not available, handle the error
        return;
    }

    //Full exporting path with the file name
    std::string destination = path + UTIL::folderDistinguisher() + this->title + ".obj";

    //Create the scene
    aiScene* scene = createAssimpScene();

    // Export the scene to FBX format
    exporter.Export(scene, exportFormat->id, destination);

}


/// IS NOT USED & DOESN'T WORK PROPERLY USE this-> scene

aiScene* Model::createAssimpScene() {
    // Create an instance of aiScene
    aiScene* scene = new aiScene();

    // Create an array of aiMesh pointers
    scene->mMeshes = new aiMesh*[1];
    scene->mNumMeshes = 1;

    // Create an instance of aiMesh
    aiMesh* mesh = new aiMesh();
    scene->mMeshes[0] = mesh;

    // Create three vertices for the triangle
    aiVector3D vertices[3];
    vertices[0] = aiVector3D(-1.0f, -1.0f, 0.0f);
    vertices[1] = aiVector3D(1.0f, -1.0f, 0.0f);
    vertices[2] = aiVector3D(0.0f, 1.0f, 0.0f);

    // Set the vertex positions in the mesh
    mesh->mVertices = new aiVector3D[3];
    std::copy(vertices, vertices + 3, mesh->mVertices);
    mesh->mNumVertices = 3;

    // Set the faces in the mesh
    mesh->mFaces = new aiFace[1];
    mesh->mFaces[0].mIndices = new unsigned int[3];
    mesh->mFaces[0].mIndices[0] = 0;
    mesh->mFaces[0].mIndices[1] = 1;
    mesh->mFaces[0].mIndices[2] = 2;
    mesh->mFaces[0].mNumIndices = 3;

    // Set the scene root node
    scene->mRootNode = new aiNode();
    scene->mRootNode->mMeshes = new unsigned int[1];
    scene->mRootNode->mMeshes[0] = 0;
    scene->mRootNode->mNumMeshes = 1;

    // Update scene metadata
    scene->mNumMaterials = 0;
    scene->mNumAnimations = 0;
    scene->mNumTextures = 0;
    scene->mNumLights = 0;
    scene->mNumCameras = 0;

    return scene;
}