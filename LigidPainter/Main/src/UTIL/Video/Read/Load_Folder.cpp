/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Video/Video.hpp"

void Video::load_folder(const std::string folder_path, int fps){
    
    this->fps = fps;

    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            std::string file_path = entry.path().string();
            if(UTIL::getFileExtension(file_path) == "png" || UTIL::getFileExtension(file_path) == "jpg" || UTIL::getFileExtension(file_path) == "jpeg"){
                Texture txtr;
                int w, h, c;
                unsigned char* pxs = txtr.getTextureDataViaPath(file_path.c_str(), w, h, c, 4, false);
                this->frames.push_back(pxs);

                this->resolution = glm::ivec2(w,h);
                this->channels = c;
            }
        }
    }

    this->generate_color_buffer();
}