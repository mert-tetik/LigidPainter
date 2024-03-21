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

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

static void LOAD_textures(const std::string textures_folder_path);
static void LOAD_materials(const std::string materials_folder_path);
static void LOAD_brushes(const std::string brushes_folder_path);
static void LOAD_models(const std::string models_folder_path);
static void LOAD_filters(const std::string filters_folder_path);
static void LOAD_texturePacks(const std::string texture_packs_folder_path);

bool project_load_library_elements(std::string folderPath, std::string ligidFilePath){
    try
    {
        //Load the textures
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "Textures")){
            LOAD_textures(project_path() + UTIL::folderDistinguisher() + "Textures");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Textures folder doesn't exists" << LGDLOG::end;
        }

        //Load the materials
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "Materials")){
            LOAD_materials(project_path() + UTIL::folderDistinguisher() + "Materials");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Materials folder doesn't exists" << LGDLOG::end;
        }

        //Load the 3D models
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "3DModels")){
            LOAD_models(project_path() + UTIL::folderDistinguisher() + "3DModels");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : 3DModels folder doesn't exists" << LGDLOG::end;
        }

        if(ligidFilePath.size()){
            time_t creationDate; 
            time_t lastOpenedDate;
            
            // Read the ligid file
            if(!projectUTIL_read_ligid_file(ligidFilePath, creationDate, lastOpenedDate)){
                // Couldn't read the ligid file (probably cause of version)

                LGDLOG::start << "WARNING : Failed to load the ligid file : New ligid file will be generated." << LGDLOG::end;
                
                // Generate new ligid file
                if(projectUTIL_write_ligid_file(ligidFilePath)){
                    
                    // Read the generated ligid file
                    if(!projectUTIL_read_ligid_file(ligidFilePath, creationDate, lastOpenedDate)){
                        // Can't load the generated ligid file :( 
                        LGDLOG::start << "WARNING : Failed to load the regenerated ligid file : Ligid file will be ignored." << LGDLOG::end;
                    }
                
                }
                else{
                    // Couldn't generate the new ligid file :(
                    LGDLOG::start << "WARNING : Failed to rewrite the ligid file : Ligid file will be ignored." << LGDLOG::end;
                }
            }
        }
        
        //Load the brushes
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "Brushes")){
            LOAD_brushes(project_path() + UTIL::folderDistinguisher() + "Brushes");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Brushes folder doesn't exists" << LGDLOG::end;
        }

        //Load the filters
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "Filters")){
            LOAD_filters(project_path() + UTIL::folderDistinguisher() + "Filters");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Filters folder doesn't exists" << LGDLOG::end;
        }

        //Load the texture packs
        if(std::filesystem::exists(project_path() + UTIL::folderDistinguisher() + "Texture Packs")){
            LOAD_texturePacks(project_path() + UTIL::folderDistinguisher() + "Texture Packs");
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Texture Packs folder doesn't exists" << LGDLOG::end;
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 658972 " << ex.what() << LGDLOG::end;
    }

    return true;
}


bool project_load(std::string ligidFilePath){
    
    while(true){
        if(!projectUTIL_processing)
            break;
    }
    
    projectUTIL_processing = true;

    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        LGDLOG::start<< "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << LGDLOG::end;
        projectUpdatingThreadElements.updateTextures = false;
        projectUTIL_processing = false;
        return false;
    }

    //Remove the file from the path (./myProject/myProject.ligid -> ./myProject/) 
    projectUTIL_set_path(UTIL::removeLastWordBySeparatingWithChar(ligidFilePath, UTIL::folderDistinguisher()));

    project_load_library_elements(project_path(), ligidFilePath);

    projectUpdatingThreadElements.updateTextures = false;
    projectUTIL_processing = false;
    return true;
}




























/*---------------------- UTILS ------------------------*/

static void LOAD_textures(const std::string textures_folder_path){
    // Cler the textures
    Library::clearTextures();
    
    // Loop thru all the files in the textures folder
    for (const auto & entry : std::filesystem::directory_iterator(textures_folder_path)){
        // Path to the texture file
        std::string texturePath = entry.path().string();

        // Load the texture
        Texture texture;
        if(texture.load(texturePath.c_str())){
            //Check if the texture is a png file (re-create the texture file if not png)
            if(UTIL::getFileExtension(texturePath) != "png"){
                //The texture in the project folder is not a png file

                //Delete the non-png file
                std::filesystem::remove(texturePath);
            
                //Recreate the png file
                texture.exportTexture(UTIL::removeLastWordBySeparatingWithChar(texturePath, UTIL::folderDistinguisher()), "PNG");
            }
            
            // Add texture to the library
            Library::addTexture(texture, "");
        }
        else{
            // Can't load the texture file
            LGDLOG::start << "WARNING! Loading project folder : Can't read a texture file! : " << texturePath << LGDLOG::end;
        }
    }
}

