/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIGID_TEXTURE_FIELD_SCENE_HPP
#define LIGID_TEXTURE_FIELD_SCENE_HPP

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"

/*! @brief Used to render and handle the movable, resizable or rotatable dynamic textures (Used for painting over) 
            WRAP MODE : Texture field is being wrapped around the 3D model in 3D scene instead of being rendered in 2D
*/
class TextureField{
public:
    /*------------------------------- GENERAL  -------------------------------*/
    
    /*! @brief Texture of the texture field (this texture is being rendered)*/
    Texture texture;
    
    /*! @brief This flag is used to determine if the texture field is in wrap mode or not*/
    bool wrapMode = false;

    /*! @brief Texture field is selected */
    bool active = false;
    
    /*! @brief Default constructor*/
    TextureField(){}
    
    /*! @brief Constructor*/
    TextureField(Texture texture);

    /*! @brief Render the texture field*/
    void render(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool doMouseTracking);

    
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
    void render2DTextureField(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool doMouseTracking);
    
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
    void checkIfWrappedTextureClicked(Framebuffer bindedFBO, Painter& painter, bool doMouseTracking);


    /*! Updates the threeDWrapBox */
    void updateWrapBox(Painter& painter);
    
    /* These buttons are being rendered if the wrapped texture field is active and lets you modify it*/
    /**/Button wrap_deleteButton; // Deletes the texture field
    /**/Button wrap_flipHorizontalButton; // Flips the texture coordinates in x axis of the 3D box (wrapped texture) (sets a flag in ThreeDBox)
    /**/Button wrap_flipVerticalButton; // Flips the texture coordinates in y axis of the 3D box (wrapped texture) (sets a flag in ThreeDBox)
    /**/Button wrap_unwrapModeButton; // Disables the wrap mode
    /**/Button wrap_detailModeButton; // Enables the detail mode (check if the detail mode is active by checking if this button's clickState1 set to true)
    
    /*! @brief Renders wrap modify buttons labeled with "wrap_" (only called if the texture field is active) */
    void renderWrappedModifyElements(Timer& timer, bool doMouseTracking);

    /*! @brief Render the wrapped texture field */
    void renderWrappedTextureField(Timer& timer, Painter& painter, std::vector<TextureField>& srcVector, int& srcVectorI, bool editMode, bool generatingTextureMode, bool doMouseTracking);

    /*! @brief Renders info texts about placing first points of the wrapped texture field
               Only called if top left and bottom right points set to vec3(0)
               And also renders a cancel button that can disable the wrap mode*/
    void renderWrapPointDecidingScene(Timer& timer, Framebuffer bindedFBO);

};

class TextureFieldScene{
public:
    std::vector<TextureField> texture_fields;

    Texture painting_over_texture;

    void render(Timer& timer, Painter& painter, bool doMouseTracking, bool threeD_only, bool editMode);
    void add_new(TextureField texture_field);
    void update_painting_over_texture(bool threeD_only, Painter& painter);
    
    TextureFieldScene(){}
    
    TextureFieldScene(std::vector<TextureField> texture_fields){
        this->texture_fields = texture_fields;

        //--------- init paintingOverTexture --------- 
        this->painting_over_texture = Texture(nullptr, 1024, 1024, GL_LINEAR);

        this->FBO = Framebuffer(
                                this->painting_over_texture, 
                                GL_TEXTURE_2D, 
                                Renderbuffer(
                                                GL_DEPTH_COMPONENT16, 
                                                GL_DEPTH_ATTACHMENT, 
                                                glm::ivec2(1024)
                                            ), 

                                "Update painting over texture"
                            );
    }
private:
    /*! @brief Used to capture painting over texture*/
    Framebuffer FBO;
};

#endif