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
class LayerScene;
class Layer;

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

    void update_buffers(const unsigned int resolution, Mesh& mesh);
};

struct LayerGUI{

    /*! @brief Button of the layer used for displaying this layer*/
    Button layerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "", Texture(), 0.f, false);
    /*! @brief Used for modifying hidden mode of the layer in GUI and rendered in render_info_and_modification_elements function */
    Button eyeBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.7f, 1.f), "", Texture(), 0.f, false);

    /*! @brief This value gradually becomes 1 if mainSelected or subSelected flags set to true*/
    float clickedMixVal = 0.f;

    /*! @brief Renders layer background for the layer GUI. Renders selected material for material layers, selected stroke for the vector layers, selected textures for texture layers and last FBO result for the painting layers*/
    void render_layer_bg(Layer* layer);

    /*! @brief Renders info and modification elements (eye icon, general opacity value, general opacity map) on top of the layer button*/
    void render_info_and_modification_elements(Timer& timer, bool doMouseTracking, Texture icon, bool* hidden, Texture alpha_map_texture, float alpha_value, glm::vec3 pos, glm::vec2 scale, float opacity);

    /*! @brief */
    bool show_layer_context_menu(Timer& timer, Layer* layer, LayerScene* src_layer_scene, unsigned int resolution, Mesh& mesh);
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
    /*! @brief This flag determines if the layer is hidden or not. Hidden layers will be ignored for layer result calculation*/
    bool hiden = false;
    /*! @brief Unique selected layer */
    bool mainSelected = false;
    /*! @brief One of the multiple selected layers */
    bool subSelected = false;
    
    LayerGUI layerGUI;

    bool type_specific_modification_enabled = false;

    /*! @brief Generate result textures for the layer */
    virtual void type_specific_generate_result(const unsigned int resolution, Mesh& mesh) = 0;
    virtual void type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) = 0;
    virtual void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, Brush** vector_stroke_brush, MaterialChannels** materialChannels, Texture** painting_capture_txtr) = 0;
    virtual bool is_type_specific_panels_hovered() = 0;

    /*! 
        @brief Renders GUI elements for the layer. Returns true if the layer is deleted
    */
    bool render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, const unsigned int resolution, Mesh& mesh, LayerScene* src_layer_scene, int src_layer_index);

    void update_result_buffers(const unsigned int resolution, Mesh& mesh);
    void update_layer_buffers_resolution_values(const unsigned int resolution, Mesh& mesh);

    /*! @brief Generates alpha and result textures & used in the constructors*/
    void generate_layer_buffers(const unsigned int resolution);
    
};

/*!
    @brief
*/
class TextureLayer : public Layer {
public:
    MaterialChannels channels = MaterialChannels();
    Panel textureSelectPanel;
    TextureLayer(const unsigned int resolution);
    
    void type_specific_generate_result(const unsigned int resolution, Mesh& mesh) override;
    void type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, Brush** vector_stroke_brush, MaterialChannels** materialChannels, Texture** painting_capture_txtr) override{
        if(materialChannels != nullptr)
            *materialChannels = &this->channels;
    }
    bool is_type_specific_panels_hovered() override{
        return textureSelectPanel.hover;
    }
};

/*!
    @brief
*/
class PaintingLayer : public Layer {
public:
    Texture painting_capture_txtr;

    PaintingLayer(const unsigned int resolution);
    
    void type_specific_generate_result(const unsigned int resolution, Mesh& mesh) override;
    void type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, Brush** vector_stroke_brush, MaterialChannels** materialChannels, Texture** painting_capture_txtr) override{
        if(painting_capture_txtr != nullptr)
            *painting_capture_txtr = &this->painting_capture_txtr;
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
    
    void type_specific_generate_result(const unsigned int resolution, Mesh& mesh) override;
    void type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, Brush** vector_stroke_brush, MaterialChannels** materialChannels, Texture** painting_capture_txtr) override{
        if(material != nullptr)
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

    void type_specific_generate_result(const unsigned int resolution, Mesh& mesh) override;
    void type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh) override;
    void get_type_specific_variable(Material** material, std::vector<VectorStroke3D>** strokes, Brush** vector_stroke_brush, MaterialChannels** materialChannels, Texture** painting_capture_txtr) override{
        if(strokes != nullptr)
            *strokes = &this->strokes; 
        if(vector_stroke_brush != nullptr)
            *vector_stroke_brush = &this->brush_properties_button.brush; 
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
    
    /*! @brief Returns only the main-selected layer. Returns nullptr if no layer was selected. layer_index is set to index value of the selected layer in the array and is not modified if no layer was main-selected*/
    Layer* get_selected_layer(int* layer_index);
    
    /*! @brief Returns all the selected layers in the layer scene. Whether sub-selected or main-selected*/
    std::vector<Layer*> get_selected_layers();

    /*! @brief  Set main-selected & sub-selected flags to false for each layer*/
    void unselect_all_layers();

    /*! @brief Deletes all the selected layers*/
    void delete_all_selected_layers();
    
    //! @brief Selects the corresponding layer at the index of layer_index. If unselect_selected_ones flag set to true all remaining layers will be unselected. If select_between flag set to true layers between the already selected and 
    void select_layer(int layer_index, bool unselect_selected_ones, bool select_between);

    // Alters the hide state of the selected layers
    void hide_unhide_selected_layers();
    
    void update_all_layers(const unsigned int resolution, glm::vec3 baseColor, Mesh& mesh);
    
};

#endif // LIGID_LAYERS_HPP