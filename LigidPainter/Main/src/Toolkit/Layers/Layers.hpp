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

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"

#include "3D/Material/Material.hpp"

#include "UTIL/Mouse/Mouse.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

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

    Panel infoPanel = Panel(
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

    Panel alphaSettingsPanel = Panel(
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
    
    bool elementSelectionMode = false;

    bool alphaSettingsMode = false;
    void renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh);
    
    bool infoMode = false;
    void renderInfoPanel(Timer& timer, bool doMouseTracking);

    /*! @brief Generate result textures for the layer */
    virtual void render(const unsigned int resolution, Mesh& mesh) = 0;
    virtual void render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) = 0;
    virtual void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, MaterialChannels** materialChannels) = 0;
    virtual bool is_type_specific_panels_hovered() = 0;

    /*! 
        @brief Renders GUI elements for the layer
        @return 0 : No msg | 1 : Delete this layer | 2 : layer selected
    */
    int render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, const unsigned int resolution, Mesh& mesh);

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
    MaterialChannels channels = MaterialChannels();
    Panel textureSelectPanel;

    TextureLayer(const unsigned int resolution);
    
    void render(const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, MaterialChannels** materialChannels) override{
        *materialChannels = &this->channels;
    }
    bool is_type_specific_panels_hovered() override{
        return false;
    }
};

/*!
    @brief
*/
class PaintingLayer : public Layer {
public:
    PaintingLayer(const unsigned int resolution);
    
    void render(const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, MaterialChannels** materialChannels) override{
        return;
    }
    bool is_type_specific_panels_hovered() override{
        return false;
    }
};

/*!
    @brief
*/
class MaterialLayer : public Layer {
public:
    Material material;
    Panel materialSelectPanel;

    MaterialLayer(const unsigned int resolution);
    
    void render(const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, MaterialChannels** materialChannels) override{
        *material = &this->material; 
    }
    bool is_type_specific_panels_hovered() override{
        return materialSelectPanel.hover;
    }
};

/*!
    @brief
*/
class VectorLayer : public Layer {
public:
    std::vector<VectorStroke3D> strokes;
    VectorScene vector_scene;

    CheckComboList color_checkComboList;
    CheckComboList mirror_checkComboList;
    Panel smear_properties_panel;
    Button filter_button;
    Button brush_properties_button;

    VectorLayer(const unsigned int resolution);

    void render(const unsigned int resolution, Mesh& mesh) override;
    void render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, MaterialChannels** materialChannels) override{
        *strokes = &this->strokes; 
    }
    bool is_type_specific_panels_hovered() override{
        return color_checkComboList.hover || mirror_checkComboList.hover || smear_properties_panel.hover || filter_button.hover || brush_properties_button.hover;
    }
};

class LayerScene{
public:
    LayerScene(){

    }

    std::vector<Layer*> layers;

    void render(Timer& timer, Panel &layerPanel, bool doMouseTracking, const unsigned int resolution, Mesh& mesh);
    void add_new(Layer* layer);
    void update_result(unsigned int resolution, glm::vec3 baseColor, Mesh& mesh);
    bool any_dialog_hovered();
    bool any_vector_editing();
    MaterialChannels get_painting_channels(bool* success);
    void update_all_layers(const unsigned int resolution, glm::vec3 baseColor, Mesh& mesh);
};

#endif // LIGID_LAYERS_HPP