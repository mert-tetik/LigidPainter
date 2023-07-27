/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture Modifier

    Dust Modifier

    Solid Modifier

    Fabric Modifier

    Ceramic Modifier

    Leather Modifier

    Rust Modifier

    Wooden Modifier

    Brick Modifier

    Image Repeate Modifier

    Shapes Modifier

    Seams Modifier

    Cracks Modifier
    
    Dirt Modifier
    
    Line Modifier

    Skin Modifier
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

MaterialModifier::MaterialModifier(){}

void textureModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void dustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void solidModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void asphaltModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void fabricModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void marbleModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void woodenModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void mossModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void rustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);
void skinModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader);

MaterialModifier::MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex){
    
    if(modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        this->sections = createTextureModifier(colorPalette,buttonShader,appTextures);
        this->title = "Texture Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert","LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == DUST_MATERIAL_MODIFIER){
        this->sections = createDustModifier(colorPalette,buttonShader,appTextures);
        this->title = "Dust Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/DustModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = dustModifierUpdateMat;
    }
    else if(modifierIndex == ASPHALT_MATERIAL_MODIFIER){
        this->sections = createAsphaltModifier(colorPalette,buttonShader,appTextures);
        this->title = "Asphalt Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/AsphaltModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = asphaltModifierUpdateMat;
    }
    else if(modifierIndex == FABRIC_MATERIAL_MODIFIER){
        this->sections = createFabricModifier(colorPalette,buttonShader,appTextures);
        this->title = "Fabric Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/FabricModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = fabricModifierUpdateMat;
    }
    else if(modifierIndex == MARBLE_MATERIAL_MODIFIER){
        this->sections = createMarbleModifier(colorPalette,buttonShader,appTextures);
        this->title = "Marble Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MarbleModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = marbleModifierUpdateMat;
    }
    else if(modifierIndex == MOSS_MATERIAL_MODIFIER){
        this->sections = createMossModifier(colorPalette,buttonShader,appTextures);
        this->title = "Moss Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MossModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = mossModifierUpdateMat;
    }
    else if(modifierIndex == RUST_MATERIAL_MODIFIER){
        this->sections = createRustModifier(colorPalette,buttonShader,appTextures);
        this->title = "Rust Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/RustModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = rustModifierUpdateMat;
    }
    else if(modifierIndex == SKIN_MATERIAL_MODIFIER){
        this->sections = createSkinModifier(colorPalette,buttonShader,appTextures);
        this->title = "Skin Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/SkinModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = skinModifierUpdateMat;
    }
    else if(modifierIndex == SOLID_MATERIAL_MODIFIER){
        this->sections = createSolidModifier(colorPalette,buttonShader,appTextures);
        this->title = "Solid Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/SolidModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = solidModifierUpdateMat;
    }
    else if(modifierIndex == WOODEN_MATERIAL_MODIFIER){
        this->sections = createWoodenModifier(colorPalette,buttonShader,appTextures);
        this->title = "Wooden Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/WoodenModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = woodenModifierUpdateMat;
    }

    this->modifierIndex = modifierIndex;
}


std::vector<Section> MaterialModifier::createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    std::vector<Section> sections =  
    {
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

    return sections;
}

std::vector<Section> MaterialModifier::createSolidModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Channels",Texture(),0.f,true)),
            {
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Albedo",              Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Albedo Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Roughness",           Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Roughness Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",            Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Normal Map",          Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Normal Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height map",          Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Ambient Occlusion",   Texture(), 0.f, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Ambient Occlusion Opacity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f) // /100
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;
    sections[0].elements[4].button.colorSelection = true;
    sections[0].elements[6].button.colorSelection = true;
    sections[0].elements[8].button.colorSelection = true;
    sections[0].elements[10].button.colorSelection = true;

    return sections;
}

