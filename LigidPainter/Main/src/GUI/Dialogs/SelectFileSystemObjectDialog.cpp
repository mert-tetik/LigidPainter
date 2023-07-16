/*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

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
/// @param filterTemplate FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID
/// @param multipleSelection Enable multiple selection if the type is FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE
/// @param type FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE , FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER , FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE
/// @return the path to the selected file system object
std::string showFileSystemObjectSelectionDialog
                                                (
                                                    const std::string title, 
                                                    const std::string defaultPath, 
                                                    const int filterTemplate, 
                                                    const bool multipleSelection, 
                                                    const int type
                                                )
{
    //The result
    char* path;

    //Select folder dialog
    if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER){
        path = tinyfd_selectFolderDialog(
                                            title.c_str(), //Title of the dialog 
                                            defaultPath.c_str() //The default folder path
                                        );
    }

    //Select file dialog
    else if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE){
        
        if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE){
            const char* filters[12] = { "*.png","*.jpeg","*.jpg","*.bmp", "*.gif", "*.tga", "*.hdr", "*.pic", "*.pnm", "*.ppm", "*.pgm", "*.pbm"  };   
            path = tinyfd_openFileDialog(
                                            title.c_str(), //Title of the dialog
                                            defaultPath.c_str(), //The default path
                                            12,  //Size of the filters array
                                            filters, //File extension filters
                                            "", //Single extension description (not used)
                                            multipleSelection //Enable multiple selections
                                        );
        }

        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL){
            const char* filters[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };
            path = tinyfd_openFileDialog(
                                            title.c_str(), //Title of the dialog
                                            defaultPath.c_str(), //The default path
                                            11,  //Size of the filters array
                                            filters, //File extension filters
                                            "", //Single extension description (not used)
                                            multipleSelection //Enable multiple selections
                                        );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID){
            const char* filters[1] = { "*.ligid" };
            path = tinyfd_openFileDialog(
                                            title.c_str(), //Title of the dialog
                                            defaultPath.c_str(), //The default path
                                            1,  //Size of the filters array
                                            filters, //File extension filters
                                            "", //Single extension description (not used)
                                            multipleSelection //Enable multiple selections
                                        );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH){
            const char* filters[1] = { "*.lgdbrush" };
            path = tinyfd_openFileDialog(
                                            title.c_str(), //Title of the dialog
                                            defaultPath.c_str(), //The default path
                                            1,  //Size of the filters array
                                            filters, //File extension filters
                                            "", //Single extension description (not used)
                                            multipleSelection //Enable multiple selections
                                        );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL){
            const char* filters[1] = { "*.lgdmaterial" };
            path = tinyfd_openFileDialog(
                                            title.c_str(), //Title of the dialog
                                            defaultPath.c_str(), //The default path
                                            1,  //Size of the filters array
                                            filters, //File extension filters
                                            "", //Single extension description (not used)
                                            multipleSelection //Enable multiple selections
                                        );
        }

    }
    
    //Export (extract / save) file dialog
    else if(type == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE){
        ;

        if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE){
            const char* filters[12] = { "*.png","*.jpeg","*.jpg","*.bmp", "*.gif", "*.tga", "*.hdr", "*.pic", "*.pnm", "*.ppm", "*.pgm", "*.pbm"  };   
            path = tinyfd_saveFileDialog(
                                title.c_str(), //Title of the dialog
                                defaultPath.c_str(), //The default path
                                12,  //Size of the filters array
                                filters, //File extension filters
                                "" //Single extension description (not used)
                            );
        }

        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL){
            const char* filters[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };
            path = tinyfd_saveFileDialog(
                                title.c_str(), //Title of the dialog
                                defaultPath.c_str(), //The default path
                                11,  //Size of the filters array
                                filters, //File extension filters
                                "" //Single extension description (not used)
                            );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID){
            const char* filters[1] = { "*.ligid" };
            path = tinyfd_saveFileDialog(
                                title.c_str(), //Title of the dialog
                                defaultPath.c_str(), //The default path
                                1,  //Size of the filters array
                                filters, //File extension filters
                                "" //Single extension description (not used)
                            );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH){
            const char* filters[1] = { "*.lgdbrush" };
            path = tinyfd_saveFileDialog(
                                title.c_str(), //Title of the dialog
                                defaultPath.c_str(), //The default path
                                1,  //Size of the filters array
                                filters, //File extension filters
                                "" //Single extension description (not used)
                            );
        }
        
        else if(filterTemplate == FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL){
            const char* filters[1] = { "*.lgdmaterial" };
        
            path = tinyfd_saveFileDialog(
                                title.c_str(), //Title of the dialog
                                defaultPath.c_str(), //The default path
                                1,  //Size of the filters array
                                filters, //File extension filters
                                "" //Single extension description (not used)
                            );
        }
    }

    if(path == nullptr)
        return "";

    else{
        std::string castedPath(path);

        //Return the retrieved path
        return castedPath;
    }
}