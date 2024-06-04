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

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


std::string project_locate_ligid_file(std::string folderPath)
{
    if(!std::filesystem::is_directory(folderPath) || !std::filesystem::exists(folderPath)){
        LGDLOG::start << "ERROR : Locating Ligid file in a folder : " << folderPath << " is not a folder!"  << LGDLOG::end;
        return "";
    }

    const std::string extension = ".ligid";
    std::vector<std::string> matchingFiles;

    // Get all the files with a ligid extension
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
            {
                matchingFiles.push_back(entry.path().string());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 789215 " << ex.what() << LGDLOG::end;
    }    

    // Can't find any ligid files
    if (matchingFiles.empty())
    {
        return "";
    }

    // Ligid files found
    else
    {
        // If multiple ligid files found
        if(matchingFiles.size() > 1)
            LGDLOG::start << "WARNING : Multiple ligid files detected!" << LGDLOG::end;
        
        // Return the first matching file path
        return matchingFiles[0];
    }
}

std::string project_locate_ligid_file(){
    return project_locate_ligid_file(project_path());
}

std::string project_recover_path(int slot){
    return project_path() + UTIL::folderDistinguisher() + "Recover" + UTIL::folderDistinguisher() + std::to_string(slot) + UTIL::folderDistinguisher();
}

void project_add_model(std::string filePath){
    if(!projectUTIL_folder_path_check()){
        LGDLOG::start<< "ERROR : CAN'T ADD MODEL TO PROJECT : Project path is not valid : " << project_path() << LGDLOG::end;
        return;
    }

    Model tdModel;
    bool success = tdModel.loadModel(filePath, true, false);
    
    if(!success){
        LGDLOG::start << "ERROR : Can't add the 3D model to the library. Failed to read the 3D model file : " << filePath << LGDLOG::end;
        return;
    }
    
    if(!tdModel.meshes.size()){
        LGDLOG::start << "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << LGDLOG::end;
        return;
    }
   
    Library::addModel(tdModel);

    if(Library::getModelArraySize())
        tdModel = Library::getModelObj(Library::getModelArraySize() - 1);

    const std::string originalThreedModelsFolderPath = project_path() + UTIL::folderDistinguisher() + "Original_3D_Model_Files"; 
    if((UTIL::createFolderIfDoesntExist(originalThreedModelsFolderPath))){
        
        UTIL::copyFileToFolder(filePath, originalThreedModelsFolderPath, 1);
    }
    
    FileHandler::writeLGDMODELFile(project_path() + UTIL::folderDistinguisher() + "3DModels", tdModel);
}   