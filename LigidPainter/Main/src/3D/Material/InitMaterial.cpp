/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

Material::Material(std::string title, std::vector<MaterialModifier> materialModifiers){
    //Resolution of the material displaying texture
    const int displayRes = 128;
    
    this->title = title;
    
    this->materialModifiers = materialModifiers;

    //Init displaying texture
    this->displayingTexture = Texture(nullptr, displayRes, displayRes, GL_LINEAR);
    this->displayingTexture.title = "MaterialDisplay";

    this->updateMaterialDisplayingTexture(displayRes, true, Camera(), 0, false);
    
    Settings::defaultFramebuffer()->FBO.bind();
}