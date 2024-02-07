/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

struct ThreeDPoint{
    glm::vec3 pos = glm::vec3(0.f);
    glm::vec3 normal = glm::vec3(0.f);

    bool active = false;
    bool moving = false;

    int detailI = 0;

    Button btn;
    
    ThreeDPoint(){}
    ThreeDPoint(Button btn, glm::vec3 pos){
        this->btn = btn;
        this->pos = pos;
    }

    void render(Timer& timer, bool doMouseTracking, Painter& painter, bool stencilTest, float radius);
};

/// Used to render and handle the movable, resizable or rotatable dynamic textures (Used for painting over and layer system) 
class TextureField{
public:
    Texture texture;
    Button textureDisplayingButton;
    Button textureDisplayingButtonIOutline;

    bool wrapMode = false;
    bool lastWrapMode = false;
    
    ThreeDPoint threeDPointTopLeft;
    ThreeDPoint threeDPointTopRight;
    ThreeDPoint threeDPointBottomLeft;
    ThreeDPoint threeDPointBottomRight;
    
    ThreeDPoint detailed_threeDPoint_r1_c2;
    ThreeDPoint detailed_threeDPoint_r1_c3;
    ThreeDPoint detailed_threeDPoint_r1_c4;

    ThreeDPoint detailed_threeDPoint_r2_c1;
    ThreeDPoint detailed_threeDPoint_r2_c2;
    ThreeDPoint detailed_threeDPoint_r2_c3;
    ThreeDPoint detailed_threeDPoint_r2_c4;
    ThreeDPoint detailed_threeDPoint_r2_c5;
    
    ThreeDPoint detailed_threeDPoint_r3_c1;
    ThreeDPoint detailed_threeDPoint_r3_c2;
    ThreeDPoint detailed_threeDPoint_r3_c3;
    ThreeDPoint detailed_threeDPoint_r3_c4;
    ThreeDPoint detailed_threeDPoint_r3_c5;
    
    ThreeDPoint detailed_threeDPoint_r4_c1;
    ThreeDPoint detailed_threeDPoint_r4_c2;
    ThreeDPoint detailed_threeDPoint_r4_c3;
    ThreeDPoint detailed_threeDPoint_r4_c4;
    ThreeDPoint detailed_threeDPoint_r4_c5;
    
    ThreeDPoint detailed_threeDPoint_r5_c2;
    ThreeDPoint detailed_threeDPoint_r5_c3;
    ThreeDPoint detailed_threeDPoint_r5_c4;
    
    ThreeDBox threeDWrapBox;

    glm::vec2 scale;
    glm::vec3 pos;
    float rotation = 0.f;
    bool rotating = false;

    bool flippedH = false;
    bool flippedV = false;

    Button topLeft_ResizeButton;
    Button bottomLeft_ResizeButton;
    Button topRight_ResizeButton;
    Button bottomRight_ResizeButton;
    
    Button rotateBtn;

    Button deleteButton;
    Button scaleToTextureResolutionButton;
    Button flipHorizontalButton;
    Button flipVerticalButton;
    Button cancelRotationButton;
    Button rotationAngleDisplayButton;
    Button wrapModeButton;
    
    Button wrap_deleteButton;
    Button wrap_flipHorizontalButton;
    Button wrap_flipVerticalButton;
    Button wrap_unwrapModeButton;
    Button wrap_detailModeButton;

    bool active = false;

    // Set to true in the last frame transformation done
    bool transformedFlag = false;

    TextureField(){}
    TextureField(Texture texture);

    void render(Timer& timer, bool doMouseTracking, bool generatingTextureMode, std::vector<TextureField>& srcVector, int& i, bool renderTheTexture, Painter& painter, bool anyPanelHover, bool anyDialogActive);
    
    bool isHover();
    void updateWrapBox(Painter& painter);


private:
    float prevRot;
    glm::vec2 prevScale;
    glm::vec3 prevPos;
    bool prevTransformedFlag = false;

    void setDetailedWrapPoints(Painter& painter);
    void unplaceWrapPoints();
    
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

