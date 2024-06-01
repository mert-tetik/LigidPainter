/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
    LigidPainter's project file writer
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

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

static bool writeStr(std::ofstream& wf, std::string str){
    int strSize = str.size(); 
    WRITE_BITS(strSize, int, "String size"); 
    for (size_t i = 0; i < strSize; i++) 
    {
        char c = str[i]; 
        WRITE_BITS(c, char, "String character");
    }
}  

static bool write_element(Element element, std::string element_title){
    if(element.state == 0){
        //WRITE_BITS(element.button.);
    }
    if(element.state == 1){
        //WRITE_BITS(element.rangeBar.value);
    }
}

bool wrtLigidFile(std::string path){
    
    // Open writing stream and truncate the file
    std::ofstream wf = std::ofstream(path, std::ios::out | std::ios::binary);

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
    uint32_t versionNumber2600 = 2600;

    WRITE_BITS(versionNumber2600, uint32_t, "");

    // ------------- Date ------------
    time_t currentDate = time(0);
    WRITE_BITS(currentDate, time_t, "");

    time_t lastOpenedDate = time(0);
    WRITE_BITS(lastOpenedDate, time_t, "");

    // ------------- Current model data ------------
    if(!writeStr(wf, getScene()->model->title))
        return false;

    // ------------- Settings ------------


    return true;
}

bool projectUTIL_write_ligid_file(std::string path){
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