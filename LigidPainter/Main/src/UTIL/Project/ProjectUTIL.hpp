/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <string>
#include <atomic>
#include <filesystem>

#include <UTIL/Project/Project.hpp>

#include <GUI/Dialogs.hpp>

/// @brief Checks if this->folderPath is valid. If not tries to create the folder into the same path. 
///        If this fails too informs the user and asks for a new path. (Called in updateProject function)
/// @return if you're good to go
static bool projectUTIL_folder_path_check(){
    // Path doesn't exist
    int tries = 0;
    while (!std::filesystem::exists(project_path()))
    {
        // Inform user
        showMessageBox("CRITICAL PROBLEM!", "Your project path is not valid. Please select a new path to your project.", MESSAGEBOX_TYPE_ERROR, MESSAGEBOX_BUTTON_OK);
        tries++;

        // Select new path
        std::string path = showFileSystemObjectSelectionDialog("Compensate the missing project path", UTIL::removeLastWordBySeparatingWithChar(project_path(), UTIL::folderDistinguisher()), 0, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
        
        // Try to create the project
        if(std::filesystem::exists(path)){
            if(project_create(path, project_title(), {})){
                project_load(path + UTIL::folderDistinguisher() + project_title() + UTIL::folderDistinguisher() + project_title() + ".ligid");
            }

        }

        // Filesystem has a problem
        if(tries == 3){
            showMessageBox("oopsies", "Appearently LigidPainter & the file system having a problem determining if your project path exists or not. Since this problem is so rare our fellow developer didn't take his time to put a recovery mechanism there specifically. However don't worry! LigidPainter has a recovery system on its own. Just search it up. And you probably need to close the app rn. Sorry :3. Note : Pls contact the developer if u see this (especially Mert Tetik)", MESSAGEBOX_TYPE_ERROR, MESSAGEBOX_BUTTON_OK);
            return false;
        }
    }
    
    return true;
}

void projectUTIL_set_path(std::string dest_path);

/// @brief Retrieve data from the ligid file
/// @param path 
/// @param creationDate 
/// @param lastOpenedDate 
/// @param meshNodeScene 
/// @param textureRes 
/// @return True if success
bool projectUTIL_read_ligid_file(std::string path,time_t &creationDate,time_t &lastOpenedDate);

/// @brief Write ligid file to the project folder
bool projectUTIL_write_ligid_file(std::string path);