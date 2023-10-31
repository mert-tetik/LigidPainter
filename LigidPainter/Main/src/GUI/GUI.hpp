/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
#include "ContextMenuSystem/ContextMenus.hpp"

struct Dropper{
    glm::vec3 value;
    bool active = false;
};

struct NodePanel{
    glm::vec2 position = glm::vec2(0);
    float scroll = 1.f;
    glm::vec2 mixVal = glm::vec2(0);
    bool hitBoundaries = false;
    int boundaryState = 0;

};

/// Used to render and handle the movable, resizable or rotatable dynamic textures (Used for painting over and layer system) 
class TextureField{
public:
    Texture texture;
    Button textureDisplayingButton;
    Button textureDisplayingButtonIOutline;

    glm::vec2 scale;
    glm::vec3 pos;

    Button topLeft_ResizeButton;
    Button bottomLeft_ResizeButton;
    Button topRight_ResizeButton;
    Button bottomRight_ResizeButton;

    Button deleteButton;
    Button scaleToTextureResolutionButton;

    bool active = false;

    // Set to true in the last frame transformation done
    bool transformedFlag = false;

    TextureField(){}
    TextureField(Texture texture);

    void render(Timer& timer, bool doMouseTracking, bool generatingTextureMode, std::vector<TextureField>& srcVector, int& i);
    
    bool isHover();

private:
    glm::vec2 prevScale;
    glm::vec3 prevPos;
    bool prevTransformedFlag = false;
};

class UI{
private:
    Websites websites;

    /*Renaming*/
    /*! @brief Textbox element used to rename library elements */
    TextBox renamingTextBox; 
    Button zoomingDisplayingButton;
    bool renamingTextBoxClosed = false;
    glm::ivec2 renamingIndices = glm::vec2(0,0); //x for the context menu index, y for the element index
    std::string lastTitleBeforeRenaming;

    bool prevStraightLinePaintingCondition = false;
    glm::vec2 straightLinePaintingStartPos = glm::vec2(50.f);
    glm::vec2 straightLinePaintingDirectionPos = glm::vec2(50.f);
public:
    /* -- PANELS -- */
    
    Panel navigationPanel;
    Panel windowPanel;
    Panel libraryPanelLeft; 
    Panel libraryPanelDisplayer; 
    Panel nodeEditorDisplayer; 
    Panel selectedTextureDisplayer; 
    Panel twoDPaintingPanel; 
    Panel paintingModesPanel; 
    Panel vectorPaintingModePropertyPanel; 
    Panel smearPaintingModePropertyPanel;
    Panel displayingModesPanel; 
    Button filterPaintingModeFilterBtn;
    
    Panel paintingPanel;
    Panel paintingPanelModePanel;
    Button paintingPanelModeDisplayer;
    int selectedPaintingPanelMode = 0;
    Section colorSection; 
    Section brushSection; 
    Section meshSection; 
    Section paintingChannelsSection; 
    Section mirrorSection; 
    Section paintingOverSection; 

    bool anyPanelHover = false;

    Dropper dropper;

    std::vector<TextureField> paintingOverTextureFields;

    /* -- DIALOGS -- */

    LogDialog logDialog;
    GreetingDialog greetingDialog;
    NewProjectDialog newProjectDialog;
    ExportDialog exportDialog;
    MaterialDisplayerDialog materialDisplayerDialog;
    FilterDisplayerDialog filterDisplayerDialog;
    LoadProjectDialog loadProjectDialog;
    MaterialEditorDialog materialEditorDialog;
    TextureSelectionDialog textureSelectionDialog;
    BakingDialog bakingDialog;
    FilterSelectionDialog filterSelectionDialog;
    NewTextureDialog newTextureDialog;
    SettingsDialog settingsDialog;
    DisplayerDialog displayerDialog;
    TextureEditorDialog textureEditorDialog;
    TexturePackEditorDialog texturePackEditorDialog;
    ProjectRecoverDialog projectRecoverDialog;

    float twoDPaintingSceneScroll = 2.f;
    glm::vec2 twoDPaintingScenePos = glm::vec2(0.f);
    Box twoDPaintingBox;

    int frameCounter = 0; 

    int selectedMaterialIndex = 0; //In the library (to manipulate with)
    Texture textureEditorSelectedTxtr;
    
    bool anyContextMenuActive = false;

    bool anyDialogActive = false;

    NodePanel nodePanel;

    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    /*! @brief Default constructor */
    UI();

    /*! @brief Initializes all the GUI elements. Creates the panels & stuff. And takes the parameters to the member variables. */
    void init(Websites websites);
    
    /*! @brief Renders all the GUI */
    void render(Timer &timer,Project &project, Painter &painter, Skybox &skybox);

private: 
    void elementInteraction(Painter &painter,Timer &timer, float screenGapPerc,
                            Project& project, AppMaterialModifiers& appMaterialModifiers);

    void renderPanels(Timer &timer, Painter &painter, float screenGapPerc);

    void renderRenamingTextbox(Timer &timer, Painter &painter);

    void renderDialogs(Timer &timer, Project &project, Skybox &skybox, Painter& painter);

    void renderDropper(Painter &painter);

    void contextMenuInteraction(Timer &timer, Project& project, Painter &painter, AppMaterialModifiers& appMaterialModifiers);

    void libraryPanelLeftInteraction(Panel &libraryPanelLeft);
    void paintingPanelInteraction(Painter &painter);
    void libraryPanelDisplayerInteraction(Painter &painter, AppMaterialModifiers& appMaterialModifiers, Timer& timer, Project& project);


    void panelPositioning(float &screenGapPerc,  Painter &painter);
};

#endif //LIGID_GUI_HPP