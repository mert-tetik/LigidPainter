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

    void render(Timer& timer, bool doMouseTracking, Painter& painter, bool stencilTest, float radius, bool canMove);
};

/*! @brief Used to render and handle the movable, resizable or rotatable dynamic textures (Used for painting over) 
            WRAP MODE : Texture field is being wrapped around the 3D model in 3D scene instead of being rendered in 2D
*/
class TextureField{
public:
    /*------------------------------- GENERAL  -------------------------------*/
    
    /*! @brief Texture of the texture field (this texture is being rendered)*/
    Texture texture;
    
    /*! @brief This flag is used to determine if the texture field is wrap mode or not*/
    bool wrapMode = false;

    /*! @brief Texture field is selected */
    bool active = false;
    
    /*! @brief Default constructor*/
    TextureField(){}
    
    /*! @brief Constructor*/
    TextureField(Texture texture);

    /*! @brief Render the texture field*/
    void render(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool anyPanelHover);

    
    /*------------------------------- 2D TEXTURE FIELD  -------------------------------*/

    /*! @brief This button displays the texture of the texture field in 2D mode */
    Button textureDisplayingButton;
    /*! @brief This button used to render an outline for the texture displayer in 2D mode */
    Button textureDisplayingButtonIOutline;
    /* These buttons are used to resize the texture field in 2D mode*/
    /**/Button topLeft_ResizeButton;
    /**/Button bottomLeft_ResizeButton;
    /**/Button topRight_ResizeButton;
    /**/Button bottomRight_ResizeButton;
    /*! @brief This button is being rendered right into the center of the texture field and rotates it */
    Button rotateBtn;
    /* These buttons are being rendered if the texture field is active and lets you modify it*/
    /**/Button deleteButton; // Deletes the related texture field
    /**/Button scaleToTextureResolutionButton; // Auto-Scales the texture field to the resolution ratio of the texture
    /**/Button flipHorizontalButton; // Flips the texture horizontally
    /**/Button flipVerticalButton; // Flips the texture vertically
    /**/Button cancelRotationButton; // Sets the rotation to 0
    /**/Button rotationAngleDisplayButton; // Displays the rotation value
    /**/Button wrapModeButton; // Enables the wrap mode
    /*! @brief Scale of the texture field */
    glm::vec2 scale;
    /*! @brief Position of the texture field */
    glm::vec3 pos;
    /*! @brief Rotation value of the texture field in degrees */
    float rotation = 0.f;
    /*! @brief Returns true if one of texture field elements is hovered */
    bool is2DModeHovered();
    /*! @brief Auto calculates the scale according the the resolution ratio of the txtr*/
    void twoDScaleAccordingToTextureRes();
    /*! @brief Positions & scales all the elements used to render 2D texture field*/
    void position2DElements();
    /*! @brief This flag set to true if the texture is flipped vertically (used for undoing)*/
    bool flippedV = false;
    /*! @brief This flag set to true if the texture is flipped horizontally (used for undoing)*/
    bool flippedH = false;



    /*! @brief Render the 2D texture field */
    void render2DTextureField(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool anyPanelHover);
    
    /*------------------------------- WRAP MODE  -------------------------------*/

    /* Points of the wrap mode */
    
    /* 4 main points */
    /**/ThreeDPoint threeDPointTopLeft;
    /**/ThreeDPoint threeDPointTopRight;
    /**/ThreeDPoint threeDPointBottomLeft;
    /**/ThreeDPoint threeDPointBottomRight;
    
