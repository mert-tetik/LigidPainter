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

bool Project::loadProject(std::string ligidFilePath,AppMaterialModifiers& appMaterialModifiers){
    
    while(true){
        if(!this->projectProcessing)
            break;
    }

    this->projectProcessing = true;



    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        LGDLOG::start<< "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << LGDLOG::end;
        this->projectProcessing = false;
        return false;
    }

    //Remove the file from the path (./myProject/myProject.ligid -> ./myProject/) 
    this->folderPath = UTIL::removeLastWordBySeparatingWithChar(ligidFilePath, UTIL::folderDistinguisher());

    //Make sure folder path doesn't have seperator at the end (./myProject/ -> ./myProject)
    if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') 
        this->folderPath.pop_back();

    //writeLigidFile(meshNodeScene);

    try
    {
        //Load the textures
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

                
                Library::addTexture(texture);
            }
        }




        //Load the materials
        Library::clearMaterials();
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Materials")){
            std::string materialPath = entry.path().string();

            Material material("", 0);
            if(FileHandler::readLGDMATERIALFile(materialPath, material, appMaterialModifiers))
                Library::addMaterial(material);
        
        }

        //TODO Do smt with these variables
        time_t creationDate; 
        time_t lastOpenedDate;

        if(!readLigidFile(ligidFilePath, creationDate, lastOpenedDate)){
            this->projectProcessing = false;
            return false;
        }
        
        //Load the brushes
        Library::clearBrushes();
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Brushes")){
            std::string brushPath = entry.path().string();

            Brush brush;
            if(FileHandler::readLGDBRUSHFile(brushPath, brush)){
                Library::addBrush(brush);
            }
        }

        //Load the filters
        Library::clearFilters();
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Filters")){
            std::string filterPath = entry.path().string();

            Filter filter;
            if(filter.load(filterPath))
                Library::addFilter(filter);
        }

   
        //Load the texture packs
        Library::clearTexturePacks();
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Texture Packs")){
            std::string filterPath = entry.path().string();

            TexturePack texturePack;
            texturePack.load(filterPath);
            Library::addTexturePack(texturePack);
        }

        
        //Load the tdmodels
        Library::clearModels();
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "3DModels")){
            std::string modelPath = entry.path().string();

            Model TDModel;
            bool error = !TDModel.loadModel(modelPath, true);

            //Check if the model is an obj file
            if(UTIL::getLastWordBySeparatingWithChar(modelPath, '.') != "obj"){
                //The 3D model in the project folder is not an obj file

                //Delete the non-obj file
                std::filesystem::remove(modelPath);
            
                //Recreate the obj file
                FileHandler::writeOBJFile(UTIL::removeExtension(modelPath) + ".obj",TDModel);
            }

            if(TDModel.meshes.size() && !error)
                Library::addModel(TDModel);
            else if(!error)
                LGDLOG::start<< "ERROR : Can't add the 3D model to the Library:: Mesh size is 0!" << LGDLOG::end;

        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 658972 " << ex.what() << LGDLOG::end;
    }

    


    if(Library::getModelArraySize())
        *getModel() = *Library::getModel(0);
    else
        getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true);


    this->projectProcessing = false;
    return true;
}