std::vector<Section> MaterialModifier::createMarbleModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Marble Properties", Texture(), 0.f, true)),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 1", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 2", appTextures.greetingDialogImage, 0.f, false),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 5.f), // /10
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Checker State", appTextures.greetingDialogImage, 0.f, 0, 3, 1),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Crack Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Offset", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 5.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Depth", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 30.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Zebra Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Zebra Amp", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 67.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Profile", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Slope", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 50.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Width", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "First Octave", appTextures.greetingDialogImage, 0.f, 0, 8, 3), // /100 
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Octaves", appTextures.greetingDialogImage, 0.f, 0, 8, 8), // /100 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistence", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 60.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 50.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Corners", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Radius", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 20.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 50.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Noise Size", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 80.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Reflectiveness", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 70.f), // /100
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(1.f), 1.f);

    return sections;
}


std::vector<Section> MaterialModifier::createFabricModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Stripes",Texture(),0.f,true)),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader, "Color",   appTextures.greetingDialogImage,0.f,false),
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,100.f,32.f), // 1
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Frequency",appTextures.greetingDialogImage,0.f,0.f,100.f,20.f), // /2
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Gap Frequency",appTextures.greetingDialogImage,0.f,0.f,100.f,60.f), // /2
                RangeBar(ELEMENT_STYLE_STYLIZED,glm::vec2(1,1.5f),colorPalette,buttonShader,"Style",appTextures.greetingDialogImage,0.f,0,2,0),  
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Element Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wetness",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",appTextures.greetingDialogImage,0.f,0.f,100.f,0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
            }
        ),
    };

    sections[0].elements[0].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.79f, 0.79f, 0.73f), 1.f);

    return sections;
} 

std::vector<Section> MaterialModifier::createMossModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Color", Texture(), 0.f, true)),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Moss Color Back", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Moss Color Front", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Dirt Color", appTextures.greetingDialogImage, 0.f, false),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Droplets", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Count", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Opacity Jitter", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Size", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 15.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Front Layer", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Front Layer Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Front Layer Scale", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 50.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Lighting", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Light Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Darkening Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Moss Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 40.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.19f, 0.19f, 0.13f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.21f, 0.27f, 0.01f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.27f, 0.22f, 0.15f), 1.f);
    

    return sections;
}

std::vector<Section> MaterialModifier::createRustModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Colors", Texture(), 0.f, true)),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 1", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 2", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 3", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 4", appTextures.greetingDialogImage, 0.f, false),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 30.f), // /1
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Perlin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "First Octave", appTextures.greetingDialogImage, 0.f, 0, 8, 3), 
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Octaves", appTextures.greetingDialogImage, 0.f, 0, 8, 8), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistence", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 60.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "FBM Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Octaves", appTextures.greetingDialogImage, 0.f, 0, 8, 8), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Roughness", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 50.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Rust Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Rust Radius", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 20.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Battering Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200, 100), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
            }
        )
    };
    
    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;
    sections[0].elements[3].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.65f, 0.65f, 0.61f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.43f, 0.4f, 0.39f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.29f, 0.22f, 0.17f), 1.f);
    sections[0].elements[3].button.color = glm::vec4(glm::vec3(0.49f, 0.3f, 0.2f), 1.f);
    

    return sections;
}

std::vector<Section> MaterialModifier::createSkinModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Droplets", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Count", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Opacity Jitter", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Size", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 15.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Veins", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Veins Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Veins Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Blushing", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blushing Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 20.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Skin Prints", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Prints Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Prints Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 20.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Skin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Color Type", appTextures.greetingDialogImage, 0.f, 0, 5, 0), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Height", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Ambient Occlusion", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 100.f), // /100
            }
        )
    };

    return sections;
}

std::vector<Section> MaterialModifier::createWoodenModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Colors", Texture(), 0.f, true)),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 1", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 2", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 3", appTextures.greetingDialogImage, 0.f, false),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Offset", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Seed", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 30.f), // *100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Perlin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Max Octaves", appTextures.greetingDialogImage, 0.f, 0, 8, 8), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistance", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 50.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Musgrave Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Lacunarity", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 25.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Strength", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 75.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Noise", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Base", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 46.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color Saturation", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "FBM Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Frequency", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 10.f), // /10
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wood Gamma", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 60.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Shininess", appTextures.greetingDialogImage, 0.f, 0.f, 200.f, 100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 100.f, 0.f), // /100
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.25f, 0.11f, 0.04f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.52f, 0.32f, 0.19f), 1.f);


    return sections;
}