    /* Detailed mode points*/
    /**/ThreeDPoint detailed_threeDPoint_r1_c2;
    /**/ThreeDPoint detailed_threeDPoint_r1_c3;
    /**/ThreeDPoint detailed_threeDPoint_r1_c4;
    /**/ThreeDPoint detailed_threeDPoint_r2_c1;//
    /**/ThreeDPoint detailed_threeDPoint_r2_c2;
    /**/ThreeDPoint detailed_threeDPoint_r2_c3;
    /**/ThreeDPoint detailed_threeDPoint_r2_c4;
    /**/ThreeDPoint detailed_threeDPoint_r2_c5;
    /**/ThreeDPoint detailed_threeDPoint_r3_c1;//
    /**/ThreeDPoint detailed_threeDPoint_r3_c2;
    /**/ThreeDPoint detailed_threeDPoint_r3_c3;
    /**/ThreeDPoint detailed_threeDPoint_r3_c4;
    /**/ThreeDPoint detailed_threeDPoint_r3_c5;
    /**/ThreeDPoint detailed_threeDPoint_r4_c1;//
    /**/ThreeDPoint detailed_threeDPoint_r4_c2;
    /**/ThreeDPoint detailed_threeDPoint_r4_c3;
    /**/ThreeDPoint detailed_threeDPoint_r4_c4;
    /**/ThreeDPoint detailed_threeDPoint_r4_c5;
    /**/ThreeDPoint detailed_threeDPoint_r5_c2;//
    /**/ThreeDPoint detailed_threeDPoint_r5_c3;
    /**/ThreeDPoint detailed_threeDPoint_r5_c4;
    
    /*! @brief Auto-Generates new position values for the detail points (according to 4 main points)*/
    void setDetailedWrapPoints(Painter& painter);
    
    /*! @brief Sets the positions of the points to 0*/
    void unplaceWrapPoints();

    /*! @brief Places the first (top left) point on the 3D model 
        (Call if mouse click & edit mode) */
    void placeFirstPoint(Painter& painter, Framebuffer bindedFBO);
    
    /*! @brief Places the second (bottom right) point on the 3D model.
                Then auto-places rest of the points in a way to form a square 
        (Call if mouse click & edit mode) */
    void placeSecondPoint(Painter& painter, Framebuffer bindedFBO);

    /*! @brief Renders all the points (detailed points too if detail mode is active)*/
    void renderPoints(Timer& timer,Painter& painter, bool doMouseTracking);

    /*! @brief Returns true if any of the points moved */
    bool didAnyWrapPointMove();

    /*! @brief Returns true if any of the points is active */
    bool isAnyWrapPointActive();

    /*! @brief This class manages vertex buffers, generating subdivided plane mesh & wrapping 
        (basically this class used for rendering the texture in wrap mode) */
    ThreeDBox threeDWrapBox;

    /*! @brief Renders the wrapped texture (threeDWrapBox) */
    void renderWrappedTextureBox(bool generatingTextureMode);
    
    /*! @brief Checks if the cursor is located on top of the wrapped texture
        (called if mouse left button clicked & other conditions are set like no panel was hovered too)*/
    void checkIfWrappedTextureClicked(Framebuffer bindedFBO);


    /*! Updates the threeDWrapBox */
    void updateWrapBox(Painter& painter);
    
    /* These buttons are being rendered if the wrapped texture field is active and lets you modify it*/
    /**/Button wrap_deleteButton; // Deletes the texture field
    /**/Button wrap_flipHorizontalButton; // Flips the texture coordinates in x axis of the 3D box (wrapped texture) (sets a flag in ThreeDBox)
    /**/Button wrap_flipVerticalButton; // Flips the texture coordinates in y axis of the 3D box (wrapped texture) (sets a flag in ThreeDBox)
    /**/Button wrap_unwrapModeButton; // Disables the wrap mode
    /**/Button wrap_detailModeButton; // Enables the detail mode (check if the detail mode is active by checking if this button's clickState1 set to true)
    
    /*! @brief Renders wrap modify buttons labeled with "wrap_" (only called if the texture field is active) */
    void renderWrappedModifyElements(Timer& timer, bool anyPanelHover);

    /*! @brief Render the wrapped texture field */
    void renderWrappedTextureField(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool anyPanelHover);

    /*! @brief Renders info texts about placing first points of the wrapped texture field
               Only called if top left and bottom right points set to vec3(0)
               And also renders a cancel button that can disable the wrap mode*/
    void renderWrapPointDecidingScene(Timer& timer, Framebuffer bindedFBO);

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