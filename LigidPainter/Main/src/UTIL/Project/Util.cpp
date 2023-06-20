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

void Project::deleteFilesInFolder(const std::string folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry.path());
        }
    }
}

void Project::updateLigidFile(){
    std::ofstream wf(this->ligidFilePath, std::ios::out | std::ios::binary);
		
    if(!wf) {
        std::cout << "ERROR WHILE WRITING LIGID FILE! Cannot open file : " << this->ligidFilePath << std::endl;
        return;
    }

    //!HEADER
        
    //!Description
    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 

    wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
    wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
    wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));

    //! Write the creation date
    time_t currentDate = time(0);
    wf.write(reinterpret_cast<char*>(   &currentDate    ),sizeof(time_t));
    
    //! Write the last opened date
    time_t lastOpenedDate = time(0);
    wf.write(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));

}


std::string Project::locateLigidFileInFolder(const std::string& folderPath)
{
    const std::string extension = ".ligid";
    std::vector<std::string> matchingFiles;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            matchingFiles.push_back(entry.path().string());
        }
    }

    if (matchingFiles.empty())
    {
        return ""; // No file with the given extension found
    }
    else
    {
        // Return the first matching file path
        return matchingFiles[0];
    }
}

bool Project::returnLigidFileData(std::string path,time_t &creationDate,time_t &lastOpenedDate){ //Returns true if path is a ligid file
    
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

void Project::copyTheProjectPathToTheClipboard(GLFWwindow* window){
    glfwSetClipboardString(window, std::filesystem::absolute(folderPath).string().c_str());
}