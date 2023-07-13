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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


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

void Project::copyTheProjectPathToTheClipboard(LigidWindow window){
    LigidGL::setClipboardText(std::filesystem::absolute(folderPath).string().c_str());
}

std::string Project::ligidFilePath(){
    return this->locateLigidFileInFolder(this->folderPath);
}

std::string Project::projectName(){
    return UTIL::getLastWordBySeparatingWithChar(this->folderPath, UTIL::folderDistinguisher());
}