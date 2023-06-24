/*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

/// @brief Shows dialogs regarding file system objects (take input from user on files/directories)
/// @param title Title of the dialog
/// @param defaultPath Default path for the searcher
/// @param filters Extension filters for the file dialogs
/// @param multipleSelection Enable multiple selection if the type is FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE
/// @param type FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE , FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER , FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE
/// @return the path to the selected file system object
std::string showFileSystemObjectSelectionDialog
                                                (
                                                    const std::string title, 
                                                    const std::string defaultPath, 
                                                    const std::vector<std::string> filters, 
                                                    const bool multipleSelection, 
                                                    const int type
                                                )
{
    //The result
    std::string path;

    //Select folder dialog
    if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER){
        path = tinyfd_selectFolderDialog(
                                            title.c_str(), //Title of the dialog 
                                            defaultPath.c_str() //The default folder path
                                        );
    }

    //Select file dialog
    else if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE){
        Util util;
        const char * const * filterPatterns = util.convertStringArray(filters);

        path = tinyfd_openFileDialog(
                                        title.c_str(), //Title of the dialog
                                        defaultPath.c_str(), //The default path
                                        (int)filters.size(),  //Size of the filters array
                                        filterPatterns, //File extension filters
                                        "", //Single extension description (not used)
                                        multipleSelection //Enable multiple selections
                                    );
    }
    
    //Export (extract / save) file dialog
    else if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE){
        Util util;
        const char * const * filterPatterns = util.convertStringArray(filters);

        path = tinyfd_saveFileDialog(
                                        title.c_str(), //Title of the dialog
                                        defaultPath.c_str(), //The default path
                                        (int)filters.size(),  //Size of the filters array
                                        filterPatterns, //File extension filters
                                        "" //Single extension description (not used)
                                    );
    }

    //Return the retrieved path
    return path;
}