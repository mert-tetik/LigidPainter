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
#include "GUI/Elements/Elements.hpp"
#include "3D/ThreeD.hpp"
#include "LibrarySystem/Library.hpp"
#include "GUI/GUI.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

bool Project::loadLibraryElements(std::string folderPath, std::string ligidFilePath){
    try
    {
        //Load the textures
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "Textures")){
            std::cout << "LOAD MODE : TEXTURES" << std::endl;
            
            Library::clearTextures();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Textures")){
                std::string texturePath = entry.path().string();

                Texture texture;
                bool error = !texture.load(texturePath.c_str());

                if(!error){
                    //Check if the texture is a png file
                    if(UTIL::getLastWordBySeparatingWithChar(texturePath, '.') != "png"){
                        //The texture in the project folder is not an png file

                        //Delete the non-png file
                        std::filesystem::remove(texturePath);
                    
                        //Recreate the png file
                        texture.exportTexture(UTIL::removeLastWordBySeparatingWithChar(texturePath, UTIL::folderDistinguisher()), "PNG");
                    }

                    
                    Library::addTexture(texture, "");
                }
            }
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Textures folder doesn't exists" << LGDLOG::end;
        }



        //Load the materials
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "Materials")){
            std::cout << "LOAD MODE : MATERIALS" << std::endl;

            Library::clearMaterials();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Materials")){
                std::string materialPath = entry.path().string();

                Material material("", 0);
                if(FileHandler::readLGDMATERIALFile(materialPath, material))
                    Library::addMaterial(material, "");
            
            }
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Materials folder doesn't exists" << LGDLOG::end;
        }

        //Load the tdmodels
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "3DModels")){
            std::cout << "LOAD MODE : 3D MODELS" << std::endl;
            
            Library::clearModels();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "3DModels")){
                std::string modelPath = entry.path().string();

                Model TDModel;
                bool success = TDModel.loadModel(modelPath, true, false);

                if(TDModel.meshes.size() && success){
                    Library::addModel(TDModel);
                }
                else if(success)
                    LGDLOG::start<< "ERROR : Can't add the 3D model to the Library : Mesh size is 0!" << LGDLOG::end;
            }
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : 3DModels folder doesn't exists" << LGDLOG::end;
        }


        if(ligidFilePath.size()){
            std::cout << "LOAD MODE : LIGID FILE" << std::endl;
            //TODO Do smt with these variables
            time_t creationDate; 
            time_t lastOpenedDate;

            if(!this->readLigidFile(ligidFilePath, creationDate, lastOpenedDate)){
                LGDLOG::start << "WARNING : Failed to load the ligid file : New ligid file will be generated!" << LGDLOG::end;
                if(this->writeLigidFile(ligidFilePath)){
                    if(!this->readLigidFile(ligidFilePath, creationDate, lastOpenedDate)){
                        LGDLOG::start << "WARNING : Failed to load the regenerated ligid file : Ligid file will be ignored." << LGDLOG::end;
                    }
                }
                else{
                    LGDLOG::start << "WARNING : Failed to rewrite the ligid file : Ligid file will be ignored." << LGDLOG::end;
                }
            }
        }
        
        //Load the brushes
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "Brushes")){
            std::cout << "LOAD MODE : BRUSHES" << std::endl;
            
            Library::clearBrushes();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Brushes")){
                std::string brushPath = entry.path().string();

                Brush brush;
                if(FileHandler::readLGDBRUSHFile(brushPath, brush)){
                    Library::addBrush(brush, "");
                }
            }
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Brushes folder doesn't exists" << LGDLOG::end;
        }

        //Load the filters
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "Filters")){
            std::cout << "LOAD MODE : FILTERS" << std::endl;

            Library::clearFilters();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Filters")){
                std::string filterPath = entry.path().string();

                Filter filter;
                if(filter.load(filterPath))
                    Library::addFilter(filter);
            }
        }
        else{
            LGDLOG::start << "WARNING! Loading project folder : Filters folder doesn't exists" << LGDLOG::end;
        }

        //Load the texture packs
        if(std::filesystem::exists(this->folderPath + UTIL::folderDistinguisher() + "Texture Packs")){
            std::cout << "LOAD MODE : TEXTURE PACKS" << std::endl;
    
            Library::clearTexturePacks();
            for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Texture Packs")){
                std::string filterPath = entry.path().string();

                TexturePack texturePack;
                texturePack.load(filterPath);
                Library::addTexturePack(texturePack);
            }
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

bool Project::loadProject(std::string ligidFilePath){
    
    while(true){
        if(!this->projectProcessing)
            break;
    }
    
    std::cout << "LOAD MODE : START" << std::endl;

    this->projectProcessing = true;

    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        LGDLOG::start<< "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << LGDLOG::end;
        projectUpdatingThreadElements.updateTextures = false;
        this->projectProcessing = false;
        return false;
    }

    //Remove the file from the path (./myProject/myProject.ligid -> ./myProject/) 
    this->folderPath = UTIL::removeLastWordBySeparatingWithChar(ligidFilePath, UTIL::folderDistinguisher());

    //Make sure folder path doesn't have seperator at the end (./myProject/ -> ./myProject)
    if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') 
        this->folderPath.pop_back();

    //writeLigidFile(meshNodeScene);
    
    this->loadLibraryElements(this->folderPath, ligidFilePath);

    projectUpdatingThreadElements.updateTextures = false;
    this->projectProcessing = false;
    return true;
}