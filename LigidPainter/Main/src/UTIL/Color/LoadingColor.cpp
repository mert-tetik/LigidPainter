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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "UTIL/Util.hpp"

Color::Color(){
    this->priv_rgb = glm::vec3(0);
}
Color::Color(glm::vec3 rgbColor){  
    loadRGB(rgbColor);
}
Color::Color(std::string hexColor){  
    loadHex(hexColor);
}

void Color::loadRGB(glm::vec3 value){
    // Load the rgb value directly
    this->priv_rgb = value;
}

void Color::loadRGB_normalized(glm::vec3 value){
    // Load the rgb value by scaling the range 0 - 1 to 0 - 255
    this->priv_rgb = value * 255.f;
}

void Color::loadHSV(glm::vec3 value){
    // Load the hsv value by converting the hsv to rgb 
    this->priv_rgb = this->hsvToRgb(value);
}

void Color::loadHex(std::string value){
    // Load the hex value by converting the hex to rgb 
    this->priv_rgb = this->hexToRgb(value);
}

glm::vec3 Color::getRGB(){
    return this->priv_rgb;
}  
    
glm::vec3 Color::getHSV(){
    return this->rgbToHsv(this->priv_rgb);
}
    
glm::vec3 Color::getRGB_normalized(){
    return this->priv_rgb / 255.f;
}
    
std::string Color::getHEX(){
    return this->rgbToHex(this->priv_rgb);
}