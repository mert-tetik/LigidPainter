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

#include <stb_image.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"

void Texture::writeTMP(){
    //If the tmp directory doesn't exist then create it
    if(!std::filesystem::exists("./tmp")){
        std::filesystem::create_directories("./tmp");
    }

    //Unique generated name for the tmp file (doesn't include the extension)
    std::string tmpTitle = generateTMPTitle();

    //Destination path of the tmp file (smt like ./tmp/13_3.tmp)
    std::string tmpFilePath = (std::string)"./tmp/" + tmpTitle + (std::string)".tmp"; 

    //Binary file stream
    std::ofstream wf(tmpFilePath , std::ios::out | std::ios::binary);

    if(!wf) {
        std::cout << "ERROR WHILE WRITING A TMP FILE! Cannot create file : " << path << std::endl;
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

    Util util;

    const char idSeperator = '_';

    //Tmp file indices (those share the same unique id)
    std::vector<int> indices;
    
    //Check all the tmp files
    for (const auto & entry : std::filesystem::directory_iterator("./tmp")){
        std::string file = entry.path().string();

        //Remove the directory from the file name
        file = util.getLastWordBySeparatingWithChar(file, folderDistinguisher);

        //Remove the extension from the file name
        file = util.removeExtension(file);

        //Unique ID value of the tmp file
        std::string fileID = util.removeLastWordBySeparatingWithChar(file, idSeperator);

        //Shares the same unique ids
        if(fileID == uniqueTitle){
            indices.push_back(std::stoi(util.getLastWordBySeparatingWithChar(file,idSeperator)));
        }
    }
    
    int fileI = 0;
    // Find the next available index that is not in the indices vector
    while (std::find(indices.begin(), indices.end(), fileI) != indices.end()) {
        fileI++;
    }

    //Add the index to the unique id
    uniqueTitle += idSeperator + std::to_string(fileI);

    //Return the result
    return uniqueTitle;
}