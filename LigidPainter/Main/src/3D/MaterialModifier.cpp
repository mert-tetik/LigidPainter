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

void textureModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view);
void dustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view);
void solidModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view);

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
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == FABRIC_MATERIAL_MODIFIER){
        this->sections = createFabricModifier(colorPalette,buttonShader,appTextures);
        this->title = "Fabric Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/FabricModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == MARBLE_MATERIAL_MODIFIER){
        this->sections = createMarbleModifier(colorPalette,buttonShader,appTextures);
        this->title = "Marble Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MarbleModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == MOSS_MATERIAL_MODIFIER){
        this->sections = createMossModifier(colorPalette,buttonShader,appTextures);
        this->title = "Moss Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MossModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == RUST_MATERIAL_MODIFIER){
        this->sections = createRustModifier(colorPalette,buttonShader,appTextures);
        this->title = "Rust Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/RustModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == SKIN_MATERIAL_MODIFIER){
        this->sections = createSkinModifier(colorPalette,buttonShader,appTextures);
        this->title = "Skin Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/SkinModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
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
        this->updateMaterialChannels = textureModifierUpdateMat;
    }

    this->modifierIndex = modifierIndex;
}


std::vector<Section> MaterialModifier::createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    return 
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
}

std::vector<Section> MaterialModifier::createSolidModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    return 
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
}

std::vector<Section> MaterialModifier::createMarbleModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Marble Properties", Texture(), 0.f, true)),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 1", appTextures.greetingDialogImage, 0.f, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Color 2", appTextures.greetingDialogImage, 0.f, false),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                //Numeric(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Checker State", appTextures.greetingDialogImage, false),
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Crack Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Offset", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Depth", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Zebra Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Zebra Amp", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Profile", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Slope", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Width", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Crack Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "First Octave", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Octaves", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistence", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Corners", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Radius", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blackout Noise Size", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Reflectiveness", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        )
    };
}


std::vector<Section> MaterialModifier::createFabricModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Stripes",Texture(),0.f,true)),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader, "Color",   appTextures.greetingDialogImage,0.f,false),
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Frequency",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Gap Frequency",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                //Numeric(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Style",appTextures.greetingDialogImage,0.f,0,2,0), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Element Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wetness",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
    };
} 

std::vector<Section> MaterialModifier::createMossModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    return 
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
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Count", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Opacity Jitter", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Size", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Front Layer", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Front Layer Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Front Layer Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Lighting", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Light Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Darkening Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Moss Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        )
    };
}

std::vector<Section> MaterialModifier::createRustModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    return 
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
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Offset", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Seed", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Perlin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "First Octave", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Octaves", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistence", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "FBM Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Octaves", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Roughness", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Rust Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Rust Radius", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Battering Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        )
    };
}

std::vector<Section> MaterialModifier::createSkinModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Droplets", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Count", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Opacity Jitter", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Droplets Size", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Veins", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Veins Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Veins Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Blushing", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Blushing Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Skin Prints", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Prints Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Prints Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Noise", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Skin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Color Type", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Wetness", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Height", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Skin Ambient Occlusion", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        )
    };
}

std::vector<Section> MaterialModifier::createWoodenModifier(ColorPalette colorPalette, Shader buttonShader, AppTextures appTextures)
{
    return 
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
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Scale", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Noise Offset", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Seed", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Perlin Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Max Octaves", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Persistance", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Musgrave Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Lacunarity", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Musgrave Noise", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Base", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Base Noise Strength", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Base Color Saturation", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "FBM Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "FBM Frequency", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 2.f), colorPalette, buttonShader, "Element Properties", Texture(), 0.f, true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Wood Gamma", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Shininess", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Metallic", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), colorPalette, buttonShader, "Height", appTextures.greetingDialogImage, 0.f, 0.f, 500.f, 500.f), // /100
            }
        )
    };
}

