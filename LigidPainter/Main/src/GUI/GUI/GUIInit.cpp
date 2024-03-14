/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init the GUI class

*/


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>


void UI::init
            (  
                Websites websites,
                Painter& painter
            ){
    
    //Take params to the class member variables
    this->websites = websites;

    //Init the project bar (navigation panel)
    navigationPanel = Panel(
                            
                            
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


    //Init the window panel
    windowPanel = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"3D Painting"         , appTextures.threeDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"2D Painting"         , appTextures.twoDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Export"              , appTextures.exportIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Displayer"           , appTextures.environmentIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Settings"            , appTextures.gearIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Baking"              , appTextures.bakingIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Materials"           , appTextures.materialIcon, 0.f,false))
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
    
    //Init the library panel (left)
    libraryPanelLeft  = Panel(
                            
                            
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Textures"        , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Materials"       , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Brushes"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"3D Models"       , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Fonts"           , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Scripts"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Filters"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Layers"          , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Texture Packs"   , Texture(), 0.f,true)),
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

    //Init the library panel displayer 
    libraryPanelDisplayer  = Panel(
                            
                            
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

    //Init the selected texture displayer
    selectedTextureDisplayer  = Panel(
                            
                            
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
 
    meshSelectionButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f, 4.f), "", Texture(), 0.f, false);
    layersResolutionComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f), {"256", "512", "1024", "2048", "4096", "8192"}, "Layers Resolution", 1.f);
    
    layersPanel  = Panel(
                            
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
    
    addLayerPanel  = Panel(                        
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.textureIcon, 0.f, false)),
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.brushIcon, 0.f, false)),
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.materialIcon, 0.f, false)),
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 1.2f), "", appTextures.inkPenIcon, 0.f, false))
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

    //Init the 2D painting panel
    twoDPaintingPanel  = Panel(
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


    //Init the Painting modes panel
    paintingModesPanel = Panel(
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Draw"          , appTextures.brushIcon,        0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Soften"        , appTextures.softenIcon,       0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Smear"         , appTextures.smearIcon,        0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal"        , appTextures.normalMapIcon,    0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Filter"        , appTextures.filterIcon,       0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Vector"        , appTextures.inkPenIcon,       0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Bucket"        , appTextures.bucketIcon,       0.f,true))
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

    //Init the Painting modes panel
    vectorPaintingModePropertyPanel = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), {"Drawing", "Softening", "Smearing", "Normal", "Filtering"}, "Stroke Painting Mode", 1.f),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Add Point Between The Selected Points", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Clear All", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Remove The Selected PointS (CTRL + X)", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Stroke", Texture(), 0.5f, false)
                                    }
                                )
                            },
                            
                            glm::vec2(7.f, 9.f), //Initial scale value
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

    vectorPaintingMode2DModeWrapCheckBox = CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2), "Paint in wrap mode", 1.f); //1

    //Init the smear painting mode property panel
    smearPaintingModePropertyPanel = Panel(
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
    
    //Init the displaying modes panel
    displayingModesPanel = Panel(
                            
                            
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Nodes"           , appTextures.nodeDisplayingMode,        0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Painting"        , appTextures.paintingDisplayingMode,       0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Solid Painting"  , appTextures.solidPaintingDisplayingMode,       0.f,true)),
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

    objectsPanel = Panel(
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

    paintingChannelsTextureSelectionPanel = Panel(
                            {
                                Section()
                            },
                            
                            glm::vec2(10.f ,15.f), //Initial scale value
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

    //Init renaming textbox
    libraryPanelDisplayer.isLibraryDisplayer = true;
    renamingTextBox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "", 4.f, false);
    renamingTextBox.thickness = 0.f;
    renamingTextBox.animationStyle = 1;
    renamingTextBox.radius = 10000.f;

    this->zoomingDisplayingButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f,8.f), "", Texture(), 0.f, true);
    
    this->textureSelectedObjectsButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(4.f,3.f), "Texture Selected Objects", Texture(), 0.f, false);
    
    this->filterPaintingModeFilterBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(4.f,5.f), "Select Filter", Texture(), 0.f, false);
    this->filterPaintingModeFilterBtn.filterSelection = true;

    navigationPanel.solidStyle = true;

    this->twoDPaintingBox.customMeshInit(glm::vec3(0.),glm::vec2(0.)); 
    
    this->meshSelectionButton.meshSelection = true;
    this->layersResolutionComboBox.selectedIndex = 2;

    this->navigationPanel.preRenderingMode = true;
    this->windowPanel.preRenderingMode = true;
    this->libraryPanelLeft.preRenderingMode = true;
    this->libraryPanelDisplayer.preRenderingMode = true;
    this->twoDPaintingPanel.preRenderingMode = true;
    this->paintingModesPanel.preRenderingMode = true;
    this->vectorPaintingModePropertyPanel.preRenderingMode = true;
    this->smearPaintingModePropertyPanel.preRenderingMode = true;
    this->displayingModesPanel.preRenderingMode = true;
    this->layersPanel.preRenderingMode = true;
    
    this->navigationPanel.solidStyle = true;
    this->windowPanel.solidStyle = true;
    this->libraryPanelLeft.solidStyle = true;
    this->libraryPanelDisplayer.solidStyle = true;
    this->selectedTextureDisplayer.solidStyle = true;
    this->twoDPaintingPanel.solidStyle = true;
    this->paintingModesPanel.solidStyle = true;
    this->vectorPaintingModePropertyPanel.solidStyle = true;
    this->smearPaintingModePropertyPanel.solidStyle = true;
    this->displayingModesPanel.solidStyle = true;
    this->layersPanel.solidStyle = true;

    this->meshSection =         Section(
                                    Element(),
                                    {  
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Mask to selected mesh (F3)", 1.f), //1
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Mesh selection mode (F4)", 1.f), //2
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f,2.f), {"Circle", "Box"}, "Selecting Mode", 1.f), //3
                                        RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(2,1.f), "Circle radius"  , Texture(), 2.f, 1.f, 100.f, 10.f), //4
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f,2.f), {"Custom"}, "Auto Select", 2.f), //5
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4), "Mask via texture"  , Texture(), 4.f, false)),//6
                                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2), "Cancel texture mask"  , Texture(), 0.f, false)),//7
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Hide unselected mesh", 2.f), //8
                                    }
                                );
    
    this->mirrorSection =       Section(
                                    Element(),
                                    {   
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "X Axis", 7.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "X Axis Mirror Offset", Texture(), 0.f, -1.f, 1.f, 0.f)),
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Y Axis", 1.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Y Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)),
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Z Axis", 1.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Z Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)),
                                    }
                                );
            

    this->paintingOverSection = Section(
                                    Element(),
                                    {   
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Use Painting Over (F1)", 7.f)), //0
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Editor Mode (F2)", 1.f)), //1
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Add Texture From Library", Texture(), 1.f,false)), //2
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Add Texture From Computer", Texture(), 1.f,false)), //3
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Grayscale Masking", 1.f)),// 4
                                    }
                                );

    this->colorSection =        Section(
                                    Element(),
                                    {   
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,4), ""  , Texture(), 7.f, true)), // 0
                                        
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Albedo Channel"  , 1.f)), //1
                                        Element(PainterColorSelection(glm::vec2(2,2), 1.f)), // 2
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color2"  , Texture(), 1.f, true)), // 3
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color3"  , Texture(), 1.f, true)), // 4
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Dropper"  , Texture(), 1.f, false)), // 5
 
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Roughness Channel"  , 2.5f)),//6
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Roughness"  , Texture(), 0.2f, 0.f, 1.f, 0.f)), //7
                                        
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Metallic Channel"  , 2.5f)),//8
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Metallic"  , Texture(), 0.2f, 0.f, 1.f, 1.f)), //9

                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Normal Map Channel"  , 2.5f)),//10
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Normal Map Strength"  , Texture(), 0.2f, 0.f, 1.f, 1.f)), //11

                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable Height Map Channel"  , 2.5f)),//12
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Height Map"  , Texture(), 0.2f, 0.f, 1.f, 0.5f)), //13

                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Enable AO Channel"  , 2.5f)),//14
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Ambient Occlusion"  , Texture(), 0.2f, 0.f, 1.f, 0.5f)), //15

                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Use Custom Material"  , 2.5f)),//16
                                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Select A Material"  , Texture(), 0.2f, false)), // 17
                                    }
                                );

    this->cantBeDisplayedSection =        Section(
                                    Element(),
                                    {   
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Can't be displayed in the current displaying mode"  , Texture(), 7.f, false)),
                                    }
                                );

    this->cantBeDisplayedSection.elements[0].button.color = glm::vec4(0.);
    this->cantBeDisplayedSection.elements[0].button.color2 = glm::vec4(0.);
    this->cantBeDisplayedSection.elements[0].button.outline = false;;
    this->cantBeDisplayedSection.elements[0].button.outlineExtra = false;;
    
    meshSection.elements[5].button.textureSelection3D = true;

    colorSection.elements[0].button.textureSizeScale = 1.f;
    colorSection.elements[0].button.color2 = colorSection.elements[0].button.color;
    colorSection.elements[1].checkBox.clickState1 = true;
    colorSection.elements[6].checkBox.clickState1 = true;
    colorSection.elements[8].checkBox.clickState1 = true;
    colorSection.elements[10].checkBox.clickState1 = true;
    colorSection.elements[12].checkBox.clickState1 = true;
    colorSection.elements[14].checkBox.clickState1 = true;

    //Change the properties of the color elements in the painting panel (color section)
    for (size_t i = 2; i < 5; i++)
    {
        colorSection //Color section
                                 .elements[i] //Color button
                                             .button.outlineThickness = 5.f;
        
        colorSection //Color section
                                 .elements[i] //Color button
                                            .button.animationStyle = 2;
        
        colorSection //Color section
                                 .elements[i] //Color button
                                            .button.solidColor = true;
    }

    /*
    paintingSectionDisplayMat = Material("Painting section display", 0);
    paintingSectionDisplayMat.materialModifiers.push_back(MaterialModifier(SOLID_MATERIAL_MODIFIER));
*/

    sceneGizmo = Gizmo("", 0, glm::vec3(0.f), glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y), 0.f, false);
    sceneGizmo.scale *= 2.f;

    currentModeDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeDisplayer.color = glm::vec4(0.f);
    currentModeDisplayer.outline = false;
    
    currentModeHintDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeHintDisplayer.color = glm::vec4(0.f);
    currentModeHintDisplayer.outline = false;

    wrapModeCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(4.f,2), "Wrap Mode"  , 2.5f);

    faceSelectionCheckComboList = CheckComboList("Toggle masking mesh for painting / See the mesh settings", appTextures.TDModelIcon, glm::vec3(0.f, 0.f, 0.7f), 0.f, meshSection);
    paintingOverCheckComboList = CheckComboList("Toggle painting over / See the painting over settings", appTextures.paintingOverIcon, glm::vec3(0.f, 0.f, 0.7f), 0.f, paintingOverSection);
    colorCheckComboList = CheckComboList("See the color settings", appTextures.colorIcon, glm::vec3(0.f, 0.f, 0.7f), 0.f, colorSection);
    mirrorCheckComboList = CheckComboList("See the mirror settings", appTextures.mirrorIcon, glm::vec3(0.f, 0.f, 0.7f), 0.f, colorSection);
    paintingBrushButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f), ""  , appTextures.brushIcon, 0.f, false);
    paintingBrushButton.infoText = "Modify or display the current brush used for painting";
    paintingBrushButton.brushModification = true;
}   