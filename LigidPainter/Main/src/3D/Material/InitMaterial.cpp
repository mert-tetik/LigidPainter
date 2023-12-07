/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Material class constructor

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

//TODO Handle unique ids better

Material::Material(){}

Material::Material(std::string title, int ID){
    //Resolution of the material displaying texture
    const int displayRes = 128;
    
    this->title = title;
    this->uniqueID = ID;
    
    //Init displaying texture
    this->displayingTexture = Texture(nullptr, displayRes, displayRes, GL_LINEAR);
    this->displayingTexture.title = "MaterialDisplay";

    //Capturing framebuffer
    this->displayingFBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)));

    Settings::defaultFramebuffer()->FBO.bind();
}