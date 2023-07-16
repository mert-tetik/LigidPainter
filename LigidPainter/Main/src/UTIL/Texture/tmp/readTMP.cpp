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

#include <stb_image.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"

void Texture::readTMP(){
    //If the tmp directory doesn't exist then create it
    if(!std::filesystem::exists("./tmp")){
        std::filesystem::create_directories("./tmp");
    }

    //Unique name for the tmp file with the greatest index (doesn't include the extension)
    std::string tmpTitle = getTMPTitleWithGreatestIndex();

    //Destination path of the tmp file (smt like ./tmp/13_3.tmp)
    std::string tmpFilePath = (std::string)"./tmp/" + tmpTitle + (std::string)".tmp"; 

    //Binary file stream
    std::ifstream rf(tmpFilePath , std::ios::in | std::ios::binary);

    if(!rf) {
        std::cout << "ERROR WHILE READING A TMP FILE! Cannot open file : " << tmpFilePath << std::endl;
        return;
    }   
    
    glm::ivec2 txtrResolution;

    //Read the texture resolution data from the tmp file
    rf.read(reinterpret_cast<char*>(   &txtrResolution    ),sizeof(glm::ivec2));
    
    //Pixels of the texture
    char* pixels = new char[txtrResolution.x * txtrResolution.y * 4];

    //Read the pixels from the tmp file (no compression algorithm is used)
    rf.read(pixels, txtrResolution.x * txtrResolution.y * 4);

    //Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);

    //Params of the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    //Write the pixels
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, txtrResolution.x , txtrResolution.y, 0, GL_RGBA, GL_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Release the allocated memory
    delete[] pixels;

    //Close the binary reading stream
    rf.close();

    //Deleate the tmp file after reading
    std::filesystem::remove(tmpFilePath);
}

std::string Texture::getTMPTitleWithGreatestIndex(){
    std::string uniqueTitle = std::to_string(this->uniqueId);

    const char idSeperator = '_';

    //Tmp file indices (those share the same unique id)
    std::vector<int> indices;
    
    //Check all the tmp files
    for (const auto & entry : std::filesystem::directory_iterator("./tmp")){
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
    
    int fileI = 0;
    // Find the biggest index in the indices vector
    for (size_t i = 0; i < indices.size(); i++)
    {
        if(fileI < indices[i])
            fileI = indices[i];
    }

    //Add the index to the unique id
    uniqueTitle += idSeperator + std::to_string(fileI);

    //Return the result
    return uniqueTitle;
}