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

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

MaterialModifier::MaterialModifier(){}


MaterialModifier::MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex){
    
    if(modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        this->sections = createTextureModifier(colorPalette,buttonShader,appTextures);
        this->title = "Texture Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag",nullptr);
    }

    this->modifierIndex = modifierIndex;
}


std::vector<Section> MaterialModifier::createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    return {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Channels",Texture(),0.f,true)),
            {
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Albedo",              appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Roughness",           appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",            appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Normal map",          appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height map",          appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Ambient Occlusion",   appTextures.greetingDialogImage,0.f,false))
            }
        )
    };
}