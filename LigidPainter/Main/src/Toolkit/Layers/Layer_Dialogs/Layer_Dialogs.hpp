/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIGID_LAYER_DIALOGS_HPP
#define LIGID_LAYER_DIALOGS_HPP

#include <glad/glad.h>
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

static DialogControl alpha_settings_dialog_control;

static Panel alpha_settings_panel = Panel(
                                {
                                    Section(
                                        Element(),
                                        {   
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "General Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //0
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "General Opacity Map"  , Texture(), 0.f, false), //1
                                            
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Albedo Opacity"  , Texture(), 4.f, 0.f, 1.f, 1.f), //2
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Albedo Opacity Map"  , Texture(), 0.f, false), //3
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Roughness Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //4
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Roughness Opacity Map"  , Texture(), 0.f, false), //5
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Metallic Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //6
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Metallic Opacity Map"  , Texture(), 0.f, false), //7
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Normal Map Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //8
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Normal Map Opacity Map"  , Texture(), 0.f, false), //9
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Height Map Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //10
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Height Map Opacity Map"  , Texture(), 0.f, false), //11
                                            RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Ambient Occlusion Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //12
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Ambient Occlusion Opacity Map"  , Texture(), 0.f, false), //13
                                        }
                                    )
                                }, 
                                glm::vec2(15.f, 20.f), 
                                glm::vec3(50.f, 50.f, 0.7), 
                                ColorPalette::secondColor, 
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

static void LAYERDIALOG_show_alpha_settings_dialog(Timer& timer, LayerAlpha* layer_alpha, Button layer_btn, unsigned int resolution, Mesh& mesh){
    alpha_settings_dialog_control.activate();

    while (!getContext()->window.shouldClose())
    {
        alpha_settings_dialog_control.updateStart(true);
       
        /* Set alpha map texture selection button's texture selection mode flag to true */
        alpha_settings_panel.sections[0].elements[1].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[3].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[5].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[7].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[9].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[11].button.textureSelection3D = true;
        alpha_settings_panel.sections[0].elements[13].button.textureSelection3D = true;

        /* Set layer alpha data to panel*/
        alpha_settings_panel.sections[0].elements[0].rangeBar.value = layer_alpha->general_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[1].button.texture = layer_alpha->general_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[2].rangeBar.value = layer_alpha->albedo_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[3].button.texture = layer_alpha->albedo_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[4].rangeBar.value = layer_alpha->roughness_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[5].button.texture = layer_alpha->roughness_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[6].rangeBar.value = layer_alpha->metallic_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[7].button.texture = layer_alpha->metallic_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[8].rangeBar.value = layer_alpha->normalMap_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[9].button.texture = layer_alpha->normalMap_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[10].rangeBar.value = layer_alpha->heightMap_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[11].button.texture = layer_alpha->heightMap_Alpha.alphaMap;
        alpha_settings_panel.sections[0].elements[12].rangeBar.value = layer_alpha->ambientOcclusion_Alpha.alphaValue;
        alpha_settings_panel.sections[0].elements[13].button.texture = layer_alpha->ambientOcclusion_Alpha.alphaMap;

        /* Transform the panel and render the panel*/
        alpha_settings_panel.pos = layer_btn.pos;
        alpha_settings_panel.pos.x -= layer_btn.scale.x + alpha_settings_panel.scale.x;
        alpha_settings_panel.pos.y -= layer_btn.scale.y - alpha_settings_panel.scale.y;
        alpha_settings_panel.render(timer, !dialog_log.isHovered());

        /* Set properties back to layer alpha data*/
        layer_alpha->general_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[0].rangeBar.value;
        layer_alpha->general_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[1].button.texture;
        layer_alpha->albedo_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[2].rangeBar.value;
        layer_alpha->albedo_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[3].button.texture;
        layer_alpha->roughness_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[4].rangeBar.value;
        layer_alpha->roughness_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[5].button.texture;
        layer_alpha->metallic_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[6].rangeBar.value;
        layer_alpha->metallic_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[7].button.texture;
        layer_alpha->normalMap_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[8].rangeBar.value;
        layer_alpha->normalMap_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[9].button.texture;
        layer_alpha->heightMap_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[10].rangeBar.value;
        layer_alpha->heightMap_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[11].button.texture;
        layer_alpha->ambientOcclusion_Alpha.alphaValue = alpha_settings_panel.sections[0].elements[12].rangeBar.value;
        layer_alpha->ambientOcclusion_Alpha.alphaMap = alpha_settings_panel.sections[0].elements[13].button.texture;
        
        if((!alpha_settings_panel.hover && *Mouse::LClick()) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE))
            alpha_settings_dialog_control.unActivate();
        
        alpha_settings_dialog_control.updateEnd(timer,0.15f);        

        // Quit the alpha settings dialog
        if(alpha_settings_dialog_control.mixVal == 0.f){
            layer_alpha->update_buffers(resolution, mesh);
            mesh.layerScene.update_result(resolution, glm::vec3(0.f), mesh);
            break;
        }
    }
}


static DialogControl layer_info_dialog_control;

