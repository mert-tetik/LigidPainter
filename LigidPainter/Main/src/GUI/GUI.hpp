/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef GUI_HPP
#define GUI_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"

//Forward decleration


struct Dropper{
    glm::vec3 value;
    bool active = false;
};


class UI{
private:
    Shaders shaders; 
	AppTextures appTextures;
    Model sphereModel;
    Websites websites;
public:
    //UI Elements
    //(Mostly panels)
    Panel navigationPanel;
    Panel windowPanel;
    Panel paintingPanel; //Main panel (kinda)
    Panel libraryPanelLeft; 
    Panel libraryPanelDisplayer; 
    Panel nodeEditorDisplayer; 
    Panel selectedTextureDisplayer; 
    Panel twoDPaintingPanel; 
    Panel paintingModesPanel; 

    bool anyPanelHover = false;

    Dropper dropper;

    //Dialogs    
    GreetingDialog greetingDialog;
    NewProjectDialog newProjectDialog;
    ExportDialog exportDialog;
    LoadProjectDialog loadProjectDialog;
    //ColorPickerDialog colorPickerDialog;
    MaterialEditorDialog materialEditorDialog;
    TextureSelectionDialog textureSelectionDialog;
    NewTextureDialog newTextureDialog;
    SettingsDialog settingsDialog;
    DisplayerDialog displayerDialog;

    TextBox renamingTextBox; //This textbox is used to rename library elements
    bool renamingTextBoxClosed = false;
    glm::ivec2 renamingIndices; //x for the context menu index, y for the element index
    std::string lastTitleBeforeRenaming;

    int frameCounter = 0; //Reset every 1000 frame

    int selectedMaterialIndex = 0; //In the library (to manipulate with )
    
    bool anyContextMenuActive = false;

    bool anyDialogActive = false;

    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    ColorPalette colorPalette;


    //Constructor
    UI();

    //Public member functions
    void init(Shaders shaders,Context context,AppTextures appTextures,Websites websites,glm::vec2 videoScale,Model &sphereModel);
    void render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                std::vector<ContextMenu> &contextMenus,int &textureRes, Project &project, Painter &painter,bool &VSync,bool &backfaceCulling,Skybox &skybox,Model &model);

private: 
    void elementInteraction(Painter &painter,Mouse &mouse, Library &library,std::vector<ContextMenu> &contextMenus,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                            Context &context,glm::vec2 &videoScale,TextRenderer &textRenderer, Timer &timer, int &textureRes,float screenGapPerc,Model &model,Project& project);

    void renderPanels(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library);

    void renderRenamingTextbox(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library, Context &context);

    void renderNodes(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &nodeScene);

    void renderDialogs(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &nodeScene,Context &context, Project &project, Model& model, Skybox &skybox, int &textureRes, bool &VSync,bool &backfaceCulling, Box &box, std::vector<Node> &appNodes, std::vector<ContextMenu> &contextMenus);

    void renderDropper(Mouse &mouse,Painter &painter);

    void contextMenuInteraction(std::vector<ContextMenu> &contextMenus, Mouse &mouse , Library &library,std::vector<Node>& appNodes ,std::vector<Node> &nodeScene,Context &context,glm::vec2 videoScale,Timer &timer,TextRenderer &textRenderer,Project& project,int &textureRes, Painter &painter);

    void libraryPanelLeftInteraction(Panel &libraryPanelLeft, Library &library,Mouse &mouse);


    void panelPositioning(float &screenGapPerc, Library &library, Painter &painter);
};

#endif //GUI_HPP