std::vector<Section> MaterialModifier::createAsphaltModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Color",Texture(),0.f,true)),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Color1",              appTextures.greetingDialogImage,0.f,false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Color2",              appTextures.greetingDialogImage,0.f,false),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Dirt",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,100.f,40.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Strength",appTextures.greetingDialogImage,0.f,0.f,200.f,100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"2nd Color",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Noise Scale",appTextures.greetingDialogImage,0.f,0.f,100.f,50.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Noise Strength",appTextures.greetingDialogImage,0.f,0.f,200.f,100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Noise",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Strength",appTextures.greetingDialogImage,0.f,0.f,200.f,100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Element Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wetness",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",appTextures.greetingDialogImage,0.f,0.f,100.f,0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Perlin Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED,glm::vec2(1,1.5f),colorPalette,buttonShader,"First Octave",appTextures.greetingDialogImage,0.f, 0, 8, 3),
                RangeBar(ELEMENT_STYLE_STYLIZED,glm::vec2(1,1.5f),colorPalette,buttonShader,"Octaves",appTextures.greetingDialogImage,0.f, 0, 8, 8), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Persistence",appTextures.greetingDialogImage,0.f,0.f,100.f,70.f), // /100
            }
        )
    };
    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    
    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.25f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.23f, 0.25f, 0.27f), 1.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createDustModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){

    std::vector<Section> sections =  
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Noise",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Size",appTextures.greetingDialogImage,0.f,0.f,100.f,20.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Offset Intensity",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Rotation",appTextures.greetingDialogImage,0.f,0.f,360.f,360.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Brightness",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
            }
        ),
        
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Blur",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Intensity",appTextures.greetingDialogImage,0.f,0.f,100.f,0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Direction",appTextures.greetingDialogImage,0.f,0.f,360.f,0.f), // /1
            }
        ),
        
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Scratches",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wavyness",appTextures.greetingDialogImage,0.f,0.f,100.f,10.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,100.f,30.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Base Frequency",appTextures.greetingDialogImage,0.f,0.f,100.f,50.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"FrequencyStep",appTextures.greetingDialogImage,0.f,0.f,100.f,25.f), // /100
            }
        ),

        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Droplets",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Count",appTextures.greetingDialogImage,0.f,0.f,100.f,10.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"OpacityJitter",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Size",appTextures.greetingDialogImage,0.f,0.f,100.f,50.f), // /10
            }
        ),
    };

    return sections;
}

void channelPrep(Material &material, Mesh &mesh, int& textureResolution, int& curModI, glm::mat4& perspective, glm::mat4& view, int& channelI, unsigned int& FBO, Texture& currentTexture, Texture& previousTexture){
    glDisable(GL_DEPTH_TEST);

    //Get the channel's texture from material
    
    if(channelI == 0){
        currentTexture = mesh.albedo;
    }
    if(channelI == 1){
        currentTexture = mesh.roughness;
    }
    if(channelI == 2){
        currentTexture = mesh.metallic;
    }
    if(channelI == 3){
        currentTexture = mesh.normalMap;
    }
    if(channelI == 4){
        currentTexture = mesh.heightMap;
    }
    if(channelI == 5){
        currentTexture = mesh.ambientOcclusion;
    }

    /* ! Binds another framebuffer ! */
    previousTexture = currentTexture.duplicateTexture();

    //That framebuffer will be used to get the results of the shader 
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);

    //Bind the channel texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture.ID);
    
    //Params for the channel texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    
    //Refresh the channel texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureResolution, textureResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Bind the channel texture to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currentTexture.ID, 0);
    
    //Make the background pink (cause why not)
    glClearColor(1,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void generateNormalMap(unsigned int& heightMap, unsigned int& normalMap, Shader heightToNormalMapShader, int textureResolution){
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0.5,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Box box;
    box.init();
    box.bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    heightToNormalMapShader.use();
    heightToNormalMapShader.setInt("heightMap", 0);
    heightToNormalMapShader.setMat4("projection"  ,       projection);
    heightToNormalMapShader.setMat4("projectedPosProjection"  ,       projection);
    heightToNormalMapShader.setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    heightToNormalMapShader.setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightMap);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
}

