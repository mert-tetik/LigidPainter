/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef PAINTER_HPP
#define PAINTER_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Util.hpp"
#include "3D/Material/Material.hpp"


struct PaintingBuffers{
    /// @brief 3D model linearized depth texture (3d model rendered with a depth shader) 
    Texture depth_texture;

    Texture window_painting_texture;

    Texture projected_painting_texture;
    Texture projected_painting_texture_low;
};

struct MirrorSide{
    bool active = false;

    PaintingBuffers paintingBuffers;

    /*! @brief Whichever axis this mirror side effects becomes -1 and other axises being set to 1. For instance glm::vec3(-1.f, 1.f, 1.f) effects only the x axis*/
    glm::vec3 effectAxis = glm::vec3(0.);
};

struct MirrorSettings{
    bool X = false;
    float X_offset;

    bool Y = false;
    float Y_offset;

    bool Z = false;
    float Z_offset;
};

struct PaintSettings{
    struct ColorBuffer{
        Color stroke_albedo_color;
        float stroke_roughness_color = 0.f;
        float stroke_metallic_color = 0.f;
        float stroke_normalMap_color = 0.f;
        float stroke_heightMap_color = 0.f;
        float stroke_ao_color = 0.f;
    };
    struct DrawMode{
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
        ColorBuffer stroke_color_buffer;
    };
    struct SoftenMode{
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
        float softening_strength = 0.f;
    };
    struct SmearMode{
        float smear_strength = 0.f;
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
        float smear_direction = 0.f;
    };
    struct NormalMode{
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
        float normal_strength = 0.f;
    };
    struct FilterMode{
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
        Filter filter;
    };
    struct BucketMode{
        glm::vec2 click_location;
        ColorBuffer bucket_color_buffer;
    };

    struct PaintVertexBuffer{
        /*! @brief Paint on a 3D model*/
        Mesh* model_mesh;

        /*! @brief Paint on a 2D square*/
        Box* box;

        /*! @brief Paint on model_mesh if set to true, box if else*/
        bool paint_model = false;
    };

    struct PointData{
        glm::vec2 point_2D;
        ThreeDPoint point_3D; 

        bool use_3D = false;
    };


    /*! @brief Paint painting_channels if set to true. Paint painting_texture if set to false*/
    bool material_painting = false;

    /*! @brief These textures will be painted if material_painting flag set to true*/
    MaterialChannels painting_channels;

    /*! @brief These texture will be painted if material_painting flag set to false*/
    Texture painting_texture;

    MirrorSettings mirror_settings;

    PaintVertexBuffer vertex_buffer;
    
    PointData point;

    /*! @brief 0 : Draw Mode, 1 : Soften Mode, 2 : Smear Mode, 3 : Normal Mode, 4 : Filter Mode, 5 Bucket Mode, */ 
    int painting_mode;
    DrawMode draw_mode;
    SoftenMode soften_mode;
    SmearMode smear_mode;
    NormalMode normal_mode;
    FilterMode filter_mode;
    BucketMode bucket_mode;
};

class Painter
{
public:
    bool wrapMode = false;

    /// @brief Is paintingTexture16f if smearBrush used, paintingTexture8 if not
    Texture paintingTexture;  

    Texture projectedPaintingTexture;
    Texture projectedPaintingTexture8Low;
    Texture projectedPaintingTexture16fLow;
    Texture projectedPaintingTexture8;
    Texture projectedPaintingTexture16f;

    FaceSelection faceSelection;

    /// @brief To avoid refreshing every frame in RendererRender.cpp
    /// Refreshing : Update the selected texture & clean the capturing framebuffer
    bool refreshable = true; 
    
    /// @brief Brush property values 
    /// Set by the GUI elements (painting panel (brush sections) )
    /// Is the same as the GUI elements
    BrushProperties brushProperties;

    /// @brief Painter constructor (use initPainter to initialize the painter class)
    Painter();
    
