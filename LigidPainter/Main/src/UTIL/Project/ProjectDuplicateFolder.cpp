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

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


void Project::duplicateFolder(std::string dstPath)
{

    if(dstPath == ""){
        dstPath = showFileSystemObjectSelectionDialog("Select a folder to duplicate the project folder file.", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }

    if(!dstPath.size())
        return;

    //Remove the / or \ at the end of the destination path if there are any
    if(dstPath.size()){
        if(dstPath[dstPath.size() - 1] == '/' || dstPath[dstPath.size() - 1] == '\\'){
            dstPath.pop_back();
        }
    }

    ;

    //If the destination path doesn't exists
    if(!std::filesystem::exists(dstPath)){
        std::cout << "ERROR : Can't duplicate the project folder! Destination path doesn't exists : " << dstPath << std::endl;
        return;
    }

    // DestinationPath/MyProject
    std::string dstProjectPath = dstPath + UTIL::folderDistinguisher() + this->projectName;

    //If there is already a folder with that names 
    if(std::filesystem::exists(dstProjectPath)){
        std::cout << "ERROR : Can't duplicate the project folder! There is already a folder with the same name : " << dstPath << std::endl;
        return;
    }

    //Duplicate files and folders from the original path to destination path
    for (const auto& entry : std::filesystem::recursive_directory_iterator(this->folderPath)){
        std::string file = entry.path().string();  

        std::string dst = dstProjectPath + UTIL::folderDistinguisher() + UTIL::rmvPath(this->folderPath, file);
        
        //If a folder then create a folder
        if(std::filesystem::is_directory(file)){
            
            std::filesystem::create_directories(dst);
        
        }

        //If a file then duplicate the file
        else{

            std::filesystem::copy_file(file,dst);

        }
    }

    //Update the project data
    this->folderPath = dstProjectPath;
    this->ligidFilePath = locateLigidFileInFolder(dstProjectPath);
    this->projectName = UTIL::getLastWordBySeparatingWithChar(folderPath,UTIL::folderDistinguisher());
}