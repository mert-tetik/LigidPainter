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
#include "UTIL/Wait/Wait.hpp"

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define TD_MODEL_FOLDER_CREATION 0
#define BRUSH_FOLDER_CREATION 1
#define FILTER_FOLDER_CREATION 2

//Forward declaration for the util functions
void completeFolder(std::string path, int action);

extern std::atomic<bool> load_ligidpainter_done;

bool project_create(std::string destinationPath, std::string name, std::vector<std::string> TDModelPaths){
    
    UTIL::correctFolderDistinguishers(destinationPath);

    // Wait until LigidPainter is loaded successfuly
    WAIT_WHILE(!load_ligidpainter_done);

    std::lock_guard<std::mutex> lock(project_mutex);

    //Make sure destination path doesn't have / at the end
    if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') 
        destinationPath.pop_back();

    //If the destination path doesn't exist inform the user and return 
    if(!std::filesystem::exists(destinationPath)){
        showMessageBox(
                            "Warning!", 
                            "Project path is not valid! Please try another directory : " + destinationPath, 
                            MESSAGEBOX_TYPE_WARNING, 
                            MESSAGEBOX_BUTTON_OK
                        );
                        
        
        return false;
    }
    
    //If there is already a project with the same name and the same directory
    if(std::filesystem::exists(destinationPath + UTIL::folderDistinguisher() + name)){
        int res = showMessageBox
                (
                    "Warning!", 
                    "This folder is already exists. Do you want to overwrite?", 
                    MESSAGEBOX_TYPE_WARNING, 
                    MESSAGEBOX_BUTTON_YESNO
                );

        //If pressed the 'no' abort the process
        if(res == 0){
            return false;
        }

        //If pressed to 'yes' remove the folder named same
        else if(res == 1){
            try
            {
                std::filesystem::remove_all(destinationPath + UTIL::folderDistinguisher() + name);
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LGDLOG::start << "ERROR : Filesystem : Location ID 286424 " << ex.what() << LGDLOG::end;
            }
        }
    }
    //Update the folder path of the project
    std::string folderPath = destinationPath + UTIL::folderDistinguisher() + name;
    
    try
    {
        //Create the project folder
        if(!std::filesystem::create_directory(folderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << folderPath << LGDLOG::end; 
        
        projectUTIL_set_path(folderPath);

        //Create the saved data folder
        std::string recoverFolderPath = folderPath + UTIL::folderDistinguisher() + "Recover";
        if(!std::filesystem::create_directory(recoverFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << recoverFolderPath << LGDLOG::end; 
        
        //Create the textures folder
        std::string textureFolderPath = folderPath + UTIL::folderDistinguisher() + "Textures";
        if(!std::filesystem::create_directory(textureFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << textureFolderPath << LGDLOG::end; 

        //Materials
        std::string materialsFolderPath = folderPath + UTIL::folderDistinguisher() + "Materials";
        if(!std::filesystem::create_directory(materialsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << materialsFolderPath << LGDLOG::end; 
        
        //Brushes
        std::string brushesFolderPath = folderPath + UTIL::folderDistinguisher() + "Brushes";
        if(!std::filesystem::create_directory(brushesFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << brushesFolderPath << LGDLOG::end; 
        UTIL::duplicateFolder("./LigidPainter/Resources/Brushes", brushesFolderPath);
        
        //Fonts
        std::string fontsFolderPath = folderPath + UTIL::folderDistinguisher() + "Fonts";
        if(!std::filesystem::create_directory(fontsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << fontsFolderPath << LGDLOG::end; 
        
        //Scripts
        std::string scriptsFolderPath = folderPath + UTIL::folderDistinguisher() + "Scripts";
        if(!std::filesystem::create_directory(scriptsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << scriptsFolderPath << LGDLOG::end; 
        
        //Texture Packs
        std::string texturePacksFolderPath = folderPath + UTIL::folderDistinguisher() + "Texture Packs";
        if(!std::filesystem::create_directory(texturePacksFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << texturePacksFolderPath << LGDLOG::end; 
        
        //Filters
        std::string filtersFolderPath = folderPath + UTIL::folderDistinguisher() + "Filters";
        if(!std::filesystem::create_directory(filtersFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << filtersFolderPath << LGDLOG::end; 
        UTIL::duplicateFolder("./LigidPainter/Resources/Filters", filtersFolderPath);
        
        //Layers
        std::string layersFolderPath = folderPath + UTIL::folderDistinguisher() + "Layers";
        if(!std::filesystem::create_directory(layersFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << layersFolderPath << LGDLOG::end; 
        
        //3D Models
        std::string tdModelFolderPath = folderPath + UTIL::folderDistinguisher() + "3DModels";
        if(!std::filesystem::create_directory(tdModelFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << tdModelFolderPath << LGDLOG::end; 
        

        TDModelPaths.push_back("./LigidPainter/Resources/3D Models/sphere.fbx");
        TDModelPaths.push_back("./LigidPainter/Resources/3D Models/plane.fbx");

        for (size_t i = 0; i < TDModelPaths.size(); i++)
        {
            project_add_model(TDModelPaths[i]);
        }
        
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 119121 " << ex.what() << LGDLOG::end;
    }

    // Create ligid file
    if(!projectUTIL_write_ligid_file(folderPath + UTIL::folderDistinguisher() + name + ".ligid")){
        LGDLOG::start << "ERROR : Creating project folder : Failed to write ligid file." << LGDLOG::end;
    }
    
    return true;
}

/// @brief add the contents off the related folder
void completeFolder(std::string path, int action){
    
    if(action == FILTER_FOLDER_CREATION){
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Filters")) {
            if (std::filesystem::is_regular_file(entry)) {
                // Get the filename from the full path
                std::string filename = entry.path().filename().string();
                
                // Create the destination path by combining the destination folder path and filename
                std::filesystem::path destinationPath = std::filesystem::path(path) / filename;
                
                // Copy the file
                std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
    else if(action == BRUSH_FOLDER_CREATION){
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Brushes")) {
            if (std::filesystem::is_regular_file(entry)) {
                // Get the filename from the full path
                std::string filename = entry.path().filename().string();
                
                // Create the destination path by combining the destination folder path and filename
                std::filesystem::path destinationPath = std::filesystem::path(path) / filename;
                
                // Copy the file
                std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
}