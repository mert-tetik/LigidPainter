/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIGID_LAYERS_HPP
#define LIGID_LAYERS_HPP

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"
#include "ContextMenuSystem/ContextMenus.hpp"
#include "MouseSystem/Mouse.hpp"

struct MaterialChannels{
    Texture albedo;
    Texture roughness;
    Texture metallic;
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;
};

/*! @brief Handles the alpha value of a channel*/
struct ChannelAlpha{
    /*! @brief Alpha map (basically opacity map)*/
    Texture alphaMap;
    /*! @brief Alpha value (basically opacity)*/
    float alphaValue = 1.f;
};

struct LayerAlpha{
    ChannelAlpha general_Alpha;

    ChannelAlpha albedo_Alpha;
    ChannelAlpha roughness_Alpha;
    ChannelAlpha metallic_Alpha;
    ChannelAlpha normalMap_Alpha;
    ChannelAlpha heightMap_Alpha;
    ChannelAlpha ambientOcclusion_Alpha;
};

/*! 
    Base class
    
    @brief

    All kinds of layers modifiy the @ref result
*/
class Layer {
public:
    /*! @brief Handle opacity of the layer */
    LayerAlpha alpha;
    /*! @brief Result textures of the layer */
    MaterialChannels result;
    /*! @brief Title of the layer ("myLayer" for instance)*/
    std::string title;
    /*! @brief This icon represents the layer & rendered with the button*/
    Texture layerIcon;
    /*! @brief Button of the layer to display this layer*/
    Button layerButton;

    bool rightClicked = false;
    Panel contextMenu = Panel(
                                        {
                                            Section(
                                                Element(),
                                                {   
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Layer Info"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Opacity Settings"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Delete Layer"  , Texture(), 0.f, false), //1
                                                }
                                            )
                                        }, 
                                        glm::vec2(5.f, 3.f), glm::vec3(50.f, 50.f, 0.7), ColorPalette::secondColor, ColorPalette::thirdColor, true, true, true, true, true, 1.f, 1, {}, 20.f, true
                                    );

    bool alphaSettingsMode = false;
    void renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking);

    /*! @brief Generate result textures for the layer */
    virtual void render() = 0;

    /*! 
        @brief Renders GUI elements for the layer
        @return 0 : No msg | 1 : Delete this layer
    */
    int render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale){
        layerButton.pos = pos;
        layerButton.scale = scale;
        layerButton.render(timer, doMosueTracking);

        if(layerButton.hover && *Mouse::RClick()){
            rightClicked = true;
            contextMenu.pos = glm::vec3(*Mouse::cursorPos() / *Settings::videoScale() * 100.f, pos.z + 0.04f);
        }

        if(!doMosueTracking)
            rightClicked = false;

        if(rightClicked){
            contextMenu.clearDepthBuffer = false;
            contextMenu.render(timer, true);
            if(contextMenu.sections[0].elements[1].button.clicked){
                alphaSettingsMode = true;
                rightClicked = false;
            }
            if(contextMenu.sections[0].elements[2].button.clicked){
                return 1;
                rightClicked = false;
            }

            if(!contextMenu.hover)
                rightClicked = false;
        }

        if(alphaSettingsMode){
            this->renderAlphaSettingsPanel(timer, doMosueTracking);
        }
        
        return 0;
    }

    /*! @brief Generates the this->layerButton from scratch using this->title, this->layerIcon*/
    void updateLayerButton(){
        this->layerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), this->title, this->layerIcon, 0.f, false);
    }
};

/*!
    @brief
*/
class TextureLayer : public Layer {
public:
    TextureLayer(){
        this->title = "Texture Layer";
        this->layerIcon = appTextures.textureIcon;
        this->updateLayerButton();
    }
    
    void render() override {
    
    }
};

/*!
    @brief
*/
class PaintingLayer : public Layer {
public:
    PaintingLayer(){
        this->title = "Painting Layer";
        this->layerIcon = appTextures.brushIcon;
        this->updateLayerButton();
    }
    
    void render() override {
    
    }
};

/*!
    @brief
*/
class MaterialLayer : public Layer {
public:
    MaterialLayer(){
        this->title = "Material Layer";
        this->layerIcon = appTextures.materialIcon;
        this->updateLayerButton();
    }
    
    void render() override {
    
    }
};

/*!
    @brief
*/
class VectorLayer : public Layer {
public:
    VectorLayer(){
        this->title = "Vector Layer";
        this->layerIcon = appTextures.inkPenIcon;
        this->updateLayerButton();
    }

    void render() override {
    }
};

void layers_render(Timer& timer, Panel &layerPanel);
void layers_add_new(Layer* layer);
void layers_update_result();

#endif // LIGID_LAYERS_HPP