void removeSeams(Mesh& mesh, unsigned int texture, int textureResolution, Shader boundaryExpandingShader){
    
    /*! Binds another framebuffer !*/
    Texture textureObject = Texture(texture);
    unsigned int textureCopy = textureObject.duplicateTexture();
    
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0.5,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Box box;
    box.init();
    box.bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    boundaryExpandingShader.use();
    boundaryExpandingShader.setMat4("projection"  ,       projection);
    boundaryExpandingShader.setMat4("projectedPosProjection"  ,       projection);
    boundaryExpandingShader.setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    boundaryExpandingShader.setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));

    boundaryExpandingShader.setInt("whiteUVTexture", 0);
    boundaryExpandingShader.setInt("originalTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureCopy);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureCopy);
}

glm::vec2 getDirectionVector(float rotation) {
  // Convert rotation from degrees to radians
  float radians = glm::radians(rotation);

  // Calculate the x and y components of the direction vector
  float x = glm::cos(radians);
  float y = glm::sin(radians);

  return glm::vec2(x, y);
}


void textureModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,(float)textureResolution,(float)textureResolution,0.f);
    
    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);

        //Set the uniforms of the modifier's shader
        material.materialModifiers[curModI].shader.use(); //Use the shader of the modifier
        material.materialModifiers[curModI].shader.setMat4("projection",projection); //Set the projection
        material.materialModifiers[curModI].shader.setVec2("scale",fragScale); //Set the scale
        material.materialModifiers[curModI].shader.setVec3("pos",fragPos); //Set the position
        material.materialModifiers[curModI].shader.setInt("state",channelI); //Set the texture slot
        material.materialModifiers[curModI].shader.setInt("theTexture",0); //Set the texture slot

        //Bind the texture (bind the channel textures if rendering a texture modifier & bind the result of the previous modifier)
        glActiveTexture(GL_TEXTURE0);

        //Bind the texture from the materialModifier's button element texture
        glBindTexture(GL_TEXTURE_2D,material.materialModifiers[curModI].sections[0].elements[channelI].button.texture.ID);
        
        //Render the result to the framebuffer
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void dustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); // TODO : Set the opacity

        /* Noise */
        modifierShader.setFloat("size", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value / 10.f); 
        modifierShader.setFloat("offsetIntensity", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value / 10.f); 
        modifierShader.setFloat("rotation", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value); 
        modifierShader.setFloat("brightness", material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value / 100.f); 
        /* Blur */
        modifierShader.setFloat("blurIntensity", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 100.f);
        modifierShader.setVec2("blurDirection", getDirectionVector(material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value));
        /* Scratches */
        modifierShader.setFloat("scratchesWavyness", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f); 
        modifierShader.setVec2("scratchesScale", glm::vec2(material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 10.f));
        modifierShader.setVec2("scratchesBaseFrequency", glm::vec2(material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 100.f));
        modifierShader.setVec2("scratchesFrequencyStep", glm::vec2(material.materialModifiers[curModI].sections[2].elements[3].rangeBar.value / 100.f));
        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f); 
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 10.f);
        

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);

        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }

        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void solidModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state",channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[0].elements[channelI * 2 + 1].rangeBar.value / 100.f); //Set the chammeş opacity
        modifierShader.setVec3( "value" , glm::vec3(
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.r, 
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.g, 
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.b
                                                    )); //Set the channel color value

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void asphaltModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Colors */
        modifierShader.setVec3("asphaltColor", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("asphaltColor2", material.materialModifiers[curModI].sections[0].elements[1].button.color);

        /* Dirt */
        modifierShader.setFloat("dirtScale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("dirtStrength", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value / 100.f);

        /* 2nd Color */
        modifierShader.setFloat("colorNoiseScale", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("colorNoiseStrength", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 100.f);

        /* Noise */ 
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 100.f);

        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[4].elements[2].rangeBar.value / 100.f);

        /* Perlin Noise Properties*/
        modifierShader.setInt("firstOctave", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[5].elements[1].rangeBar.value); 
        modifierShader.setFloat("persistence", material.materialModifiers[curModI].sections[5].elements[2].rangeBar.value / 100.f); 

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void fabricModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Stripes */
        modifierShader.setVec3("color", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value / 1.f);
        modifierShader.setFloat("stripeFrequency", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value / 2.f);
        modifierShader.setFloat("gapFrequency", material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value / 2.f);
        modifierShader.setInt("style", material.materialModifiers[curModI].sections[0].elements[4].rangeBar.value);

        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value / 100.f);


        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void marbleModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Marble Properties */
        modifierShader.setVec3("color1", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("color2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value);
        modifierShader.setInt("checkerState", material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value); 

        /* Crack Properties*/
        modifierShader.setFloat("ofs", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("crackDepth", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value / 10.f);
        modifierShader.setFloat("crackZebraScale", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value / 10.f); 
        modifierShader.setFloat("crackZebraAmp", material.materialModifiers[curModI].sections[1].elements[3].rangeBar.value / 100.f);
        modifierShader.setFloat("crackProfile", material.materialModifiers[curModI].sections[1].elements[4].rangeBar.value / 10.f);
        modifierShader.setFloat("crackSlope", material.materialModifiers[curModI].sections[1].elements[5].rangeBar.value / 1.f);
        modifierShader.setFloat("crackWidth", material.materialModifiers[curModI].sections[1].elements[6].rangeBar.value / 10.f);
        modifierShader.setFloat("crackScale", material.materialModifiers[curModI].sections[1].elements[7].rangeBar.value / 1.f);

        /* Noise Properties */
        modifierShader.setInt("firstOctave", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        modifierShader.setFloat("persistence", material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 100.f);
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[2].elements[3].rangeBar.value / 100.f);

        /* Corners */
        modifierShader.setFloat("cornerBlackoutRadius", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("cornerBlackoutStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value / 10.f);
        modifierShader.setFloat("cornerBlackoutNoiseSize", material.materialModifiers[curModI].sections[3].elements[2].rangeBar.value / 10.f);

        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[4].elements[2].rangeBar.value / 100.f);


        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void woodenModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Colors */
        modifierShader.setVec3("woodColor1", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("woodColor2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("woodColor3", material.materialModifiers[curModI].sections[0].elements[2].button.color);

        /* Properties */
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("noiseOffset", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value / 10.f);
        modifierShader.setFloat("seed", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value * 100.f);

        /* Perlin Properties */
        modifierShader.setInt("maxOctaves", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setFloat("persistance", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 100.f);

        /* Musgrave Properties */
        modifierShader.setFloat("musgraveLacunarity", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("musgraveStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("musgraveNoise", material.materialModifiers[curModI].sections[3].elements[2].rangeBar.value / 10.f);

        /* Base */
        modifierShader.setFloat("baseNoiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("baseColorSaturation", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value / 100.f);

        /* FBM Properties */
        modifierShader.setFloat("fbmFrequency", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value / 10.f);

        /* Element property */
        modifierShader.setFloat("woodGamma", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[3].rangeBar.value / 100.f);

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void mossModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Color */
        modifierShader.setVec3("mossColorBack", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("mossColorFront", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("dirtColor", material.materialModifiers[curModI].sections[0].elements[2].button.color);

        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 1.f);
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 10.f);

        /* Front Layer */
        modifierShader.setFloat("frontLayerStrength", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("frontLayerScale", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f);

        /* Lighting */
        modifierShader.setFloat("lightStrength", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("darkeningStrength", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 100.f);

        /* Noise */
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 100.f);

        /* Moss Properties*/
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value / 1.f);

        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value / 100.f);

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void rustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.); //TODO : Set the opacity

        /* Colors */
        modifierShader.setVec3("color1", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("color2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("color3", material.materialModifiers[curModI].sections[0].elements[2].button.color);
        modifierShader.setVec3("color4", material.materialModifiers[curModI].sections[0].elements[3].button.color);

        /*Properties*/
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 1.f);

        /* Perlin Properties */
        modifierShader.setInt("firstOctave", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        modifierShader.setFloat("persistence", material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 100.f); 

        /* FBM Properties*/
        modifierShader.setInt("fbmOctaves", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value); 
        modifierShader.setFloat("fbmRoughness", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value / 100.f);

        /* Rust Properties */
        modifierShader.setFloat("rustRadius", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 10.f);
        modifierShader.setFloat("batteringStrength", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value / 100.f);

        /* Noise Properties*/
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value / 100.f);

        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value / 100.f);

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}

void skinModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view, Shader heightToNormalShader){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //TODO Don't create the shader in the modifier function
    Shader boundaryExpandingShader = Shader("./LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "./LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag", nullptr, nullptr, nullptr);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        unsigned int FBO;
        Texture currentTexture;
        Texture previousTexture;
        channelPrep(material, mesh, textureResolution, curModI, perspective, view, channelI, FBO, currentTexture, previousTexture);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("orthoProjection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        modifierShader.setMat4("perspectiveProjection",perspective); //Set the projection
        modifierShader.setMat4("view",view); //Set the projection

        /* Channel Properties */
        modifierShader.setInt("state", channelI); //Set the channel state
        modifierShader.setInt("mask", 0); //Set the mask texture slot
        modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
        modifierShader.setFloat( "opacity" , 1.f); //TODO : Set the opacity
        modifierShader.setInt("proceduralID", material.materialModifiers[curModI].maskTexture.proceduralID); //Set the channel state

        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value / 1.f);
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 100.f);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value / 10.f);

        /* Veins */
        modifierShader.setFloat("veinsScale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 1.f);
        modifierShader.setFloat("veinsStrength", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value / 100.f);

        /* Blushing */
        modifierShader.setFloat("blushingStrength", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 10.f);

        /* Skin Prints */
        modifierShader.setFloat("skinPrintsScale", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value / 1.f);
        modifierShader.setFloat("skinPrintsStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value / 100.f);

        /* Noise */
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value / 10.f);

        /* Skin Properties*/
        modifierShader.setInt("skinColorType", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        modifierShader.setFloat("skinScale", material.materialModifiers[curModI].sections[5].elements[1].rangeBar.value / 1.f);
        modifierShader.setFloat("skinWetness", material.materialModifiers[curModI].sections[5].elements[2].rangeBar.value / 100.f);
        modifierShader.setFloat("skinMetallic", material.materialModifiers[curModI].sections[5].elements[3].rangeBar.value / 100.f);
        modifierShader.setFloat("skinHeight", material.materialModifiers[curModI].sections[5].elements[4].rangeBar.value / 100.f);
        modifierShader.setFloat("skinAmbientOcclusion", material.materialModifiers[curModI].sections[5].elements[5].rangeBar.value / 100.f);

        // Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.materialModifiers[curModI].maskTexture.ID);
        
        //Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        
        //Generating the normal map
        if(channelI == 4){
            generateNormalMap(mesh.heightMap.ID, mesh.normalMap.ID, heightToNormalShader, textureResolution);
            removeSeams(mesh, mesh.normalMap.ID, textureResolution, boundaryExpandingShader);
        }
        glEnable(GL_DEPTH_TEST);

        removeSeams(mesh, currentTexture.ID, textureResolution, boundaryExpandingShader);
        glDeleteTextures(1, &previousTexture.ID);
    }

    glDeleteProgram(boundaryExpandingShader.ID);
}