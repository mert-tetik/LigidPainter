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
#include "MouseSystem/Mouse.hpp"

struct MaterialChannels{
    Texture albedo;
    Texture roughness;
    Texture metallic;
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;
};

// Forward declerations
class MaterialSelectionDialog; 

/*! @brief Handles the alpha value of a channel*/
struct ChannelAlpha{
    /*! @brief Alpha map (basically opacity map)*/
    Texture alphaMap;
    Texture alphaMapProceduralTxtr;
    /*! @brief Alpha value (basically opacity)*/
    float alphaValue = 1.f;

    void genTxtrs(){
        char whitePx[4] = {127,127,127,127}; 
        this->alphaMapProceduralTxtr = Texture(whitePx, 1, 1);
        this->alphaMap = Texture();
        this->alphaMap.proceduralProps.proceduralID = 24;
    }
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
    /*! @brief This string represents the type of the layer ("texture", "painting", "material", "vector")*/
    std::string layerType;
    /*! @brief Button of the layer to display this layer*/
    Button layerButton;
    
    bool renamingMode = false;
    TextBox renamingTextBox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "Select A Path", 6.f, false);

    Button eyeBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.7f, 1.f), "", Texture(), 0.f, false);
    bool hiden = false;

    bool mainSelected = false;
    bool subSelected = false;

    bool rightClicked = false;
    Panel contextMenu = Panel(
                                        {
                                            Section(
                                                Element(),
                                                {   
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Layer Info"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Opacity Settings"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Rename"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Delete"  , Texture(), 0.f, false), //1
                                                    Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Set Elements"  , Texture(), 0.f, false), //1
                                                }
                                            )
                                        }, 
                                        glm::vec2(5.f, 5.f), glm::vec3(50.f, 50.f, 0.7), ColorPalette::secondColor, ColorPalette::thirdColor, true, true, true, true, true, 1.f, 1, {}, 20.f, true
                                    );
    
    bool elementSelectionMode = false;

    bool alphaSettingsMode = false;
    void renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh);
    
    bool infoMode = false;
    void renderInfoPanel(Timer& timer, bool doMouseTracking);

    /*! @brief Generate result textures for the layer */
    virtual void render(Painter& painter, const unsigned int resolution, Mesh& mesh) = 0;
    virtual void render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh) = 0;

    /*! 
        @brief Renders GUI elements for the layer
        @return 0 : No msg | 1 : Delete this layer | 2 : layer selected
    */
    int render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh);

    /*! @brief Generates the this->layerButton from scratch using this->title, this->layerIcon*/
    void updateLayerButton();

    void genResultChannels(const unsigned int resolution);

    void updateProceduralMaskTextures(const unsigned int resolution, Mesh& mesh);

    void updateResultTextureResolutions(const unsigned int resolution, Mesh& mesh);
};

/*!
    @brief
*/
class TextureLayer : public Layer {
public:
    MaterialChannels channels;

    TextureLayer(const unsigned int resolution);
    
    void render(Painter& painter, const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh) override;
};

/*!
    @brief
*/
class PaintingLayer : public Layer {
public:
    PaintingLayer(const unsigned int resolution);
    
    void render(Painter& painter, const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh) override;
};

/*!
    @brief
*/
class MaterialLayer : public Layer {
public:
    Material material;

    MaterialLayer(const unsigned int resolution);
    
    void render(Painter& painter, const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh) override;
};

/*!
    @brief
*/
class VectorLayer : public Layer {
public:
    std::vector<VectorStroke3D> strokes;

    VectorLayer(const unsigned int resolution);

    void render(Painter& painter, const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution, Mesh& mesh) override;
};

class LayerScene{
public:
    LayerScene(){

    }

    std::vector<Layer*> layers;

    void render(Timer& timer, Panel &layerPanel, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, bool doMouseTracking, const unsigned int resolution, Mesh& mesh);
    void add_new(Layer* layer);
    void update_result(unsigned int resolution, glm::vec3 baseColor, Mesh& mesh);
    bool any_dialog_active();
    bool any_vector_editing();
    MaterialChannels get_painting_channels(bool* success);
    void update_all_layers(const unsigned int resolution, glm::vec3 baseColor, Painter& painter, Mesh& mesh);
};

#endif // LIGID_LAYERS_HPP