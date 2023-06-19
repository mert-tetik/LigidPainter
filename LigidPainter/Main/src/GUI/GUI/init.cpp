/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init the GUI class

*/


#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

#include "tinyfiledialogs.h"


void UI::init
            (  
                Shaders shaders, //All the shaders
                Context context, //GLfW window & stuff
                AppTextures appTextures, //Textures used in GUI
                Websites websites, //Websites related to the LigidPainter
                glm::vec2 videoScale, //Monitor resolution
                Model &sphereModel  //A sphere model
            ){
    
    //Take params to the class member variables
    this->shaders = shaders;
    this->appTextures = appTextures;
    this->sphereModel = sphereModel;
    this->websites = websites;
    
    //Init the project bar (navigation panel)
    navigationPanel = Panel(
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Project"    ,Texture(), 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Painting"    ,Texture(), 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Help"    ,Texture(), 0.f,false)),
                                    }
                                )
                            },
                            glm::vec2(50,1), //Initial scale value
                            glm::vec3(50,1,0.1f),  //Initial position value
                            colorPalette.mainColor, //Color of the panel
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
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"3D Painting"         , appTextures.threeDIcon, 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"2D Painting"         , appTextures.twoDIcon, 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Export"              , appTextures.exportIcon, 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Displayer"           , appTextures.environmentIcon, 0.f,false)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Settings"           , appTextures.gearIcon, 0.f,false))
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
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color"  , appTextures.colorIcon, 0.f,true)),
                                    {   
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color1"  , Texture(), 1.f, true)),
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color2"  , Texture(), 1.f, true)),
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color3"  , Texture(), 1.f, true)),
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Dropper"  , Texture(), 1.f, false))
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Brush"  , appTextures.brushIcon, 3.f,true)),
                                    {   
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Radius    "  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Opacity   "  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Hardness  "  , appTextures.TDModelIcon, 1.f,-100.f,100.f,0.f)),
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Spacing   "  , appTextures.TDModelIcon, 1.f,0.f,100.f,0.f)),
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,1),colorPalette,shaders.buttonShader, "More"  , appTextures.modifierIcon, 1.f,true)),
                                    {   
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Size Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //0
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Fade"  , Texture(), 1.f,0.f,100.f,0.f)),//1
                                        Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Sin Wave Pattern"  , 1.f)),//2
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Scatter"  , Texture(), 1.f,0.f,100.f,0.f)),//3
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,4),colorPalette,shaders.buttonShader, "Texture"  , appTextures.greetingDialogImage, 1.f,true)),//4
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Remove"  , Texture(), 0.f,false)),//5
                                        Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Individual Texture"  , 1.f)),//6
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), //7
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Rotation Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //8
                                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Alpha Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //9
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Create New Brush"  , Texture(), 2.f,false)),//10
                                        Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Export"  , Texture(), 0.5f,false)),//11
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Soften"  , appTextures.softenIcon, 3.f,true)),
                                    {   
                                        Element(Button(BUTTON_STYLE_STYLIZED,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                        Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Smear"  , appTextures.smearIcon, 3.f,true)),
                                    {   
                                        Element(Button(BUTTON_STYLE_STYLIZED,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                        Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Area"  , appTextures.areaPaintingIcon, 3.f,true)),
                                    {   
                                        Element(Button(BUTTON_STYLE_STYLIZED,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Box"  , appTextures.TDModelIcon, 1.f,false)),
                                        Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Gradient"  , 1.f)),
                                        Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Depth Test"  , 1.f)),
                                        Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Polygonal Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Circle"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                    }
                                ),
                                Section(
                                    Element(Button(BUTTON_STYLE_BASIC,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Meshes"  , appTextures.modifierIcon, 3.f,true)),
                                    {   
                                    
                                    }
                                ),
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
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Textures"        , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Materials"       , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Brushes"         , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"3D Models"       , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Fonts"           , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Scripts"         , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Filters"         , Texture(), 0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Layers"          , Texture(), 0.f,true)),
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
                            shaders.buttonShader,
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
                                Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Add"        , Texture(), 0.f,false),
                                Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Import"        , Texture(), 0.f,false)
                            },
                            11.f,
                            true
                        );

    //Init the node editor displayer
    nodeEditorDisplayer  = Panel(
                            shaders.buttonShader,
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
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5.5f),colorPalette,shaders.buttonShader,""        , appTextures.greetingDialogImage, 0.f,false)),
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
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,5.5f),colorPalette,shaders.buttonShader,""        , appTextures.greetingDialogImage, 0.f,false)),
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
    paintingModesPanel  = Panel(
                            shaders.buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Draw"          , appTextures.brushIcon,        0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Soften"        , appTextures.softenIcon,       0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Smear"         , appTextures.smearIcon,        0.f,true)),
                                        Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Area"          , appTextures.areaPaintingIcon, 0.f,true)),
                                    }
                                )
                            },
                            
                            glm::vec2(5 ,8), //Initial scale value
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

    //Init dialogs
    greetingDialog              = GreetingDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
    newTextureDialog            = NewTextureDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
    displayerDialog             = DisplayerDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures,shaders.prefilteringShader,shaders.skyboxBall,sphereModel);
    newProjectDialog            = NewProjectDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
    loadProjectDialog           = LoadProjectDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
    materialEditorDialog        = MaterialEditorDialog(shaders.buttonShader,shaders.tdModelShader,colorPalette,appTextures,sphereModel);
    textureSelectionDialog      = TextureSelectionDialog(shaders.buttonShader,colorPalette);
    settingsDialog              = SettingsDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
    exportDialog                = ExportDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);

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
    renamingTextBox = TextBox(0,glm::vec2(4,2),colorPalette,shaders.buttonShader,"",4.f,false),context.window;
    renamingTextBox.thickness = 0.f;
    renamingTextBox.animationStyle = 1;
    renamingTextBox.radius = 10000.f;

}   