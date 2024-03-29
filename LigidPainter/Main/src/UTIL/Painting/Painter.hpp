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

        Material material;
        bool use_custom_material = false;
    };
    struct DrawMode{
        /*! @brief This brush will be used for the strokes;*/
        Brush stroke_brush;
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

    struct PaintingOverData{
        bool active = false;
        bool gray_scale = false;
        TextureFieldScene* texture_field_scene;
    };
    struct PaintedBuffers{
        /*! @brief Paint material_channel textures if set to true. Paint solid_painted_texture if set to false*/
        bool material_painting = false;

        bool material_channel_albedo_active = false;
        Texture material_channel_albedo;
        bool material_channel_roughness_active = false;
        Texture material_channel_roughness;
        bool material_channel_metallic_active = false;
        Texture material_channel_metallic;
        bool material_channel_normalMap_active = false;
        Texture material_channel_normalMap;
        bool material_channel_heightMap_active = false;
        Texture material_channel_heightMap;
        bool material_channel_ao_active = false;
        Texture material_channel_ao;

        /*! @brief Only this texture will be painted if material_painting flag set to false*/
        Texture solid_painted_texture;
    };

    PaintingOverData painting_over_data;
    
    /*! @brief This color values will be used for painting (draw mode & bucket mode : directly paint this color, any other modes : Multiply the stroke value with these colors (black : no effect, white : full effect))*/
    ColorBuffer color_buffer;

    /*! @brief Set which textures will be painted*/
    PaintedBuffers painted_buffers;

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

extern Framebuffer painting_projected_painting_FBO;

void painting_paint_buffers(PaintSettings settings, bool first_frame, bool last_frame);

#endif