/*! @brief This panel used for displaying the properties of a layer*/
Panel layer_info_panel = Panel(
                            {
                                Section(
                                    Element(),
                                    {   
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Title : "  , Texture(), 0.f, false), //0
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Type : "  , Texture(), 0.f, false), //1
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "General Mask Texture"  , Texture(), 0.f, false), //2
                                        
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "ALBEDO"  , Texture(), 2.f, false), //3
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //4
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //5

                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "ROUGHNESS"  , Texture(), 1.f, false), //6
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //7
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //8

                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "METALLIC"  , Texture(), 1.f, false), //9
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //10
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //11

                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "NORMAL MAP"  , Texture(), 1.f, false), //12
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //13
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //14

                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "HEIGHT MAP"  , Texture(), 1.f, false), //15
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //16
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //17

                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "AMBIENT OCCLUSION"  , Texture(), 1.f, false), //18
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Mask Texture"  , Texture(), 0.f, false), //19
                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //20
                                    }
                                )
                            }, 
                            glm::vec2(15.f, 20.f), 
                            glm::vec3(50.f, 50.f, 0.7), 
                            ColorPalette::mainColor, 
                            ColorPalette::thirdColor, 
                            true, 
                            true, 
                            true, 
                            true, 
                            true, 
                            1.f, 
                            1, 
                            {}, 
                            20.f, 
                            true
                        );

static void info_panel_render_eye(Timer& timer, Element button){
    Button detailed_display_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", appTextures.eyeOpenedIcon, 0.f, false); 
    
    detailed_display_btn.pos = glm::vec3(button.pos.x + button.scale.x - detailed_display_btn.scale.x-0.5f, button.pos.y - button.scale.y + detailed_display_btn.scale.y+0.5f, button.pos.z); detailed_display_btn.update_result_transform_values();
    
    if(Mouse::isMouseHover(detailed_display_btn.resultScale, detailed_display_btn.resultPos))
        detailed_display_btn.scale *= 1.5f;

    detailed_display_btn.color.a = 0.f; detailed_display_btn.color2.a = 0.f; detailed_display_btn.outlineExtra = false; 

    detailed_display_btn.render(timer, !dialog_log.isHovered());

    if(detailed_display_btn.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, button.button.texture);
    }
}

static void LAYERDIALOG_show_info_dialog(Timer& timer, Layer* layer, Button layer_btn, unsigned int resolution, Mesh& mesh){

    layer_info_dialog_control.activate();

    while (!getContext()->window.shouldClose())
    {
        layer_info_dialog_control.updateStart(true);
    
        /* Set main layer properties */
        layer_info_panel.sections[0].elements[0].button.text = "Title : " + layer->title;
        layer_info_panel.sections[0].elements[1].button.text = "Type : " + layer->layerType;
        layer_info_panel.sections[0].elements[1].button.texture = layer->layerIcon;

        /* Set layer alpha/opacity maps */
        layer_info_panel.sections[0].elements[2].button.texture = layer->alpha.general_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[4].button.texture = layer->alpha.albedo_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[7].button.texture = layer->alpha.roughness_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[10].button.texture = layer->alpha.metallic_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[13].button.texture = layer->alpha.normalMap_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[16].button.texture = layer->alpha.heightMap_Alpha.alphaMapProceduralTxtr;
        layer_info_panel.sections[0].elements[19].button.texture = layer->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr;
        
        /* Set layer result texture properties */
        layer_info_panel.sections[0].elements[5].button.texture = layer->result.albedo;
        layer_info_panel.sections[0].elements[8].button.texture = layer->result.roughness;
        layer_info_panel.sections[0].elements[11].button.texture = layer->result.metallic;
        layer_info_panel.sections[0].elements[14].button.texture = layer->result.normalMap;
        layer_info_panel.sections[0].elements[17].button.texture = layer->result.heightMap;
        layer_info_panel.sections[0].elements[20].button.texture = layer->result.ambientOcclusion;  
        
        /* Set transition colors of the buttons to their original colors*/
        for (Element& element : layer_info_panel.sections[0].elements)
        {
            element.button.color2 = element.button.color;
        }
    
        /* Transform and render the info panel */
        layer_info_panel.pos = layer_btn.pos;
        layer_info_panel.pos.x -= layer_btn.scale.x + layer_info_panel.scale.x;
        layer_info_panel.pos.y -= layer_btn.scale.y - layer_info_panel.scale.y;
        layer_info_panel.clearDepthBuffer = false;
        layer_info_panel.render(timer, !dialog_log.isHovered());

        /* Render close-up display buttons for result textures*/
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[5]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[8]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[11]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[14]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[17]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[20]);
        
        /* Render close-up display buttons for alpha maps*/
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[2]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[4]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[7]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[10]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[13]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[16]);
        info_panel_render_eye(timer, layer_info_panel.sections[0].elements[19]);

        /* Clear the depth buffer manually since we set the clearDepthBuffer flag to false*/
        glClear(GL_DEPTH_BUFFER_BIT);
        
        if((!layer_info_panel.hover && *Mouse::LClick()) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE))
            layer_info_dialog_control.unActivate();

        layer_info_dialog_control.updateEnd(timer,0.15f);

        /* Quit the layer info dialog*/
        if(layer_info_dialog_control.mixVal == 0.f){
            break;
        }
    }
}

#endif