std::vector<Section> MaterialModifier::createAsphaltModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Color",Texture(),0.f,true)),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Color1",              appTextures.greetingDialogImage,0.f,false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Color2",              appTextures.greetingDialogImage,0.f,false),
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Offset Intensity",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Dirt",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Strength",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"2nd Color",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Noise Scale",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Noise Strength",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Noise",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Strength",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Element Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wetness",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        ),
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Perlin Properties",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"First Octave",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Octaves",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Persistence",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
            }
        )
    };
}

std::vector<Section> MaterialModifier::createDustModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){

    return 
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
}

void textureModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view){

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,(float)textureResolution,(float)textureResolution,0.f);
    
    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        glDisable(GL_DEPTH_TEST);

        //That framebuffer will be used to get the results of the shader 
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
        //Get the channel's texture from material
        unsigned int textureBuffer; //Material's texture
        if(channelI == 0){
            textureBuffer = mesh.albedo.ID;
        }
        if(channelI == 1){
            textureBuffer = mesh.roughness.ID;
        }
        if(channelI == 2){
            textureBuffer = mesh.metallic.ID;
        }
        if(channelI == 3){
            textureBuffer = mesh.normalMap.ID;
        }
        if(channelI == 4){
            textureBuffer = mesh.heightMap.ID;
        }
        if(channelI == 5){
            textureBuffer = mesh.ambientOcclusion.ID;
        }
        
        //Bind the channel texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureBuffer);
        
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
        
        //Make the background pink (cause why not)
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
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


    }
}

glm::vec2 getDirectionVector(float rotation) {
  // Convert rotation from degrees to radians
  float radians = glm::radians(rotation);

  // Calculate the x and y components of the direction vector
  float x = glm::cos(radians);
  float y = glm::sin(radians);

  return glm::vec2(x, y);
}

void dustModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        glDisable(GL_DEPTH_TEST);

        //That framebuffer will be used to get the results of the shader 
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
        //Get the channel's texture from material
        unsigned int textureBuffer; //Material's texture
        if(channelI == 0){
            textureBuffer = mesh.albedo.ID;
        }
        if(channelI == 1){
            textureBuffer = mesh.roughness.ID;
        }
        if(channelI == 2){
            textureBuffer = mesh.metallic.ID;
        }
        if(channelI == 3){
            textureBuffer = mesh.normalMap.ID;
        }
        if(channelI == 4){
            textureBuffer = mesh.heightMap.ID;
        }
        if(channelI == 5){
            textureBuffer = mesh.ambientOcclusion.ID;
        }
        
        //Bind the channel texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureBuffer);
        
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
        
        //Make the background pink (cause why not)
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
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
        

        //TODO : Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        
        //TODO : Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);
    }
}

void solidModifierUpdateMat(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureResolution,textureResolution);

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,1.f,0.f,1.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        glDisable(GL_DEPTH_TEST);

        //That framebuffer will be used to get the results of the shader 
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
        //Get the channel's texture from material
        unsigned int textureBuffer; //Material's texture
        if(channelI == 0){
            textureBuffer = mesh.albedo.ID;
        }
        if(channelI == 1){
            textureBuffer = mesh.roughness.ID;
        }
        if(channelI == 2){
            textureBuffer = mesh.metallic.ID;
        }
        if(channelI == 3){
            textureBuffer = mesh.normalMap.ID;
        }
        if(channelI == 4){
            textureBuffer = mesh.heightMap.ID;
        }
        if(channelI == 5){
            textureBuffer = mesh.ambientOcclusion.ID;
        }
        
        //Bind the channel texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureBuffer);
        
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
        
        //Make the background pink (cause why not)
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
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
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[0].elements[channelI * 2 + 1].rangeBar.value / 100.f); //Set the texture slot
        modifierShader.setVec3( "value" , glm::vec3(
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.r, 
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.g, 
                                                        material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.b
                                                    )); //Set the texture slot

        //TODO : Bind the mask texture
        glActiveTexture(GL_TEXTURE0);
        
        //TODO : Bind the previous texture
        glActiveTexture(GL_TEXTURE1);
        
        //Render the result to the framebuffer
        mesh.Draw();
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);
    }
}