
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"

#include "UTIL/Settings/Settings.hpp"

#include <string>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

TextureEditorDialog::TextureEditorDialog(int){

    this->displayingTexture = Texture((char*)nullptr, 512, 512, GL_LINEAR);

    //Create the panel
    this->bgPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                    }
                )
            }
        },
        glm::vec2(34.f, 35.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->sectionPanel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Resize", appTextures.resizingIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Blur", appTextures.blurIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Coloring", appTextures.colorGearIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Normal Map", appTextures.normalMapIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Distortion", appTextures.distortionIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Filters", appTextures.filterIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Images", appTextures.solidPaintingDisplayingMode, 1.f,true)),
                    }
                )
            }
        },
        glm::vec2(6.f, 34.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::secondColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->resizeElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Stretch", "Alpha Zero", "Color", "Repeat", "Mirror"}, "Wraping Function", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Wraping Color", Texture(), 1.f, false),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center", "Custom"}, "Origin Point", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Origin Point X", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Origin Point Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        TextBox(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "1024", 0.f, false),
        TextBox(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "1024", 0.f, false)
    };
    this->resizeElements[1].button.colorSelection = true;
    
    this->bluringElement = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Box", "Gaussian", "Directional", "Radial", "Lens"}, "Bluring Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Directional Direction", Texture(), 0.f, 0.f, 360.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength", Texture(), 0.f, 0.f, 1.f, 0.f),
    };
    
    this->coloringElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"HSV", "Single Color", "Brightness"}, "Coloring Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Hue",      Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Saturation",      Texture(), 0.f, 0.f, 1.f, 1.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Value",      Texture(), 0.f, 0.f, 1.f, 1.f),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Multiply", "Hue", "Add", "Subtract", "Divide"}, "Coloring Function", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Color", Texture(), 1.f, false),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Brightness",      Texture(), 0.f, 0.f, 2.f, 1.f),
    };
    this->coloringElements[5].button.colorSelection = true;
    this->coloringElements[5].button.color = glm::vec4(0.f, 0.23f, 1.f, 1.f);

    this->normalMapElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Height Map To Normal Map", "Normal Map To Height Map"}, "Mode", 0.f),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Blue Scale", "Gray Scale"}, "Normal Map Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 2.0f, 1.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Blur",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Rotation",  Texture(), 0.f, 0.f, 360.f, 0.f),
    };

    this->distortionElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Ripple", "Sin Wave", "Swirl", "Rain", "Texture"}, "Distortion Type", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Ripple Roughness",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Ripple Octaves",  Texture(), 0.f, 0, 12, 8),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Sin X Strength",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Sin Y Strength",  Texture(), 0.f, 0.f, 40.f, 20.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Swirl Radius",  Texture(), 0.f, 0.f, 1.f, 0.5f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Swirl Angle",  Texture(), 0.f, 0.f, 10.f, 4.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Rain Scale",  Texture(), 0.f, 0.f, 50.f, 40.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Texture", Texture(), 1.f, false),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 1.f, 0.2f),
    };

    this->imagesElements = {
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Image", Texture(), 1.f, true),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Scale",  Texture(), 0.f, 0.f, 10.f, 1.f),
    };

    this->filterBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f), "Pick A Filter", Texture(), 1.f, false);
    this->filterBtn.filterSelection = true;
    this->textureBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f), "Pick A Texture", Texture(), 1.f, false);
    this->textureBtn.textureSelection2D = true;
    this->displayerBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(13.f), "", this->displayingTexture, 1.f,true);

    char whitePx[] = {127, 127, 127, 127};

    this->maskTextureButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), "Mask Texture", Texture(whitePx, 1, 1, GL_NEAREST), 1.f, false);
    this->maskTextureButton.texture.proceduralProps.proceduralID = 24; //Solid white
    this->maskTextureButton.texture.proceduralProps.proceduralnverted = 0;
    this->maskTextureButton.texture.proceduralProps.proceduralScale = 1.f;

    this->saveButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Save", Texture(), 1.f, false);
    this->saveAsButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Save As", Texture(), 1.f, false);

    this->maskTextureButton.textureSelection2D = true;
    distortionElements[8].button.textureSelection2D = true;
}