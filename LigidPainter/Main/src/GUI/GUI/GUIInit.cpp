/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
                Websites websites
            ){
    
    //Take params to the class member variables
    
    this->websites = websites;

    //Init the project bar (navigation panel)
    navigationPanel = Panel(
                            
                            
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),""    ,Settings::appTextures().ligidPainterIcon, 0.f,false)),
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
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"3D Painting"         , Settings::appTextures().threeDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"2D Painting"         , Settings::appTextures().twoDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Export"              , Settings::appTextures().exportIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Displayer"           , Settings::appTextures().environmentIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Settings"            , Settings::appTextures().gearIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),"Baking"              , Settings::appTextures().bakingIcon, 0.f,false))
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
    
    //Init the painting panel 
    paintingPanel=Panel(
                            {
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color"  , Settings::appTextures().colorIcon, 0.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color1"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color2"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color3"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Dropper"  , Texture(), 1.f, false))
                                    }
                                ),

                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Brush"  , Settings::appTextures().brushIcon, 3.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5), ""  , Texture(), 0.f, false)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Radius"  , Texture(), 2.f, 0.005f, 0.05f, 0.01f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Hardness"  , Texture(), 1.f, -10.f, 10.f, 0.f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Spacing"  , Texture(), 1.f, 0.f, 100.f, 0.f)),

                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Size Jitter"  , Texture(), 4.f, 0.f, 1.f, 0.f)), //0
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Fade"  , Texture(), 1.f, 0.f, 1.f, 0.f)),//1
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Sin Wave Pattern"  , 1.f)),//2
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Scatter"  , Texture(), 1.f, 0.f, 1.f, 0.f)),//3
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Texture"  , Texture(), 1.f, false)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Remove"  , Texture(), 0.f,false)),//5
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Individual Texture"  , 1.f)),//6
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Rotation"  , Texture(), 1.f, 0.f, 360.f, 0.f)), //7
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Rotation Jitter"  , Texture(), 1.f, 0.f, 1.f, 0.f)), //8
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Alpha Jitter"  , Texture(), 1.f, 0.f, 1.f, 0.f)), //9
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,1),"Create New Brush"  , Texture(), 2.f,false)),//10
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,1),"Export"  , Texture(), 0.5f,false)),//11
                                    }
                                ),
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4), "Select A Mesh"  , Texture(), 1.f, false)),//4
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Painting Channels"  , Settings::appTextures().materialChannelsIcon, 3.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Albedo Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Roughness Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Metallic Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Normal Map Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Height Map Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Ambient Occlusion Texture"  , Texture(), 1.f,true)),//4
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Painting Over"  , Settings::appTextures().paintingOverIcon, 3.f,true)),
                                    {   
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Use Painting Over", 1.f)), //0
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Editor Mode", 1.f)), //0
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Add Texture From Library", Texture(), 1.f,false)), //1
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Add Texture From Computer", Texture(), 1.f,false)), //1
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Grayscale Masking", 1.f)),// 2
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Wrap", 1.f)), //3
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Mirror"  , Settings::appTextures().mirrorIcon, 3.f,true)), // 6
                                    {   
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "X Axis", 1.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "X Axis Mirror Offset", Texture(), 0.f, -1.f, 1.f, 0.f)),
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Y Axis", 1.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Y Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)),
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Z Axis", 1.f),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Z Axis Mirror Offset"  , Texture(), 0.f, -1.f, 1.f, 0.f)),

                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Face Selection"  , Settings::appTextures().TDModelIcon, 3.f,true)),
                                    {  
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Mask to selected mesh", 1.f), //0
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Mesh selection mode", 1.f), //1
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f,2.f), {"Circle", "Box"}, "Selecting Mode", 1.f), //2
                                        RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(2,1.f), "Circle radius"  , Texture(), 1.f, 1.f, 100.f, 10.f), //3
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "XRay", 1.f), //4
                                    }
                                )
                            },
                            
                            glm::vec2(8,48), //Initial scale value
                            glm::vec3(50,50,0.1f),  //Initial position value
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
                            true
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
                            true,
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
                            true,
                            true,
                            1.f,
                            2,
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Add"        , Texture(), 0.f,false),
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Import"        , Texture(), 0.f,false)
                            },
                            11.f,
                            true
                        );

    //Init the node editor displayer
    nodeEditorDisplayer  = Panel(
                            
                            
                            {
                                {}
                            },
                            
                            glm::vec2(25 ,5), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            ColorPalette::mainColor, //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            false,
                            true,
                            true,
                            false,
                            1.f,
                            1,
                            {},
                            45.f,
                            false
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

    //Init the 2D painting panel
    twoDPaintingPanel  = Panel(
                            
                            
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5.5f),""        , Texture(), 0.f,false)),
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
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Draw"          , Settings::appTextures().brushIcon,        0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Soften"        , Settings::appTextures().softenIcon,       0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Smear"         , Settings::appTextures().smearIcon,        0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal"    , Settings::appTextures().normalMapIcon,    0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Filter"        , Settings::appTextures().filterIcon,       0.f,true)),
                                            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Vector (experimental)"        , Settings::appTextures().inkPenIcon,       0.f,true))
                                        }
                                    )
                                },
                                
                                glm::vec2(5, 6.f), //Initial scale value
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
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Remove The Selected Point", Texture(), 0.5f, false),
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
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Nodes"           , Settings::appTextures().nodeDisplayingMode,        0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Painting"        , Settings::appTextures().paintingDisplayingMode,       0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),"Solid Painting"  , Settings::appTextures().solidPaintingDisplayingMode,       0.f,true)),
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

    //Init dialogs
    greetingDialog              = GreetingDialog();
    newTextureDialog            = NewTextureDialog();
    logDialog                   = LogDialog(materialEditorDialog.appMaterialModifiers);
    displayerDialog             = DisplayerDialog();
    textureEditorDialog         = TextureEditorDialog();
    texturePackEditorDialog     = TexturePackEditorDialog();
    materialEditorDialog        = MaterialEditorDialog();
    textureSelectionDialog      = TextureSelectionDialog();
    bakingDialog                = BakingDialog();
    filterSelectionDialog       = FilterSelectionDialog();
    loadProjectDialog           = LoadProjectDialog(materialEditorDialog.appMaterialModifiers);
    newProjectDialog            = NewProjectDialog(materialEditorDialog.appMaterialModifiers);
    settingsDialog              = SettingsDialog();
    exportDialog                = ExportDialog();
    materialDisplayerDialog     = MaterialDisplayerDialog();
    filterDisplayerDialog       = FilterDisplayerDialog();

    char whitePixel[] = { 127, 127, 127, 127 };
    paintingPanel.sections[1].elements[9].button.textureSelection2D = true;
    paintingPanel.sections[1].elements[9].button.texture = Texture(whitePixel, 1, 1, GL_NEAREST);
    paintingPanel.sections[1].elements[9].button.texture.proceduralProps.proceduralID = 24; //Solid white
    paintingPanel.sections[1].elements[9].button.texture.proceduralProps.proceduralnverted = 0;
    paintingPanel.sections[1].elements[9].button.texture.proceduralProps.proceduralScale = 1.f;
    paintingPanel.sections[1].elements[9].button.texture.title = "AutoGeneratedMask";
    
    paintingPanel.sections[2].elements[0].button.meshSelection = true;

    //Change the properties of the color elements in the painting panel (color section)
    for (size_t i = 0; i < 3; i++)
    {
        paintingPanel.sections[0] //Color section
                                 .elements[i] //Color button
                                             .button.outlineThickness = 5.f;
        
        paintingPanel.sections[0] //Color section
                                 .elements[i] //Color button
                                            .button.animationStyle = 2;
        
        paintingPanel.sections[0] //Color section
                                 .elements[i] //Color button
                                            .button.solidColor = true;
    }
    
    //Init renaming textbox
    libraryPanelDisplayer.isLibraryDisplayer = true;
    renamingTextBox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "", 4.f, false);
    renamingTextBox.thickness = 0.f;
    renamingTextBox.animationStyle = 1;
    renamingTextBox.radius = 10000.f;

    this->zoomingDisplayingButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f,8.f), "", Texture(), 0.f, true);
    
    this->filterPaintingModeFilterBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(4.f,5.f), "Select Filter", Texture(), 0.f, false);
    this->filterPaintingModeFilterBtn.filterSelection = true;

    navigationPanel.solidStyle = true;

    this->twoDPaintingBox.customMeshInit(glm::vec3(0.),glm::vec2(0.)); 
}   