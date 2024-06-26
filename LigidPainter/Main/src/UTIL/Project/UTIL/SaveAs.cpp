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

#include "UTIL/Util.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

void project_save_as(std::string dstPath, std::atomic<bool>* is_active)
{
    std::lock_guard<std::mutex> lock(project_mutex);

    UTIL::correctFolderDistinguishers(dstPath);

    if(is_active != nullptr)
        *is_active = true;

    // Ask for a destination path from the user if not provided
    if(dstPath == ""){
        dstPath = showFileSystemObjectSelectionDialog("Select a folder to duplicate the project folder", UTIL::removeLastWordBySeparatingWithChar(project_path(), UTIL::folderDistinguisher()), {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }

    // If user didn't select a valid path (or the dstPath param is not valid)
    if(dstPath == "" || !std::filesystem::is_directory(dstPath) || !std::filesystem::exists(dstPath)){
        if(is_active != nullptr)
            *is_active = false;

        return;
    }

    //Remove the / or \ at the end of the destination path if there are any
    if(dstPath[dstPath.size() - 1] == '/' || dstPath[dstPath.size() - 1] == '\\'){
        dstPath.pop_back();
    }

    // DestinationPath/MyProject
    std::string dstProjectPath = dstPath + UTIL::folderDistinguisher() + project_title();

    // Get unique name for the destination path
    while (std::filesystem::exists(dstProjectPath))
    {
        dstProjectPath.push_back('_');
    }
    
    // Create the destination folder
    if(!std::filesystem::create_directories(dstProjectPath)){
        LGDLOG::start << "ERROR : Save as : Can't create the destination folder" << LGDLOG::end;
        
        if(is_active != nullptr)
            *is_active = false;

        return;
    }

    // Duplicate files and folders from the original path to destination path
    if(UTIL::duplicateFolder(project_path(), dstProjectPath)){
        //Update the project path
        projectUTIL_set_path(dstProjectPath);
    }
    else{
        LGDLOG::start << "ERROR : Save as : Couldn't copy the contents properly and 'save as' action aborted" << LGDLOG::end;
    }

    if(is_active != nullptr)
        *is_active = false;
}