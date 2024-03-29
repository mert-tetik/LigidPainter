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

#include <stb_image.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

void Texture::writeTMP(std::string tmpTitle){
    //If the tmp directory doesn't exist then create it
    try
    {
        if(!std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp")){
            if(!std::filesystem::create_directories(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp"))
                LGDLOG::start<< "ERROR : Writing TMP file : Creating tmp folder." << LGDLOG::end;     }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 226569 " << ex.what() << LGDLOG::end;
    }

    //Destination path of the tmp file (smt like ./LigidPainter/tmp/13_3.tmp)
    std::string tmpFilePath = UTIL::environmentSpecificAppDataFolderPath() + (std::string)"LigidPainter/tmp/" + tmpTitle + (std::string)".tmp"; 

    //Binary file stream
    std::ofstream wf(tmpFilePath , std::ios::out | std::ios::binary);

    if(!wf) {
        LGDLOG::start<< "ERROR WHILE WRITING A TMP FILE! Cannot create file : " << tmpFilePath << LGDLOG::end;
        return;
    }   

    //Get the resolution of the texture (textures in the LigidPainter always stored with 4 channels (RGBA))
    glm::ivec2 txtrResolution = getResolution();

    //Pixels of the texture
    char* pixels = new char[txtrResolution.x * txtrResolution.y * 4];
    
    //Write the pixels of the texture to the pixels array
    getData(pixels);

    //Write the resolution data to the tmp file
    wf.write(reinterpret_cast<char*>(   &txtrResolution    ),sizeof(glm::ivec2));
    
    //Write the pixels to the tmp file (no compression algorithm is used)
    wf.write(pixels, txtrResolution.x * txtrResolution.y * 4);

    //Release the allocated memory
    delete[] pixels;
}

std::string Texture::generateTMPTitle(){
    std::string uniqueTitle = std::to_string(this->uniqueId);

    const char idSeperator = '_';

    //Tmp file indices (those share the same unique id)
    std::vector<int> indices;
    
    //Check all the tmp files
    for (const auto & entry : std::filesystem::directory_iterator(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp")){
        std::string file = entry.path().string();

        //Remove the directory from the file name
        file = UTIL::getLastWordBySeparatingWithChar(file, UTIL::folderDistinguisher());

        //Remove the extension from the file name
        file = UTIL::removeExtension(file);

        //Unique ID value of the tmp file
        std::string fileID = UTIL::removeLastWordBySeparatingWithChar(file, idSeperator);

        //Shares the same unique ids
        if(fileID == uniqueTitle){
            indices.push_back(std::stoi(UTIL::getLastWordBySeparatingWithChar(file,idSeperator)));
        }
    }
    
    // Find the next available index that is not in the indices vector
    int fileI = 0;
    bool found = false;
    
    while (!found && fileI < indices.size()) {
        if (indices[fileI] == fileI) {
            fileI++;
        } else {
            found = true;
        }
    }

    //Add the index to the unique id
    uniqueTitle += idSeperator + std::to_string(fileI);

    //Return the result
    return uniqueTitle;
}