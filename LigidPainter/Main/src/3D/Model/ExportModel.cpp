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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

}

