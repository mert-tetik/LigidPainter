/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
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
#include "GUI/Dialogs.hpp"
#include "Toolkit/Layers/Layers.hpp"

TextureLayer::TextureLayer(const unsigned int resolution){
    this->title = "Texture Layer";
    this->layerType = "texture";
    this->layerIcon = appTextures.textureIcon;
    this->generate_layer_buffers(resolution);

    this->textureSelectPanel = Panel(
                                    {
                                        Section(
                                            Element(),
                                            {   
                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "ALBEDO"  , Texture(), 0.f, false), //0
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //1

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "ROUGHNESS"  , Texture(), 1.f, false), //2
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //3

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "METALLIC"  , Texture(), 1.f, false), //4
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //5

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "NORMAL MAP"  , Texture(), 1.f, false), //6
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //7

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "HEIGHT MAP"  , Texture(), 1.f, false), //8
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //9

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), "AO"  , Texture(), 1.f, false), //10
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(3.f), ""  , Texture(), 0.f, false), //11
                                            }
                                        )
                                    }, 
                                    glm::vec2(10.f, 21.f), 
                                    glm::vec3(50.f, 50.f, 0.7), 
                                    ColorPalette::mainColor, 
                                    ColorPalette::thirdColor, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    1.f, 
                                    2, 
                                    {}, 
                                    20.f, 
                                    true
                                );
}

static DialogControl texture_select_dialog_control;

void TextureLayer::type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    texture_select_dialog_control.activate();

    while (!getContext()->window.shouldClose())
    {
        textureSelectPanel.sections[0].elements[1].button.textureSelection3D = true;
        textureSelectPanel.sections[0].elements[3].button.textureSelection3D = true;
        textureSelectPanel.sections[0].elements[5].button.textureSelection3D = true;
        textureSelectPanel.sections[0].elements[7].button.textureSelection3D = true;
        textureSelectPanel.sections[0].elements[9].button.textureSelection3D = true;
        textureSelectPanel.sections[0].elements[11].button.textureSelection3D = true;
        
        textureSelectPanel.sections[0].elements[1].button.texture = this->channels.albedo;
        textureSelectPanel.sections[0].elements[3].button.texture = this->channels.roughness;
        textureSelectPanel.sections[0].elements[5].button.texture = this->channels.metallic;
        textureSelectPanel.sections[0].elements[7].button.texture = this->channels.normalMap;
        textureSelectPanel.sections[0].elements[9].button.texture = this->channels.heightMap;
        textureSelectPanel.sections[0].elements[11].button.texture = this->channels.ambientOcclusion;
        
        textureSelectPanel.pos = this->layerGUI.layerButton.pos;
        textureSelectPanel.pos.x -= this->layerGUI.layerButton.scale.x + textureSelectPanel.scale.x;
        textureSelectPanel.pos.y -= this->layerGUI.layerButton.scale.y - textureSelectPanel.scale.y;
        textureSelectPanel.render(timer, doMouseTracking);
        
        this->channels.albedo = textureSelectPanel.sections[0].elements[1].button.texture;
        this->channels.roughness = textureSelectPanel.sections[0].elements[3].button.texture;
        this->channels.metallic = textureSelectPanel.sections[0].elements[5].button.texture;
        this->channels.normalMap = textureSelectPanel.sections[0].elements[7].button.texture;
        this->channels.heightMap = textureSelectPanel.sections[0].elements[9].button.texture;
        this->channels.ambientOcclusion = textureSelectPanel.sections[0].elements[11].button.texture;

        if((!textureSelectPanel.hover && *Mouse::LClick()) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE))
            texture_select_dialog_control.unActivate();
        
        texture_select_dialog_control.updateEnd(timer,0.15f);

        /* Quit the layer info dialog*/
        if(texture_select_dialog_control.mixVal == 0.f){
            this->update_result_buffers(resolution, mesh);
            this->type_specific_modification_enabled = false;
            break;
        }
    }
}

void TextureLayer::type_specific_generate_result(const unsigned int resolution, Mesh& mesh){
    this->channels.albedo.generateProceduralTexture(mesh, this->result.albedo, resolution);
    this->channels.roughness.generateProceduralTexture(mesh, this->result.roughness, resolution);
    this->channels.metallic.generateProceduralTexture(mesh, this->result.metallic, resolution);
    this->channels.normalMap.generateProceduralTexture(mesh, this->result.normalMap, resolution);
    this->channels.heightMap.generateProceduralTexture(mesh, this->result.heightMap, resolution);
    this->channels.ambientOcclusion.generateProceduralTexture(mesh, this->result.ambientOcclusion, resolution);

    getScene()->get_selected_mesh()->layerScene.update_result(resolution, glm::vec3(0.f), mesh);
}