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
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define READ_BITS(var, type, loc)    if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Reading ligid file. Failed to read at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

#define READ_STR(str)   int strSize; \
                        READ_BITS(strSize, int, "Sring size") \
                        for (size_t i = 0; i < strSize; i++) \
                        {\
                            char c; \
                            READ_BITS(c, char, "String char") \
                            str.push_back(c); \
                        }

bool readMatChannel(std::ifstream& rf, Texture& matChannel){
    std::string title;
    READ_STR(title);

    matChannel.ID = 0;

    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(Library::getTextureObj(i).title == title && title != ""){
            matChannel = Library::getTextureObj(i);
        }
    }
    
    return true;
}

//Returns true if path is a ligid file
bool Project::readLigidFile(
                                std::string path,
                                time_t &creationDate,
                                time_t &lastOpenedDate
                            )
{ 
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            LGDLOG::start<< "ERROR : Reading ligid file! Cannot open file : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Description ------------
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        READ_BITS(ch1, uint64_t, "Description header 1");
        READ_BITS(ch2, uint64_t, "Description header 2");
        READ_BITS(ch3, uint64_t, "Description header 3");

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            LGDLOG::start<< "ERROR : Reading ligid file : Description header is not correct : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Version Number ------------
        uint32_t versionNumber2000 = 2000;   
        uint32_t versionNumber2100 = 2100;   
        uint32_t versionNumber2200 = 2200;   
        uint32_t versionNumber2300 = 2300;   
        uint32_t versionNumber2400 = 2400;   
        
        uint32_t versionNumber; //2400  
        READ_BITS(versionNumber, uint32_t, "Version number");
        
        std::cout << "File Version : " << versionNumber << std::endl;
        
        if(versionNumber != versionNumber2400){
            LGDLOG::start<< "ERROR : Reading ligid file : Invalid version : " << versionNumber << LGDLOG::end; 
            return false;
        }

        // ---------- Dates ------------
        READ_BITS(creationDate, time_t, "Creation date");
        std::cout << "Creation date : " << creationDate << std::endl;

        READ_BITS(lastOpenedDate, time_t, "Last opened date");
        std::cout << "Last opening date : " << lastOpenedDate << std::endl;
        
        // ---------- 3D Model ------------
        std::string modelTitle = "";
        READ_STR(modelTitle)
        std::cout << "Active 3D model : " << modelTitle << std::endl;
        for (size_t i = 0; i < Library::getModelArraySize(); i++)
        {
            if(Library::getModelObj(i).title == modelTitle){
                *getModel() = Library::getModelObj(i);
            }
        }
        
        if(!getModel()->meshes.size())
            getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true, false);
        
        // ---------- Settings ------------
        READ_BITS(Settings::properties()->textureRes, int, "Texture resolution");
        
        // ------------- Material Channels & Material ID TEXTURE ------------
        int32_t meshCount;
        READ_BITS(meshCount, int32_t, "");
        for (size_t meshI = 0; meshI < meshCount; meshI++)
        {
            if(meshI >=  getModel()->meshes.size())
                break;

            readMatChannel(rf, getModel()->meshes[meshI].albedo);   
            readMatChannel(rf, getModel()->meshes[meshI].roughness);   
            readMatChannel(rf, getModel()->meshes[meshI].metallic);   
            readMatChannel(rf, getModel()->meshes[meshI].normalMap);   
            readMatChannel(rf, getModel()->meshes[meshI].heightMap);   
            readMatChannel(rf, getModel()->meshes[meshI].ambientOcclusion);   
            
            // Material ID Texture
            getModel()->meshes[meshI].materialIDTxtrPath.clear();
            READ_STR(getModel()->meshes[meshI].materialIDTxtrPath);
            if(getModel()->meshes[meshI].materialIDTxtrPath.size() && std::filesystem::exists(getModel()->meshes[meshI].materialIDTxtrPath)){
                getModel()->meshes[meshI].materialIDTxtr.load(getModel()->meshes[meshI].materialIDTxtrPath.c_str());
                getModel()->meshes[meshI].materialIDColors = getModel()->meshes[meshI].materialIDTxtr.getMaterialIDPalette();
            }
        }


        getModel()->newModelAdded = true;

        return true;
    }
}