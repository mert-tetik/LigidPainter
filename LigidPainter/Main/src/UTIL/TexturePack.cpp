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

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

TexturePack::TexturePack(){

}

int TexturePack::load(std::string path){

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            Texture txtr;
            txtr.load(entry.path().string().c_str());

            this->textures.push_back(txtr);
        }     
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    this->title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());

    return 1;
}