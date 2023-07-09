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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string const &path,bool triangulate)
{
    filePath = path;
    title = UTIL::getLastWordBySeparatingWithChar(filePath,UTIL::folderDistinguisher());
    title = UTIL::removeExtension(title);

    this->newModelAdded = true;

    meshes.clear();
    // read file via ASSIMP
    // Assimp::Importer importer;

    // const aiScene* scene;
    // if(triangulate)
    //     scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_CalcTangentSpace );
    // else
    //     scene = importer.ReadFile(path, aiProcess_GenSmoothNormals  | aiProcess_CalcTangentSpace);

    // // check for errors
    // if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    // {
    //     std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    //     return;
    // }
    
    // // retrieve the directory path of the filepath
    // directory = path.substr(0, path.find_last_of('/'));

    // // process ASSIMP's root node recursively
    // processNode(scene->mRootNode, scene);
}