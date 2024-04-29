/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Textures of the Texture Modifier's texture channel buttons are deleted (indexed specially)
    Remove the filter button is indexed specially

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

MaterialModifier::MaterialModifier(){}

MaterialModifier::MaterialModifier(int modifierIndex){    
    
    /* Init the mask texture */
    char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
    this->maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
    this->maskTexture.proceduralProps.proceduralID = 24; //Solid white
    this->maskTexture.proceduralProps.proceduralnverted = 0;
    this->maskTexture.proceduralProps.proceduralScale = 1.f;
    this->maskTexture.title = "AutoGeneratedMask";


    if(modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        this->sections = createTextureModifier();
        this->title = "Texture Modifier";
        this->shader = MaterialModifierShaders::textureModifierShader();       
    }
    else if(modifierIndex == DUST_MATERIAL_MODIFIER){
        this->sections = createDustModifier();
        this->title = "Dust Modifier";
        this->shader = MaterialModifierShaders::dustModifierShader();    
    }
    else if(modifierIndex == ASPHALT_MATERIAL_MODIFIER){
        this->sections = createAsphaltModifier();
        this->title = "Asphalt Modifier";
        this->shader = MaterialModifierShaders::asphaltModifierShader();    
    }
    else if(modifierIndex == LIQUID_MATERIAL_MODIFIER){
        this->sections = createLiquidModifier();
        this->title = "Liquid Modifier";
        this->shader = MaterialModifierShaders::liquidModifierShader();    
    }
    else if(modifierIndex == MOSS_MATERIAL_MODIFIER){
        this->sections = createMossModifier();
        this->title = "Moss Modifier";
        this->shader = MaterialModifierShaders::mossModifierShader();    
    }
    else if(modifierIndex == RUST_MATERIAL_MODIFIER){
        this->sections = createRustModifier();
        this->title = "Rust Modifier";
        this->shader = MaterialModifierShaders::rustModifierShader();    
    }
    else if(modifierIndex == SKIN_MATERIAL_MODIFIER){
        this->sections = createSkinModifier();
        this->title = "Skin Modifier";
        this->shader = MaterialModifierShaders::skinModifierShader();    
    }
    else if(modifierIndex == SOLID_MATERIAL_MODIFIER){
        this->sections = createSolidModifier();
        this->title = "Solid Modifier";
        this->shader = MaterialModifierShaders::solidModifierShader();    
    }
    else if(modifierIndex == WOODEN_MATERIAL_MODIFIER){
        this->sections = createWoodenModifier();
        this->title = "Wooden Modifier";
        this->shader = MaterialModifierShaders::woodenModifierShader();    
    }
    else if(modifierIndex == MATH_MATERIAL_MODIFIER){
        this->sections = createMathModifier();
        this->title = "Math Modifier";
        this->shader = MaterialModifierShaders::mathModifierShader();    
    }

    this->modifierIndex = modifierIndex;
}