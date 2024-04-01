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

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

void panels_init(){
    
    // ---------------------------------------------------
    
    panel_navigation = Panel(
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),""    ,appTextures.ligidPainterIcon, 0.f,false)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Project"    ,Texture(), 0.f,false)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Help"    ,Texture(), 0.f,false)),
                                        }
                                    )
                                },
                                glm::vec2(50,1), //Initial scale value
                                glm::vec3(50,1,0.1f),  //Initial position value
                                ColorPalette::secondColor, //Color of the panel
                                ColorPalette::thirdColor, //Color of the panel
                                false,
                                true,
                                true,
                                false,
                                true,
                                1.f,
                                1,
                                {},
                                20.f,
                                false
                            );

    panel_navigation.solidStyle = true;
    panel_navigation.preRenderingMode = true;
    panel_navigation.solidStyle = true;


    // ---------------------------------------------------

    panel_library  = Panel(
                            {
                                {}
                            },
                            
                            glm::vec2(7,48), //Initial scale value
                            glm::vec3(7,50,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            true,
                            false,
                            false,
                            true,
                            1.f,
                            3,
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Add"        , Texture(), 0.f,false),
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Import"        , Texture(), 0.f,false)
                            },
                            11.f,
                            true
                        );

    panel_library.isLibraryDisplayer = true;
    panel_library.preRenderingMode = true;
    panel_library.solidStyle = true;

    // ---------------------------------------------------

    //Init the window panel
    panel_window = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"3D Painting"         , appTextures.threeDIcon, 0.f,false)), // 0
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"2D Painting"         , appTextures.twoDIcon, 0.f,false)), // 1
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Export"              , appTextures.exportIcon, 0.f,false)), // 2
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Displayer"           , appTextures.environmentIcon, 0.f,false)), // 3
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Settings"            , appTextures.gearIcon, 0.f,false)), // 4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Baking"              , appTextures.bakingIcon, 0.f,false)), // 5
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Materials"           , appTextures.materialIcon, 0.f,false)) //6
                                    }
                                )
                            },
                            
                            glm::vec2(1,48), //Initial scale value
                            glm::vec3(99,50,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            false,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            20.f,
                            false
                        );

    panel_window.preRenderingMode = true;
    panel_window.solidStyle = true;


    // ---------------------------------------------------
 
    panel_library_modes  = Panel(
                                    {
                                        Section(
                                            Element(Button()),
                                            {   
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Textures"        , Texture(), 0.f,true)), // 0 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Materials"       , Texture(), 0.f,true)), // 1 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Brushes"         , Texture(), 0.f,true)), // 2 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"3D Models"       , Texture(), 0.f,true)), // 3 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Fonts"           , Texture(), 0.f,true)), // 4 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Scripts"         , Texture(), 0.f,true)), // 5 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Filters"         , Texture(), 0.f,true)), // 6 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Layers"          , Texture(), 0.f,true)), // 7 
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Texture Packs"   , Texture(), 0.f,true)), // 8 
                                            }
                                        )
                                    },
                                    
                                    glm::vec2(5,48), //Initial scale value
                                    glm::vec3(5,50,0.1f),  //Initial position value
                                    ColorPalette::mainColor, //Color of the panel
                                    ColorPalette::thirdColor, //Color of the panel
                                    true,
                                    true,
                                    false,
                                    false,
                                    true,
                                    1.f,
                                    1,
                                    {},
                                    20.f,
                                    false
                                    );

    panel_library_modes.preRenderingMode = true;
    panel_library_modes.solidStyle = true;

 
    // ---------------------------------------------------
    
    //Init the selected texture displayer
    panel_selected_texture_display  = Panel(
                                                {
                                                    Section(
                                                        Element(Button()),
                                                        {   
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,5.5f), "", Texture(), 0.f, false)),
                                                        }
                                                    )
                                                },
                                                
                                                glm::vec2(50 ,5), //Initial scale value
                                                glm::vec3(50 ,95 ,0.1f),  //Initial position value
                                                ColorPalette::mainColor, //Color of the panel
                                                ColorPalette::thirdColor, //Color of the panel
                                                true,
                                                false,
                                                false,
                                                false,
                                                false,
                                                1.f,
                                                1,
                                                {},
                                                20.f,
                                                false
                                            );

    panel_selected_texture_display.solidStyle = true;
    
    // ---------------------------------------------------

    panel_displaying_modes = Panel(
                                    {
                                        Section(
                                            Element(Button()),
                                            {   
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Nodes"           , appTextures.nodeDisplayingMode,        0.f,true)), // 0
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Painting"        , appTextures.paintingDisplayingMode,       0.f,true)), // 1
                                                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Solid Painting"  , appTextures.solidPaintingDisplayingMode,       0.f,true)), // 2
                                            }
                                        )
                                    },
                                    
                                    glm::vec2(3 ,1), //Initial scale value
                                    glm::vec3(50 ,50 ,0.1f),  //Initial position value
                                    ColorPalette::mainColor, //Color of the panel
                                    ColorPalette::thirdColor, //Color of the panel
                                    false,
                                    true,
                                    true,
                                    true,
                                    true,
                                    1.f,
                                    1,
                                    {},
                                    20.f,
                                    false
                                );
    
    panel_displaying_modes.preRenderingMode = true;
    panel_displaying_modes.solidStyle = true; 
    panel_displaying_modes.elementSelectionMode = true; 

    // ---------------------------------------------------
    
    panel_objects = Panel(
                        {
                        },
                        
                        glm::vec2(6.f ,10.f), //Initial scale value
                        glm::vec3(50 ,50 ,0.1f),  //Initial position value
                        ColorPalette::mainColor, //Color of the panel
                        ColorPalette::thirdColor, //Color of the panel
                        true,
                        true,
                        true,
                        true,
                        true,
                        1.f,
                        1,
                        {},
                        20.f,
                        false
                    );
    
    panel_objects.preRenderingMode = true;
    
    // ---------------------------------------------------

    button_texture_selected_objects = Button(ELEMENT_STYLE_BASIC, glm::vec2(4.f,3.f), "Texture Selected Objects", Texture(), 0.f, false);

    // ---------------------------------------------------

    gizmo_scene = Gizmo("", 0, glm::vec3(0.f), glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y), 0.f, false);
    gizmo_scene.scale *= 2.f;
    
    // ---------------------------------------------------
    
    panel_layers = Panel(           
                            {
                                Section(
                                )
                            },
                            
                            glm::vec2(10 ,5), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            false,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            20.f,
                            false
                        );

    panel_layers.solidStyle = true;
    panel_layers.preRenderingMode = true;

    panel_add_layer  = Panel(                        
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.textureIcon, 0.f, false)), // 0
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.brushIcon, 0.f, false)), // 1
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.materialIcon, 0.f, false)), // 2
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.inkPenIcon, 0.f, false)) // 3
                                    }
                                )
                            },
                            
                            glm::vec2(10, 1.2f), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            false,
                            true,
                            true,
                            true,
                            1.f,
                            4,
                            {},
                            20.f,
                            false
                        );

    button_mesh_selection = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f, 4.f), "", Texture(), 0.f, false);
    button_mesh_selection.meshSelection = true;

    comboBox_layers_resolution = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f), {"256", "512", "1024", "2048", "4096", "8192"}, "Layers Resolution", 1.f);
    comboBox_layers_resolution.selectedIndex = 2;

    // ---------------------------------------------------
    
    panel_painting_modes = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Draw"          , appTextures.brushIcon,        0.f,true)), // 0
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Soften"        , appTextures.softenIcon,       0.f,true)), // 1
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Smear"         , appTextures.smearIcon,        0.f,true)), // 2
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal"        , appTextures.normalMapIcon,    0.f,true)), // 3
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Filter"        , appTextures.filterIcon,       0.f,true)), // 4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Vector"        , appTextures.inkPenIcon,       0.f,true)), // 5
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Bucket"        , appTextures.bucketIcon,       0.f,true)) // 6
                                    }
                                )
                            },
                            
                            glm::vec2(5, 7.f), //Initial scale value
                            glm::vec3(50 ,50 ,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            true,
                            false,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            20.f,
                            false
                        );

    panel_painting_modes.solidStyle = true;
    panel_painting_modes.preRenderingMode = true;
    panel_painting_modes.elementSelectionMode = true;

    // ---------------------------------------------------

    checkComboList_painting_color = CheckComboList(
                                                    "See the color settings", 
                                                    appTextures.colorIcon, 
                                                    glm::vec3(0.f, 0.f, 0.7f), 
                                                    0.f, 
                                                    Section(
                                                        Element(),
                                                        {   
                                                            Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,4), ""  , Texture(), 7.f, true)), // 0
                                                            
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Albedo Channel"  , 1.f)), //1
                                                            Element(PainterColorSelection(glm::vec2(2.f, 2.5f), 1.f)), // 2
                    
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Roughness Channel"  , 2.5f)),//3
                                                            Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Roughness"  , Texture(), 0.2f, 0.f, 1.f, 0.f)), //4
                                                            
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Metallic Channel"  , 2.5f)),//5
                                                            Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Metallic"  , Texture(), 0.2f, 0.f, 1.f, 1.f)), //6

                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Normal Map Channel"  , 2.5f)),//7
                                                            Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Normal Map Strength"  , Texture(), 0.2f, 0.f, 1.f, 1.f)), //8

                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Height Map Channel"  , 2.5f)),//9
                                                            Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Height Map"  , Texture(), 0.2f, 0.f, 1.f, 0.5f)), //10

                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable AO Channel"  , 2.5f)),//11
                                                            Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Ambient Occlusion"  , Texture(), 0.2f, 0.f, 1.f, 0.5f)), //12

                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Use Custom Material"  , 2.5f)),//13
                                                            Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Select A Material"  , Texture(), 0.2f, false)), // 14
                                                        }
                                                    )
                                                );

    checkComboList_painting_color.panel.sections[0].elements[0].button.textureSizeScale = 1.f;
    checkComboList_painting_color.panel.sections[0].elements[0].button.color2 = checkComboList_painting_color.panel.sections[0].elements[0].button.color;
    checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1 = true;
    checkComboList_painting_color.panel.sections[0].elements[14].button.materialSelection = true;

    // ---------------------------------------------------
    
        checkComboList_painting_face_selection = CheckComboList(
                                                                    "Toggle masking mesh for painting / See the mesh settings", 
                                                                    appTextures.TDModelIcon, 
                                                                    glm::vec3(0.f, 0.f, 0.7f), 
                                                                    0.f, 
                                                                    Section(
                                                                        Element(),
                                                                        {  
                                                                            CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Mask to selected mesh (F3)", 1.f), //0
                                                                            CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Mesh selection mode (F4)", 1.f), //1
                                                                            ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f,2.f), {"Circle", "Box"}, "Selecting Mode", 1.f), //2
                                                                            RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(2,1.f), "Circle radius"  , Texture(), 2.f, 1.f, 100.f, 10.f), //3
                                                                            ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f,2.f), {"Custom"}, "Auto Select", 2.f), //4
                                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,4), "Mask via texture"  , Texture(), 4.f, false)),//5
                                                                            Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Cancel texture mask"  , Texture(), 0.f, false)),//6
                                                                            CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Hide unselected mesh", 2.f), //7
                                                                        }
                                                                    )
                                                                );

    checkComboList_painting_face_selection.panel.sections[0].elements[5].button.textureSelection3D = true;
    
    // ---------------------------------------------------

    checkComboList_painting_mirror = CheckComboList(
                                            "See the mirror settings", 
                                            appTextures.mirrorIcon, 
                                            glm::vec3(0.f, 0.f, 0.7f), 
                                            0.f, 
                                            Section(
                                                Element(),
                                                {   
                                                    CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "X Axis", 7.f), // 0
                                                    Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "X Axis Mirror Offset", Texture(), 0.f, -1.f, 1.f, 0.f)), // 1
                                                    CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Y Axis", 1.f), // 2
                                                    Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Y Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)),// 3
                                                    CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Z Axis", 1.f),// 4
                                                    Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Z Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)), // 5
                                                }
                                            )
                                        );
    
    // ---------------------------------------------------
    
    checkComboList_painting_over = CheckComboList(
                                                "Toggle painting over / See the painting over settings", 
                                                appTextures.paintingOverIcon, 
                                                glm::vec3(0.f, 0.f, 0.7f), 
                                                0.f, 
                                                Section(
                                                        Element(),
                                                        {   
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Use Painting Over (F1)", 7.f)), //0
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Editor Mode (F2)", 1.f)), //1
                                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4), "Add Texture From Library", Texture(), 1.f,false)), //2
                                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4), "Add Texture From Computer", Texture(), 1.f,false)), //3
                                                            Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Grayscale Masking", 1.f)),// 4
                                                        }
                                                    )
                                                );
    
    // ---------------------------------------------------
    
    panel_twoD_painting = Panel(
                        {
                            Section(
                                Element(Button()),
                                {   
                                    Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5.5f), "", Texture(), 0.f,false)),
                                }
                            )
                        },
                        
                        glm::vec2(50 ,5), //Initial scale value
                        glm::vec3(50 ,95 ,0.1f),  //Initial position value
                        ColorPalette::mainColor, //Color of the panel
                        ColorPalette::thirdColor, //Color of the panel
                        true,
                        true,
                        false,
                        true,
                        false,
                        1.f,
                        1,
                        {},
                        20.f,
                        false
                    );

    panel_twoD_painting.solidStyle = true;

    twoD_painting_box.customMeshInit(glm::vec3(0.),glm::vec2(0.)); 

    // ---------------------------------------------------

    button_painting_brush = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f), ""  , appTextures.brushIcon, 0.f, false);
    button_painting_brush.infoText = "Modify or display the current brush used for painting";
    button_painting_brush.brushModification = true;
 
    // ---------------------------------------------------
    
    button_painting_filter_mode_filter = Button(ELEMENT_STYLE_SOLID, glm::vec2(4.f,5.f), "Select Filter", Texture(), 0.f, false);
    button_painting_filter_mode_filter.filterSelection = true;
    
    // ---------------------------------------------------
    
    panel_smear_painting_properties = Panel(
                                                {
                                                    Section(
                                                        Element(Button()),
                                                        {   
                                                            RangeBar(ELEMENT_STYLE_BASIC, glm::vec2(1.f), "Transform Strength", Texture(), 0.f, 0.f, 2.f, 1.f),
                                                            RangeBar(ELEMENT_STYLE_BASIC, glm::vec2(1.f), "Bluring Strength", Texture(), 1.f, 0.f, 4.f, 2.f)
                                                        }
                                                    )
                                                },
                                                
                                                glm::vec2(6.5f, 2.6f), //Initial scale value
                                                glm::vec3(50 ,50 ,0.1f),  //Initial position value
                                                glm::vec4(ColorPalette::secondColor.r, ColorPalette::secondColor.g, ColorPalette::secondColor.b, ColorPalette::secondColor.a/2.f), //Color of the panel
                                                ColorPalette::thirdColor, //Color of the panel
                                                true,
                                                true,
                                                false,
                                                true,
                                                true,
                                                1.f,
                                                1,
                                                {},
                                                20.f,
                                                false
                                            );

    // ---------------------------------------------------
    
    checkBox_wrap_mode = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(4.f,2), "Wrap Mode"  , 2.5f);
    
    // ---------------------------------------------------
}