    void renderObjectsPanel(Timer& timer, Painter& painter);
    void renderPaintingModesPanel(Timer& timer, Painter& painter, float screenGapPerc);
    void render2DPaintingScene(Timer& timer, Painter& painter, float screenGapPerc);
    void renderPaintingChannelsTextureSelectionPanel(Timer& timer, Painter& painter);
    void renderSceneInfoWrapModeCheckbox(Timer& timer, Painter& painter);
    void renderPaintingPanel(Timer& timer, Painter &painter, float screenGapPerc);
    void renderPaintingOverTextureFields(Timer& timer, Painter& painter);


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
    Button textureSelectedObjectsButton;
    Panel objectsPanel;

    Gizmo sceneGizmo;
    Button currentModeDisplayer;
    Button currentModeHintDisplayer;
    CheckBox wrapModeCheckbox;
    
    CheckComboList faceSelectionCheckComboList;
    CheckComboList paintingOverCheckComboList;
    
    Panel paintingPanel;
    Panel paintingPanelModePanel;
    Section colorSection; 
    Section brushSection; 
    Section meshSection; 
    Section mirrorSection; 
    Section paintingOverSection; 
    std::vector<Section> paintingChannelsSection;
    Section cantBeDisplayedSection; 
    Button paintingPanelModeDisplayer;
    int selectedPaintingPanelMode = 0;
    Panel paintingChannelsTextureSelectionPanel;
    bool paintingChannelsTextureSelectionPanelActive = false;
    Material paintingSectionDisplayMat;
    Material paintingCustomMat;

    bool anyPanelHover = false;

    Dropper dropper;

    std::vector<TextureField> paintingOverTextureFields;

    /* -- DIALOGS -- */

    MaterialEditorDialog materialEditorDialog;
    LogDialog logDialog;
    GreetingDialog greetingDialog;
    NewProjectDialog newProjectDialog;
    ExportDialog exportDialog;
    MaterialDisplayerDialog materialDisplayerDialog;
    FilterDisplayerDialog filterDisplayerDialog;
    LoadProjectDialog loadProjectDialog;
    TextureSelectionDialog textureSelectionDialog;
    BakingDialog bakingDialog;
    FilterSelectionDialog filterSelectionDialog;
    NewTextureDialog newTextureDialog;
    PaintingChannelsAutoCreateTexturesDialog paintingChannelsAutoCreateTexturesDialog;
    SettingsDialog settingsDialog;
    DisplayerDialog displayerDialog;
    TextureEditorDialog textureEditorDialog;
    TexturePackEditorDialog texturePackEditorDialog;
    ProjectRecoverDialog projectRecoverDialog;
    ObjectTexturingDialog objectTexturingDialog;
    MaterialSelectionDialog materialSelectionDialog;

    float twoDPaintingSceneScroll = 2.f;
    glm::vec2 twoDPaintingScenePos = glm::vec2(0.f);
    Box twoDPaintingBox;

    int frameCounter = 0; 

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
    void init(Websites websites, Painter& painter);
    
    /*! @brief Renders all the GUI */
    void render(Timer &timer,Project &project, Painter &painter, Skybox &skybox);

private: 
    void elementInteraction(Painter &painter,Timer &timer, float screenGapPerc,
                            Project& project);

    void renderPanels(Timer &timer, Painter &painter, float screenGapPerc);

    void renderRenamingTextbox(Timer &timer, Painter &painter);

    void renderDialogs(Timer &timer, Project &project, Skybox &skybox, Painter& painter);

    void renderDropper(Painter &painter);

    void contextMenuInteraction(Timer &timer, Project& project, Painter &painter);

    void libraryPanelLeftInteraction(Panel &libraryPanelLeft);
    void paintingPanelInteraction(Painter &painter);
    void libraryPanelDisplayerInteraction(Painter &painter, Timer& timer, Project& project);


    void panelPositioning(float &screenGapPerc,  Painter &painter);
};

#endif //LIGID_GUI_HPP