    /*! 
    * @brief Init the painter class. Create textures, buffer objects & get necessary member variables
    */
    void initPainter();
    
    /*! @brief Painting the strokeLocations */
    void doPaint(std::vector<glm::vec2> strokeLocations, bool firstStroke, int paintingMode, bool highResMode);
    
    /*! @brief Painting the cursor location & generating strokeLocations*/
    void doPaint(bool wrapMode, bool firstStroke, int paintingMode, bool highResMode);
    
    /*! @brief Painting the manual point (cursorPos) & generating strokeLocations*/
    void doPaint(glm::vec2 cursorPos, bool wrapMode, bool firstStroke, int paintingMode, bool highResMode);
    
    /*! @brief Painting the 3D point & generating strokeLocations*/
    void doPaint(ThreeDPoint threeDPoint, bool firstStroke, int paintingMode, bool highResMode);

    /*! 
    * @brief do painting (paint 2D). Called every frame if painting conditions are set. 
    *           painting conditions are : mouse left button pressed & cursor not hover any panel etc. 
    */
    void paintBuffers(std::vector<glm::vec2> strokeLocations, bool wrapMode, bool firstStroke, int paintingMode, bool highResMode);
    
    /*!
    * @brief call that function in a single frame as the painting is completed (Mouse left button released)
    *        refreshPainting clears the @ref paintingFBO's color data (clears the paintingTexture)      
    */
    void refreshPainting();

    /*!
    * @brief updates the @ref panel_library_selected_texture right after painting is done.
    *        Use this right before the @ref refreshPainting  
    * @param textures textures in the library 
    * @param model the 3D model
    * @param scene structure contains matrices related to 3D model rendering & cam pos
    */
    void updateTexture(int paintingMode);
    void updateTheTexture(Texture txtr, int paintingMode, glm::vec3 paintingColor, int channelI, float channelStr);

    
    /*!
    * @brief updates the @ref depthTexture right after painting is done.
    *        Use this right before the @ref refreshPainting  
    */
    void updateDepthTexture();

    /// @brief renderbuffer object used to depth test (used to create the depth texture)
    Renderbuffer depthRBO1024; 
    Renderbuffer depthRBO512; 
    Renderbuffer depthRBOcustom; 

    /*!
    * @brief framebuffer object used to capture paintingTexture
    *   Always uses the paintingTexture as it's texture buffer
    */
    Framebuffer paintingFBO; 

    /// @brief RGBA8
    Texture paintingTexture8; 
    
    /// @brief RGBA16F (Used for smear brush)
    Texture paintingTexture16f;

    Texture paintingBGTexture; 
    
    glm::vec2 startCursorPos = glm::vec2(0); 
    glm::vec2 lastCursorPos = glm::vec2(0); 
    int frameCounter = 0;
    
    std::vector<glm::vec2> getCursorSubstitution(float spacing, glm::vec2 start, glm::vec2 dest);
    void changeColor(Color &color);

    glm::ivec2 getResolution();

    unsigned int getBufferResolutions(int bufferIndex);

    void updatePaintingOverTexture();

private:
    void projectThePaintingTexture(Texture& selectedTexture, Texture& projectedPaintingTexture,  unsigned int paintingTexture,  unsigned int depthTexture, 
                                            int selectedPaintingModeIndex, float brushPropertiesOpacity,
                                            glm::mat4 viewMat,bool faceSelectionActive, Texture selectedPrimitives, bool wrapMode);

    void generateMirroredProjectedPaintingTexture(Texture paintingTxtrObj, Texture& selectedTexture,  Texture& projectedPaintingTexture,  
                                                    int selectedPaintingModeIndex, float brushPropertiesOpacity,  
                                                    bool faceSelectionActive, Texture selectedPrimitives, bool highResMod, bool firstStroke, bool wrapMode
                                                );
};

#endif