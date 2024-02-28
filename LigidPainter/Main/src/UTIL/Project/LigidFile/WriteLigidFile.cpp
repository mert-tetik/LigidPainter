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
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define WRITE_BITS(var, type, loc)   if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Writing ligid file. Failed to write at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

bool writeStr(std::ofstream& wf, std::string str){
    int strSize = str.size(); 
    WRITE_BITS(strSize, int, "String size"); 
    for (size_t i = 0; i < strSize; i++) 
    {
        char c = str[i]; 
        WRITE_BITS(c, char, "String character");
    }
}  

int findIndexInLibrary(Texture txtr){
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(txtr.ID == Library::getTextureObj(i).ID){
            return i;
        }
    }

    return -1;
}

bool Project::wrtLigidFile(std::string path){
    std::ofstream wf;
    
    wf = std::ofstream(path, std::ios::out | std::ios::binary);

    if(!wf) {
        LGDLOG::start<< "ERROR! : Writing ligid file." << LGDLOG::end;
        return false;
    }

    // ------------- Description ------------
    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 
    WRITE_BITS(h1, uint64_t, "");
    WRITE_BITS(h2, uint64_t, "");
    WRITE_BITS(h3, uint64_t, "");
    
    // ------------- Version number ------------
    uint32_t versionNumber2000 = 2000;  
    uint32_t versionNumber2100 = 2100;  
    uint32_t versionNumber2200 = 2200;
    uint32_t versionNumber2300 = 2300;
    uint32_t versionNumber2400 = 2400;
    uint32_t versionNumber2500 = 2500;
    uint32_t versionNumber2600 = 2600;

    WRITE_BITS(versionNumber2600, uint32_t, "");

    // ------------- Date ------------
    time_t currentDate = time(0);
    WRITE_BITS(currentDate, time_t, "");

    time_t lastOpenedDate = time(0);
    WRITE_BITS(lastOpenedDate, time_t, "");

    // ------------- Current model data ------------
    if(!writeStr(wf, getModel()->title))
        return false;

    // ------------- Settings ------------


    return true;
}

bool Project::writeLigidFile(std::string path){
    bool res = wrtLigidFile(path);
    if(!res){
        try
        {
            if(std::filesystem::exists(path))
                std::filesystem::remove(path);
            
            std::ofstream outputFile(path);
            
            outputFile.close();
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Recreating ligid file : " << ex.what() << LGDLOG::end;
        }
    }

    return res;
}



//------------ UTIL FUNCTIONS -------------