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
#include <assimp/Exporter.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

#include "3D/Model/Model.hpp"

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
    if(!std::filesystem::is_directory(path)){
        LGDLOG::start << "ERROR : Exporting 3D model : path is not a folder : " << path << LGDLOG::end;
    }

    // Create an Assimp scene
    aiScene scene;
    scene.mRootNode = new aiNode;
    scene.mRootNode->mName = "root"; // Set the name of the root node.

    // Create an Assimp mesh for each Mesh in the vector
    scene.mNumMeshes = static_cast<unsigned int>(meshes.size());
    scene.mMeshes = new aiMesh*[scene.mNumMeshes];

    for (size_t i = 0; i < meshes.size(); i++) {
        aiMesh* assimpMesh = new aiMesh;
        assimpMesh->mName = "mesh_" + std::to_string(i); // Set the name of the mesh.

        // Fill in vertex data, normals, UVs, and faces from the current Mesh object
        assimpMesh->mNumVertices = meshes[i].vertices.size();
        assimpMesh->mVertices = new aiVector3D[assimpMesh->mNumVertices];

        for (unsigned int j = 0; j < meshes[i].vertices.size(); j++) {
            assimpMesh->mVertices[j] = aiVector3D(meshes[i].vertices[j].Position.x, meshes[i].vertices[j].Position.y, meshes[i].vertices[j].Position.z);
        }

        // Add the mesh to the scene
        scene.mMeshes[i] = assimpMesh;
    }

    // Export the scene to an .obj file
    Assimp::Exporter exporter;
    exporter.Export(&scene, "obj", path + UTIL::folderDistinguisher() + this->title + ".obj", aiProcess_Triangulate); // Triangulate is typically required for .obj format.

    // Clean up
    //for (size_t i = 0; i < meshes.size(); i++) {
    //    delete[] scene.mMeshes[i]->mVertices;
    //    delete scene.mMeshes[i];
    //}
    //delete[] scene.mMeshes;
    //delete scene.mRootNode;
}