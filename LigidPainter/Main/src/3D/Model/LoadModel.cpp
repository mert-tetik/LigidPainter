/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string const &path,bool triangulate)
{
    title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    title = UTIL::removeExtension(title);

    this->newModelAdded = true;

    meshes.clear();
 
    //LGDLOG::start << "Loading : " << path;

    if(UTIL::toLowercase(UTIL::getFileExtension(path)) == "obj"){
        Model model = FileHandler::readOBJFile(path);
        this->meshes = model.meshes;
    }
    else if(UTIL::toLowercase(UTIL::getFileExtension(path)) == "fbx"){
        Model model = FileHandler::readFBXFile(path);
        this->meshes = model.meshes;
    }
    else{
        /* Print error message */
        LGDLOG::start<< 
            "ERROR : Can't read 3D model file. LigidPainter does not have a valid importer for the file format : " << 
            UTIL::toLowercase(UTIL::getFileExtension(path)) << 
        LGDLOG::end;
    }
}