static void LOAD_materials(const std::string materials_folder_path){
    // Clear materials
    Library::clearMaterials();
    
    // Loop thru all the files in the materials folder
    for (const auto & entry : std::filesystem::directory_iterator(materials_folder_path)){
        
        // Path to the material file
        std::string materialPath = entry.path().string();

        // If the file is a regular file
        if(std::filesystem::is_regular_file(materialPath)){
            
            // If the extension is lgdmaterial
            if(UTIL::getFileExtension(materialPath) == "lgdmaterial"){
                
                // Read the material
                Material material("", 0);
                if(FileHandler::readLGDMATERIALFile(materialPath, material)){
                    // Add the material to the library if material readen successfuly 
                    Library::addMaterial(material, "");
                }
                else{
                    // Can't load the material file
                    LGDLOG::start << "WARNING! Loading project folder : Can't read a material file! : " << materialPath << LGDLOG::end;
                }
            
            }
            else{
                // Irrevelant file detected
                LGDLOG::start << "WARNING! Loading project folder : Unrelevant file detected in materials folder : " << materialPath << LGDLOG::end;
            }

        }
        else{
            // Un-regular file detected
            LGDLOG::start << "WARNING! Loading project folder : Un-regular file detected : " << materialPath << LGDLOG::end;
        }
    }
}

static void LOAD_brushes(const std::string brushes_folder_path){
    // Clear brushes
    Library::clearBrushes();
    
    // Loop thru all the files in the brushes folder
    for (const auto & entry : std::filesystem::directory_iterator(brushes_folder_path)){
        
        // Path to the brush file
        std::string brushPath = entry.path().string();

        // If the file is a regular file
        if(std::filesystem::is_regular_file(brushPath)){
            
            // If the extension is lgdbrush
            if(UTIL::getFileExtension(brushPath) == "lgdbrush"){
                
                // Read the brush
                Brush brush;
                if(FileHandler::readLGDBRUSHFile(brushPath, brush)){
                    // Add the brush to the library if brush readen successfuly 
                    Library::addBrush(brush, "");
                }
                else{
                    // Can't load the brush file
                    LGDLOG::start << "WARNING! Loading project folder : Can't read a brush file! : " << brushPath << LGDLOG::end;
                }
            
            }
            else{
                // Irrevelant file detected
                LGDLOG::start << "WARNING! Loading project folder : Unrelevant file detected in brushes folder : " << brushPath << LGDLOG::end;
            }

        }
        else{
            // Un-regular file detected
            LGDLOG::start << "WARNING! Loading project folder : Un-regular file detected : " << brushPath << LGDLOG::end;
        }
    }
}

static void LOAD_models(const std::string models_folder_path){
    // Clear models
    Library::clearModels();
    
    // Loop thru all the files in the models folder
    for (const auto & entry : std::filesystem::directory_iterator(models_folder_path)){
        
        // Path to the model file
        std::string modelPath = entry.path().string();

        // If the file is a regular file
        if(std::filesystem::is_regular_file(modelPath)){
            
            // If the extension is lgdmodel
            if(UTIL::getFileExtension(modelPath) == "lgdmodel"){
                
                // Read the model
                Model model;
                if(FileHandler::readLGDMODELFile(modelPath, model)){
                    if(model.meshes.size()){
                        // Add the model to the library if model readen successfuly & model has meshes 
                        Library::addModel(model);
                    }
                    else{
                        // Mesh size is 0
                        LGDLOG::start << "WARNING! Loading project folder : Can't load model file : Mesh size is 0 : " << modelPath << LGDLOG::end;
                    }
                }
                else{
                    // Can't load the model file
                    LGDLOG::start << "WARNING! Loading project folder : Can't read a model file! : " << modelPath << LGDLOG::end;
                }
            
            }
            else{
                // Irrevelant file detected
                LGDLOG::start << "WARNING! Loading project folder : Unrelevant file detected in models folder : " << modelPath << LGDLOG::end;
            }

        }
        else{
            // Un-regular file detected
            LGDLOG::start << "WARNING! Loading project folder : Un-regular file detected : " << modelPath << LGDLOG::end;
        }
    }
}

static void LOAD_filters(const std::string filters_folder_path){
    // Clear filters
    Library::clearFilters();
    
    // Loop thru all the files in the filters folder
    for (const auto & entry : std::filesystem::directory_iterator(filters_folder_path)){
        
        // Path to the filter file
        std::string filterPath = entry.path().string();

        // If the file is a regular file
        if(std::filesystem::is_regular_file(filterPath)){
            
            // If the extension is lgdfilter
            if(UTIL::getFileExtension(filterPath) == "lgdfilter"){
                
                // Read the filter
                Filter filter;
                if(filter.load(filterPath)){
                    // Add the filter to the library if filter readen successfuly 
                    Library::addFilter(filter);
                }
                else{
                    // Can't load the filter file
                    LGDLOG::start << "WARNING! Loading project folder : Can't read a filter file! : " << filterPath << LGDLOG::end;
                }
            
            }
            else{
                // Irrevelant file detected
                LGDLOG::start << "WARNING! Loading project folder : Unrelevant file detected in filters folder : " << filterPath << LGDLOG::end;
            }

        }
        else{
            // Un-regular file detected
            LGDLOG::start << "WARNING! Loading project folder : Un-regular file detected : " << filterPath << LGDLOG::end;
        }
    }
}
static void LOAD_texturePacks(const std::string texture_packs_folder_path){
    
    // Clear texture packs
    Library::clearTexturePacks();
    
    for (const auto & entry : std::filesystem::directory_iterator(texture_packs_folder_path)){
        std::string texturePackPath = entry.path().string();

        if(std::filesystem::is_directory(texturePackPath)){
            TexturePack texturePack;
            texturePack.load(texturePackPath);
            Library::addTexturePack(texturePack);
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : A file was located in texture packs folder : " << texturePackPath << LGDLOG::end;
        }
    }
}