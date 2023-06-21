/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

bool Project::readLigidFile(std::string path,time_t &creationDate,time_t &lastOpenedDate){ //Returns true if path is a ligid file
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            std::cout << "ERROR WHILE READING LIGID FILE! Cannot open file : " << path << std::endl;
            return false;
        }

        //!HEADER

        //!Description
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            return false;
        }

        //! Read the creation date
        rf.read(reinterpret_cast<char*>(   &creationDate    ),sizeof(time_t));

        //! Read the last opened date
        rf.read(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));
        
        return true;
    }
}