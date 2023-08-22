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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,""    ,Settings::appTextures().ligidPainterIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Project"    ,Texture(), 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Painting"    ,Texture(), 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Help"    ,Texture(), 0.f,false)),
                                    }
                                )
                            },
                            glm::vec2(50,1), //Initial scale value
                            glm::vec3(50,1,0.1f),  //Initial position value
                            colorPalette.secondColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
    windowPanel  = Panel(
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"3D Painting"         , Settings::appTextures().threeDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"2D Painting"         , Settings::appTextures().twoDIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Export"              , Settings::appTextures().exportIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Displayer"           , Settings::appTextures().environmentIcon, 0.f,false)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5),colorPalette,"Settings"           , Settings::appTextures().gearIcon, 0.f,false))
                                    }
                                )
                            },
                            
                            glm::vec2(1,48), //Initial scale value
                            glm::vec3(99,50,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Color"  , Settings::appTextures().colorIcon, 0.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Color1"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Color2"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Color3"  , Texture(), 1.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Dropper"  , Texture(), 1.f, false))
                                    }
                                ),

                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Brush"  , Settings::appTextures().brushIcon, 3.f,true)),
                                    {   
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Radius    "  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Opacity   "  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Hardness  "  , Settings::appTextures().TDModelIcon, 1.f,-100.f,100.f,0.f)),
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Spacing   "  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,0.f)),
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,1),colorPalette, "More"  , Settings::appTextures().modifierIcon, 1.f,true)),
                                    {   
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Size Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //0
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Fade"  , Texture(), 1.f,0.f,100.f,0.f)),//1
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Sin Wave Pattern"  , 1.f)),//2
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Scatter"  , Texture(), 1.f,0.f,100.f,0.f)),//3
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Texture"  , Texture(), 1.f, false)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1),colorPalette, "Remove"  , Texture(), 0.f,false)),//5
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Individual Texture"  , 1.f)),//6
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), //7
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Rotation Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //8
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Alpha Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //9
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,1),colorPalette, "Create New Brush"  , Texture(), 2.f,false)),//10
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,1),colorPalette, "Export"  , Texture(), 0.5f,false)),//11
                                    }
                                ),
                                //Section(
                                //    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Soften"  , Settings::appTextures().softenIcon, 3.f,true)),
                                //    {   
                                //        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2),colorPalette, "Quality (samples)"  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                //    }
                                //),
                                //Section(
                                //    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Smear"  , Settings::appTextures().smearIcon, 3.f,true)),
                                //    {   
                                //        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2),colorPalette, "Quality (samples)"  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                //    }
                                //),
                                //Section(
                                //    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Area"  , Settings::appTextures().areaPaintingIcon, 3.f,true)),
                                //    {   
                                //        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2),colorPalette, "Box"  , Settings::appTextures().TDModelIcon, 1.f,false)),
                                //        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Gradient"  , 1.f)),
                                //        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Depth Test"  , 1.f)),
                                //        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2),colorPalette, "Lasso"  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                //        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2),colorPalette, "Polygonal Lasso"  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                //        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2),colorPalette, "Circle"  , Settings::appTextures().TDModelIcon, 1.f,0.f,100.f,50.f)),
                                //    }
                                //),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Meshes"  , Settings::appTextures().modifierIcon, 3.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "DefaultMeshButton"  , Texture(), 1.f,true)),//4
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Painting Channels"  , Settings::appTextures().modifierIcon, 3.f,true)),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Albedo Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Roughness Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Metallic Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Normal Map Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Height Map Texture"  , Texture(), 1.f,true)),//4
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Ambient Occlusion Texture"  , Texture(), 1.f,true)),//4
                                    }
                                ),
                                Section(
                                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Painting Over"  , Settings::appTextures().modifierIcon, 3.f,true)),
                                    {   
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Use Painting Over", 1.f)), //0
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),colorPalette, "Painting Over Texture"  , Texture(), 1.f,false)), //1
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Grayscale Masking", 1.f)),// 2
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Wrap", 1.f)), //3
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), //4 
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "X : Scale"  , Texture(), 1.f,0.f,2.f,1.f)), //5
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Y : Scale"  , Texture(), 1.f,0.f,2.f,1.f)), //6
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "X : Move"  , Texture(), 1.f, -1.f,1.f,0.f)), //7
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),colorPalette, "Y : Move"  , Texture(), 1.f, -1.f,1.f,0.f)), //8
                                    }
                                )
                            },
                            
                            glm::vec2(8,48), //Initial scale value
                            glm::vec3(50,50,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Textures"        , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Materials"       , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Brushes"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"3D Models"       , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Fonts"           , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Scripts"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Filters"         , Texture(), 0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Layers"          , Texture(), 0.f,true)),
                                    }
                                )
                            },
                            
                            glm::vec2(5,48), //Initial scale value
                            glm::vec3(5,50,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                {}
                            },
                            
                            glm::vec2(7,48), //Initial scale value
                            glm::vec3(7,50,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
                            true,
                            true,
                            false,
                            true,
                            true,
                            1.f,
                            2,
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Add"        , Texture(), 0.f,false),
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,"Import"        , Texture(), 0.f,false)
                            },
                            11.f,
                            true
                        );

    //Init the node editor displayer
    nodeEditorDisplayer  = Panel(
                            
                            colorPalette,
                            {
                                {}
                            },
                            
                            glm::vec2(25 ,5), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5.5f),colorPalette,""        , Settings::appTextures().greetingDialogImage, 0.f,false)),
                                    }
                                )
                            },
                            
                            glm::vec2(50 ,5), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,5.5f),colorPalette,""        , Texture(), 0.f,false)),
                                    }
                                )
                            },
                            
                            glm::vec2(50 ,5), //Initial scale value
                            glm::vec3(50 ,95 ,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Draw"          , Settings::appTextures().brushIcon,        0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Soften"        , Settings::appTextures().softenIcon,       0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Smear"         , Settings::appTextures().smearIcon,        0.f,true)),
                                        //Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Area"          , Settings::appTextures().areaPaintingIcon, 0.f,true)),
                                    }
                                )
                            },
                            
                            glm::vec2(5 ,6), //Initial scale value
                            glm::vec3(50 ,50 ,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
    
    //Init the displaying modes panel
    displayingModesPanel = Panel(
                            
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette, "Nodes"          , Settings::appTextures().nodeDisplayingMode,        0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette, "Painting"        , Settings::appTextures().paintingDisplayingMode,       0.f,true)),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette, "Solid Painting"        , Settings::appTextures().solidPaintingDisplayingMode,       0.f,true)),
                                    }
                                )
                            },
                            
                            glm::vec2(3 ,1), //Initial scale value
                            glm::vec3(50 ,50 ,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
                            colorPalette.thirdColor, //Color of the panel
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
    greetingDialog              = GreetingDialog(colorPalette);
    newTextureDialog            = NewTextureDialog(colorPalette);
    logDialog                   = LogDialog(colorPalette,materialEditorDialog.appMaterialModifiers);
    displayerDialog             = DisplayerDialog(colorPalette);
    textureEditorDialog         = TextureEditorDialog(colorPalette);
    materialEditorDialog        = MaterialEditorDialog(colorPalette);
    textureSelectionDialog      = TextureSelectionDialog(colorPalette);
    loadProjectDialog           = LoadProjectDialog(colorPalette,materialEditorDialog.appMaterialModifiers);
    newProjectDialog            = NewProjectDialog(colorPalette,materialEditorDialog.appMaterialModifiers);
    settingsDialog              = SettingsDialog(colorPalette);
    exportDialog                = ExportDialog(colorPalette);

    char whitePixel[] = { 127, 127, 127, 127 };
    paintingPanel.sections[2].elements[4].button.textureSelection = true;
    paintingPanel.sections[2].elements[4].button.texture = Texture(whitePixel, 1, 1, GL_NEAREST);
    paintingPanel.sections[2].elements[4].button.texture.proceduralID = 24; //Solid white
    paintingPanel.sections[2].elements[4].button.texture.proceduralnverted = 0;
    paintingPanel.sections[2].elements[4].button.texture.proceduralScale = 1.f;
    paintingPanel.sections[2].elements[4].button.texture.title = "AutoGeneratedMask";

    //Change the properties of the color elements in the painting panel (color section)
    for (size_t i = 0; i < 3; i++)
    {
        paintingPanel.sections[0] //Color section
                                 .elements[i] //Color button
                                             .button.outlineThickness = 5.f;
        
        paintingPanel.sections[0] //Color section
                                 .elements[i] //Color button
                                            .button.animationStyle = 2;
    }
    
    //Init renaming textbox
    libraryPanelDisplayer.isLibraryDisplayer = true;
    renamingTextBox = TextBox(0,glm::vec2(4,2),colorPalette,"",4.f,false);
    renamingTextBox.thickness = 0.f;
    renamingTextBox.animationStyle = 1;
    renamingTextBox.radius = 10000.f;

    paintingPanel.sections[5].elements[1].button.textureSelection = true;
}   