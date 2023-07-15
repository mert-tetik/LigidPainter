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

#ifndef LIGID_GUI_HPP
#define LIGID_GUI_HPP

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"
#include "GUI/Node/Node.hpp"


struct Dropper{
    glm::vec3 value;
    bool active = false;
};

struct NodePanel{
    glm::vec2 position = glm::vec2(0);
    float scroll = 1.f;
};

// Forward declared 
struct AppSettings;

class UI{
private:
    Shaders shaders; 
	AppTextures appTextures;
    Model sphereModel;
    Websites websites;

    /*Renaming*/
    /*! @brief Textbox element used to rename library elements */
    TextBox renamingTextBox; 
    bool renamingTextBoxClosed = false;
    glm::ivec2 renamingIndices; //x for the context menu index, y for the element index
    std::string lastTitleBeforeRenaming;
public:
    /* -- PANELS -- */
    
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

    /* -- DIALOGS -- */

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

    int frameCounter = 0; 

    int selectedMaterialIndex = 0; //In the library (to manipulate with)
    
    bool anyContextMenuActive = false;

    bool anyDialogActive = false;

    NodePanel nodePanel;

    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    /*! @brief LigidPainter color theme */
    ColorPalette colorPalette;

    /*! @brief Default constructor */
    UI();

    /*! @brief Initialized all the GUI elements. Creates the panels & stuff. And takes the parameters to the member variables. */
    void init(Shaders shaders,Context context,AppTextures appTextures,Websites websites,glm::vec2 videoScale,Model &sphereModel);
    
    /*! @brief Renders all the GUI */
    void render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &meshNodeScene,
                std::vector<ContextMenu> &contextMenus, AppSettings& settings, Project &project, Painter &painter, Skybox &skybox,Model &model);

private: 
    void elementInteraction(Painter &painter,Mouse &mouse, Library &library,std::vector<ContextMenu> &contextMenus,std::vector<Node> &meshNodeScene,
                            Context &context,glm::vec2 &videoScale,TextRenderer &textRenderer, Timer &timer, int &textureRes,float screenGapPerc,Model &model,Project& project);

    void renderPanels(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library);

    void renderRenamingTextbox(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library, Context &context);

    void renderNodes(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene);

    void renderDialogs(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene,Context &context, Project &project, Model& model, Skybox &skybox, AppSettings &settings, Box &box, std::vector<ContextMenu> &contextMenus);

    void renderDropper(Mouse &mouse,Painter &painter);

    void contextMenuInteraction(std::vector<ContextMenu> &contextMenus, Mouse &mouse , Library &library,std::vector<Node> &meshNodeScene,Context &context,glm::vec2 videoScale,Timer &timer,TextRenderer &textRenderer,Project& project,int &textureRes, Painter &painter);

    void libraryPanelLeftInteraction(Panel &libraryPanelLeft, Library &library,Mouse &mouse);

    void panelPositioning(float &screenGapPerc, Library &library, Painter &painter);
};

#endif //GUI_HPP