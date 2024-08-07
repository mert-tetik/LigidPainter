/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

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
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Wait/Wait.hpp"

#include "3D/Material/Material.hpp"

#include "GUI/Panels.hpp"

extern MirrorSide O_side;
extern MirrorSide X_side;
extern MirrorSide Y_side;
extern MirrorSide XY_side;
extern MirrorSide Z_side;
extern MirrorSide XZ_side;
extern MirrorSide YZ_side;
extern MirrorSide XYZ_side;

static std::vector<Renderbuffer> renderbuffers;
static Renderbuffer get_renderbuffer_for_texture(Texture texture){
    for (Renderbuffer rbo : renderbuffers)
    {
        if(rbo.getResolution() == texture.getResolution()){
            return rbo;
        }
    }
    
    Renderbuffer rbo = Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(texture.getResolution()));
    renderbuffers.push_back(rbo);
    return rbo;
}

static void set_brush_properties (
                                    BrushProperties brushProperties
                                )
{
    
    ShaderSystem::twoDPainting().setFloat("brush.radius", brushProperties.radius);
    ShaderSystem::twoDPainting().setFloat("brush.hardness", brushProperties.hardness);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - brushProperties.sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - brushProperties.scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", brushProperties.fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", brushProperties.rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - brushProperties.rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - brushProperties.alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", brushProperties.individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", brushProperties.sinWavePattern);
    
    //Bind the texture of the brush
    ShaderSystem::twoDPainting().setInt("brush.txtr", GL::bindTexture_2D((brushProperties.brushTexture.ID) ? brushProperties.brushTexture.ID : appTextures.white.ID, "PaintingUTIL : generate_projected_painting_texture"));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 0.f, 0.f, 0.f, 1.f };  // Replace r, g, b, a with the desired color values
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glGenerateMipmap(GL_TEXTURE_2D);
}

#define GET_DATA_FROM_PAINT_MODE(data, src)  if(settings.painting_mode == 0)\
                                                data = settings.draw_mode.src;\
                                            if(settings.painting_mode == 1)\
                                                data = settings.soften_mode.src;\
                                            if(settings.painting_mode == 2)\
                                                data = settings.smear_mode.src;\
                                            if(settings.painting_mode == 3)\
                                                data = settings.normal_mode.src;\
                                            if(settings.painting_mode == 4)\
                                                data = settings.filter_mode.src;\
                                            if(settings.painting_mode == 6)\
                                                data = settings.filter_mode.src;

static void window_paint(Texture* window_paint_texture, std::vector<glm::vec2> strokes, Brush brush, int frame_count, bool paint_offset_value){
    //Bind the painting texture to the painting framebuffer
    Texture copy_txtr = window_paint_texture->get_temp_copy_txtr("void window_paint");

    Framebuffer FBO = FBOPOOL::requestFBO(*window_paint_texture, "window_paint");

    float resolution = window_paint_texture->getResolution().x;

    glViewport(0, 0, resolution, resolution);
    //glClearColor(0,0,0,0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec2 scale = glm::vec2(0.5f);
    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 0.9f);
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);
    
    //Prepeare the 2D painting shader
    ShaderSystem::twoDPainting().use();
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen

    ShaderSystem::twoDPainting().setVec2("paintingRes", glm::vec2(resolution)); 
    ShaderSystem::twoDPainting().setVec2("videoScale", getContext()->windowScale); 
    ShaderSystem::twoDPainting().setInt("frame", frame_count);
    ShaderSystem::twoDPainting().setInt("bgTxtr", GL::bindTexture_2D(copy_txtr.ID, "paintingUTIL : window_paint"));
    ShaderSystem::twoDPainting().setInt("redChannelOnly", !paint_offset_value);
    ShaderSystem::twoDPainting().setVec3("rgbClr", glm::vec3(0.));
    
    //Set brush properties
    set_brush_properties(brush.properties);
    
    // Set the mouseOffset value
    if(strokes.size())
        ShaderSystem::twoDPainting().setVec2("mouseOffset", strokes[0] - strokes[strokes.size()-1]);
    
    // Set the stroke positions
    ShaderSystem::twoDPainting().setInt("posCount", strokes.size());
    for (int i = 0; i < strokes.size(); i++)
    {
        std::string target = "positions[" + std::to_string(i) + "]";
        ShaderSystem::twoDPainting().setVec2(target, strokes[i]);
    }
    
    //Prepeare painting
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    //Painting
    getBox()->draw("Painter::doPaint : Rendering 2D painted");
    
    //Finish
    GL::releaseBoundTextures("paintingUTIL : window_paint");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);
}

static void project_window_painting_texture(
                                                // Textures
                                                Texture* projected_painting_texture, 
                                                Texture window_painting_texture, 
                                                Texture depth_texture,
                                                
                                                Camera cam,
                                                PaintSettings::PaintVertexBuffer vertex_buffer_data,
                                                PaintSettings::PaintingOverData painting_over_data,
                                                glm::vec3 paint_color,
                                                
                                                // Painting data
                                                int painting_mode, 
                                                float brush_opacity, 
                                                bool wrapMode
                                            )
{

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(*projected_painting_texture, projected_painting_texture->getResolution(), "project_window_painting_texture");
    
    glClearColor(0, 0, 0, 0);
    
    if(wrapMode)
        glClear(GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Bind the related shader program
    ShaderSystem::projectingPaintedTextureShader().use();

    //*Fragment
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingTexture", GL::bindTexture_2D(window_painting_texture.ID, "paintingUTIL::project_window_painting_texture"));
    ShaderSystem::projectingPaintedTextureShader().setInt("depthTexture", GL::bindTexture_2D(depth_texture.ID, "paintingUTIL::project_window_painting_texture"));
    ShaderSystem::projectingPaintedTextureShader().setFloat("paintingOpacity", brush_opacity);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPaintingModeIndex", painting_mode);
    
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverTexture", GL::bindTexture_2D(painting_over_data.texture_field_scene->painting_over_texture.ID, "paintingUTIL::project_window_painting_texture"));
    ShaderSystem::projectingPaintedTextureShader().setInt("usePaintingOver", painting_over_data.active);
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverGrayScale", painting_over_data.gray_scale);
    ShaderSystem::projectingPaintedTextureShader().setVec3("paintingColor", paint_color);

    // Painting a 3D model
    if(vertex_buffer_data.paint_model){
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 1);
        
        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", cam.viewMatrix);

        ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::projectingPaintedTextureShader(), *vertex_buffer_data.model_mesh);

        //Draw the UV of the selected model
        {    
            if(wrapMode){
                glEnable(GL_BLEND);
                
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_MAX, GL_MAX);
            }

            vertex_buffer_data.model_mesh->Draw("Painting_UTIL : project_window_painting_texture");
            
            if(wrapMode){
                glDisable(GL_BLEND);

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            }
        }

        GL::releaseBoundTextures("painting : project_window_painting_texture");
    }

    // Painting the 2D scene 
    else{
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 0);

        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getContext()->ortho_projection);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        ShaderSystem::projectingPaintedTextureShader().setInt("usingMeshSelection", false);
        
        vertex_buffer_data.box->bindBuffers();
        
        ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", 2);
        getBox()->draw("Painting : Projecting painted texture (For 2D Scene)");
    }

    GL::releaseBoundTextures("paintingUTIL::project_window_painting_texture");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);

}


std::vector<glm::vec2> getCursorSubstitution(float spacing, glm::vec2 start, glm::vec2 dest)
{
    
    glm::vec2 cursorPos = start;
    
    std::vector<glm::vec2> holdLocations;
    
    glm::vec2 fstrokeLocation = glm::vec2(cursorPos);
    holdLocations.push_back(fstrokeLocation);
    
    //----------------------PAINTING----------------------\\

    int differenceBetweenMousePoints = glm::distance(cursorPos, dest);
    if(spacing > 5)
        differenceBetweenMousePoints = 1; 

    float xposDif = (cursorPos.x - dest.x) / differenceBetweenMousePoints;
    float yposDif = (cursorPos.y - dest.y) / differenceBetweenMousePoints;

    for (size_t i = 0; i < differenceBetweenMousePoints; i++)
    {
        ////differenceBetweenMousePoints > 10
        if (true) {
            cursorPos.x -= xposDif;
            cursorPos.y -= yposDif;
        }
        if(true){
            glm::vec2 strokeLocation = cursorPos;
            holdLocations.push_back(strokeLocation);
        }
    }
    return holdLocations;
}
 
static glm::vec2 findPos(glm::ivec2 res, float* posData, float* pxs){
    for (size_t y = 0; y < res.y; y++)
    {
        for (size_t x = 0; x < res.x; x++){
            int index = (y * res.x + x) * 4;

            if(true){
                float tolerance = 0.005f;

                if(index + 3 >= res.x * res.y * 4)
                    return glm::vec2(-1.f);

                if(pxs[index + 3] != 0.f){
                    if(pxs[index] >= posData[0] - tolerance && pxs[index] <= posData[0] + tolerance){
                        if(pxs[index + 1] >= posData[1] - tolerance && pxs[index + 1] <= posData[1] + tolerance){
                            if(pxs[index + 2] >= posData[2] - tolerance && pxs[index + 2] <= posData[2] + tolerance){
                                
                                glm::vec2 crsPos = glm::vec2((float)getContext()->windowScale.x / ((float)res.x / (float)x), (float)getContext()->windowScale.y / ((float)res.y / (float)y));
                                
                                return crsPos;
                            }
                        }
                    }
                }
            }
        }
    }
    return glm::vec2(-1.f);
}

static Texture process3DPointTxtr;

static void process_3D_point_calculate_cam(Camera* cam, ThreeDPoint threeDPoint){
    cam->cameraPos = glm::vec3(
                                            threeDPoint.pos[0] + threeDPoint.normal[0] * 5.1f, 
                                            threeDPoint.pos[1] + threeDPoint.normal[1] * 5.f, 
                                            threeDPoint.pos[2] + threeDPoint.normal[2] * 5.f
                                        );

    cam->originPos = glm::vec3(
                                                threeDPoint.pos[0], 
                                                threeDPoint.pos[1], 
                                                threeDPoint.pos[2]
                                            );
    
    cam->updateViewMatrix(cam->cameraPos, cam->originPos);
}

static glm::vec2 process_3D_point_calculate_2D_location(Camera cam, ThreeDPoint threeDPoint, Mesh* mesh){

    const unsigned int resolution = 512;

    if(!process3DPointTxtr.ID){
        process3DPointTxtr = Texture((char*)nullptr, resolution, resolution);
    }

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(process3DPointTxtr, glm::ivec2(resolution), "process_3D_point_calculate_2D_location");

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    ShaderSystem::renderModelData().use();
    ShaderSystem::renderModelData().setMat4("view", cam.viewMatrix);
    ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::renderModelData().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::renderModelData().setInt("state", 1);

    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::renderModelData(), *mesh);
    
    mesh->Draw("process_3D_point_calculate_2D_location");

    GL::releaseBoundTextures("process_3D_point_calculate_2D_location");
    ShaderUTIL::release_bound_shader();

    float* pxs = new float[resolution * resolution * 4]; 
    
    glReadPixels(
                    0, 
                    0, 
                    resolution, 
                    resolution,
                    GL_RGBA,
                    GL_FLOAT,
                    pxs
                );
    
    float posData[4];
    
    posData[0] = threeDPoint.pos.x;
    posData[1] = threeDPoint.pos.y;
    posData[2] = threeDPoint.pos.z;
    posData[3] = 1.f;

    posData[0] = (posData[0] + 1.f) / 2.f;
    posData[1] = (posData[1] + 1.f) / 2.f;
    posData[2] = (posData[2] + 1.f) / 2.f;

    glm::vec2 res = findPos(glm::ivec2(resolution), posData, pxs);

    delete[] pxs;

    FBOPOOL::releaseFBO(FBO);

    return res;
}

static void process_3D_point(      
                            ThreeDPoint threeDPoint, 
                            ThreeDPoint threeDPoint2, 
                            MirrorSide* mirrorSide,

                            // Return params
                            std::vector<glm::vec2>* strokes,
                            Mesh* mesh, 

                            // Dynamic Variables
                            bool first_stroke,
                            float spacing
                        )
{
    if(first_stroke){
        process_3D_point_calculate_cam(&mirrorSide->threeD_cam, threeDPoint);
    }

    glm::vec2 crs_pos = process_3D_point_calculate_2D_location(mirrorSide->threeD_cam, threeDPoint, mesh);
    glm::vec2 crs_pos_2 = process_3D_point_calculate_2D_location(mirrorSide->threeD_cam, threeDPoint2, mesh);
    crs_pos_2.y = getContext()->windowScale.y - crs_pos_2.y;
    crs_pos.y = getContext()->windowScale.y - crs_pos.y;

    if(crs_pos != glm::vec2(-1.f) && crs_pos_2 != glm::vec2(-1.f)){
        *strokes = getCursorSubstitution(spacing, crs_pos, crs_pos_2);
    }
    else if(crs_pos != glm::vec2(-1.f))
        strokes->push_back(crs_pos);

    ShaderSystem::twoDPainting().use();
}

glm::vec2 last_point = glm::vec2(0.f);

void process_2D_point(      
                            glm::vec2 point, 
    
                            // Return params
                            std::vector<glm::vec2>* strokes, 

                            // Dynamic Variables
                            bool first_stroke,
                            float spacing
                        )
{
    if(first_stroke)
        last_point = point;

    *strokes = getCursorSubstitution(spacing, point, last_point);

    last_point = point;
}

void update_depth_texture(Texture depth_texture, Camera cam, Mesh* mesh){

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(depth_texture, depth_texture.getResolution(),"update_depth_texture");
    
    //Clear the depth texture
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LESS);
    
    //Use the depth 3D shader
    ShaderSystem::depth3D().use();
    ShaderSystem::depth3D().setMat4("view", cam.viewMatrix);
    ShaderSystem::depth3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::depth3D().setMat4("modelMatrix", getScene()->transformMatrix);
    
    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::depth3D(), *mesh);
    
    mesh->Draw("update_depth_texture");

    //!Finished
    GL::releaseBoundTextures("paintingUTIL : update_depth_texture");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);


    glDepthFunc(GL_LEQUAL);
} 

void generate_projected_painting_texture(Texture* txtr, bool mirror_X, bool mirror_Y, bool mirror_Z, bool use_low_resolution_buffers){
        
    if(mirror_X || mirror_Y || mirror_Z){
        // Generate and bind the capturing framebuffer
        Framebuffer FBO = FBOPOOL::requestFBO(*txtr, "generate_projected_painting_texture");
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, txtr->getResolution().x, txtr->getResolution().y);

        ShaderSystem::projectedPaintingTextureMixerShader().use();

        ShaderSystem::projectedPaintingTextureMixerShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::projectedPaintingTextureMixerShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::projectedPaintingTextureMixerShader().setVec2("scale", glm::vec2(0.5f));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("redChannel", false);
        
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr1", GL::bindTexture_2D((use_low_resolution_buffers) ? O_side.paintingBuffers.projected_painting_texture_low.ID : O_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));
        
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2Active", mirror_X);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2", GL::bindTexture_2D((use_low_resolution_buffers) ? X_side.paintingBuffers.projected_painting_texture_low.ID : X_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3Active", mirror_Y);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3", GL::bindTexture_2D((use_low_resolution_buffers) ? Y_side.paintingBuffers.projected_painting_texture_low.ID : Y_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4Active", mirror_X && mirror_Y);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4", GL::bindTexture_2D((use_low_resolution_buffers) ? XY_side.paintingBuffers.projected_painting_texture_low.ID : XY_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5Active", mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5", GL::bindTexture_2D((use_low_resolution_buffers) ? Z_side.paintingBuffers.projected_painting_texture_low.ID : Z_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6Active", mirror_X && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6", GL::bindTexture_2D((use_low_resolution_buffers) ? XZ_side.paintingBuffers.projected_painting_texture_low.ID : XZ_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7Active", mirror_Y && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7", GL::bindTexture_2D((use_low_resolution_buffers) ? YZ_side.paintingBuffers.projected_painting_texture_low.ID : YZ_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8Active", mirror_X && mirror_Y && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8", GL::bindTexture_2D((use_low_resolution_buffers) ? XYZ_side.paintingBuffers.projected_painting_texture_low.ID : XYZ_side.paintingBuffers.projected_painting_texture.ID, "PaintingUTIL : generate_projected_painting_texture"));

        getBox()->draw("Painter::generateMirroredProjectedPaintingTexture : Render result");

        GL::releaseBoundTextures("PaintingUTIL : generate_projected_painting_texture");
        ShaderUTIL::release_bound_shader();
        FBOPOOL::releaseFBO(FBO);
    }
    else{
        if(use_low_resolution_buffers)
            O_side.paintingBuffers.projected_painting_texture_low.duplicateTexture(*txtr, "generate_projected_painting_texture");
        else
            O_side.paintingBuffers.projected_painting_texture.duplicateTexture(*txtr, "generate_projected_painting_texture");
    }
}

Camera twoD_painting_calculate_camera(PaintSettings settings, MirrorSide mirrorSide){
    glm::vec3 offset = glm::vec3(
                                    settings.mirror_settings.X_offset,
                                    settings.mirror_settings.Y_offset,
                                    settings.mirror_settings.Z_offset
                                );

    offset *= glm::max(mirrorSide.effectAxis, 0.f);

    glm::vec3 orgCamPos = getScene()->camera.cameraPos;
    glm::vec3 orgOriginPos = getScene()->camera.originPos;
    glm::vec3 camPosOriginDistance = orgCamPos - orgOriginPos;
    glm::vec3 camPos = orgOriginPos * -mirrorSide.effectAxis - camPosOriginDistance * mirrorSide.effectAxis - offset * 2.f; 

    Camera cam;
    cam.cameraPos = camPos;
    cam.originPos = orgOriginPos * -mirrorSide.effectAxis - offset * 2.f;
    cam.updateViewMatrix(cam.cameraPos, cam.originPos);

    return cam;
}

void twoD_painting_generate_mirrored_window_painting_texture(MirrorSide* mirrorSide){
    O_side.paintingBuffers.window_painting_texture.duplicateTexture("twoD_painting_generate_mirrored_window_painting_texture");

    bool horizontal = false;
    bool vertical = false;

    glm::vec3 cam = glm::abs(glm::normalize(getScene()->camera.cameraPos));

    //In the X axis
    if(cam.x > 0.5){
        float invertVal = 1.f;
        if(
            mirrorSide->effectAxis == X_side.effectAxis || 
            mirrorSide->effectAxis == XY_side.effectAxis || 
            mirrorSide->effectAxis == XZ_side.effectAxis || 
            mirrorSide->effectAxis == XYZ_side.effectAxis
        )
            invertVal = -1.f;
        
        horizontal = std::max(mirrorSide->effectAxis.z * invertVal, 0.f);
        vertical = std::max(mirrorSide->effectAxis.y, 0.f);
    }
    
    
    //In the Z axis
    else{
        float invertVal = 1.f;
        if(
            mirrorSide->effectAxis == Z_side.effectAxis || 
            mirrorSide->effectAxis == XZ_side.effectAxis || 
            mirrorSide->effectAxis == YZ_side.effectAxis || 
            mirrorSide->effectAxis == XYZ_side.effectAxis
        )
            invertVal = -1.f;
        
        horizontal = std::max(mirrorSide->effectAxis.x * invertVal, 0.f);
        vertical = std::max(mirrorSide->effectAxis.y, 0.f);
    }

    if(horizontal || vertical)
        mirrorSide->paintingBuffers.window_painting_texture.flipTexture(horizontal, vertical);
}

void bucket_paint_texture(Texture texture, Color color, float opacity, Mesh* mesh){
    char whitePx[4] = {
                        color.getRGB_normalized().r * 127,
                        color.getRGB_normalized().g * 127,
                        color.getRGB_normalized().b * 127,
                        opacity * 127
                    };

    glm::ivec2 res = texture.getResolution();
    char* pxs = new char[res.x * res.y * 4];

    for (size_t y = 0; y < res.y; y++)
    {
        for (size_t x = 0; x < res.x; x++){
            int index = (y * res.x + x) * 4;
        
            if(index + 3 < res.x * res.y * 4){
                pxs[index + 0] = whitePx[0]; 
                pxs[index + 1] = whitePx[1]; 
                pxs[index + 2] = whitePx[2]; 
                pxs[index + 3] = whitePx[3]; 
            }
        }
    }

    texture.update(pxs, res.x, res.y, GL_LINEAR);

    if(mesh != nullptr)
        texture.removeUnselectedFaces(*mesh);

    delete[] pxs;
}

static MaterialChannels custom_mat_result_channels;
static Material prevCustomMaterial;
static int prevMeshVBO = 0;
static size_t prevMeshIndicesSize = 0;
static void update_custom_material_mesh(PaintSettings::ColorBuffer color_buffer, Mesh* mesh, glm::ivec2 resolution){
    
    LigidWindow* bound_context = LigidGL::getBoundContext();
    
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : update_custom_material_mesh : No valid bound context" << LGDLOG::end;
        return;
    }
    
    if(prevCustomMaterial != color_buffer.material || prevMeshVBO != mesh->vertex_buffers[bound_context].VBO || prevMeshIndicesSize != mesh->indices.size()){
        if(!custom_mat_result_channels.albedo.ID){
            custom_mat_result_channels.albedo = Texture((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.roughness = Texture((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.metallic = Texture((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.normalMap = Texture((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.heightMap = Texture((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.ambientOcclusion = Texture((char*)nullptr, resolution.x, resolution.y);
        }
        else{
            custom_mat_result_channels.albedo.update((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.roughness.update((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.metallic.update((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.normalMap.update((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.heightMap.update((char*)nullptr, resolution.x, resolution.y);
            custom_mat_result_channels.ambientOcclusion.update((char*)nullptr, resolution.x, resolution.y);
        }


        material_thread.apply_material(&color_buffer.material, getScene()->model, mesh, &custom_mat_result_channels, resolution.x);

        WAIT_WHILE(material_thread.actions.size());
    }

    prevCustomMaterial = color_buffer.material;
    prevMeshVBO = mesh->vertex_buffers[bound_context].VBO;
    prevMeshIndicesSize = mesh->indices.size();
}

static void captureTxtrToSourceTxtr(Texture &captureTexture, Texture &selectedTexture){
    unsigned char* pxs = new unsigned char[captureTexture.getResolution().x * captureTexture.getResolution().y * 4]; 
    captureTexture.getData(pxs);

    selectedTexture.update(pxs, captureTexture.getResolution().x, captureTexture.getResolution().y);

    delete[] pxs; //Remove the capture texture's pixels out of the memory

    GL::releaseBoundTextures("captureTxtrToSourceTxtr");
}

static Texture captureTexture;
static void updateTheTexture(
                                Texture txtr, 
                                int channelI,
                                PaintSettings settings,
                                Texture projected_painting_txtr
                            )
{
    glm::vec2 destScale = glm::vec2(txtr.getResolution());

    glActiveTexture(GL_TEXTURE0);

    if(!captureTexture.ID){
        captureTexture = Texture((char*)nullptr, destScale.x, destScale.y, GL_LINEAR);
    }
    else{
        captureTexture.update((char*)nullptr, destScale.x, destScale.y, GL_LINEAR);
    }
    
    Framebuffer FBO = FBOPOOL::requestFBO(captureTexture, "Painter::updateTheTexture");

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    GL::bindTexture_2D(txtr.ID, "paintingUTIL : updateTheTexture");

    //Set the viewport to the resolution of the texture
    glViewport(0,0,destScale.x,destScale.y);
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", glm::ortho(0.f, destScale.x, destScale.y, 0.f));
    ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(destScale.x  / 2.f, destScale.y / 2.f, 0.1));
    ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(destScale / 2.f));
    ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(txtr.ID, "Scene::render_skybox : Render bg texture"));
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);
    
    getBox()->draw("Painter::updateTheTexture : Rendering the original texture (background)");

    ShaderSystem::textureUpdatingShader().use();
    ShaderSystem::textureUpdatingShader().setInt("channel_index", channelI);

    //*Fragment
    ShaderUTIL::set_shader_struct_painting_data(
                                                    ShaderSystem::textureUpdatingShader(), 
                                                    ShaderUTIL::PaintingData(
                                                                                ShaderUTIL::PaintingData::PaintingBuffers(
                                                                                                                            txtr,
                                                                                                                            txtr,
                                                                                                                            txtr,
                                                                                                                            txtr,
                                                                                                                            txtr,
                                                                                                                            txtr,
                                                                                                                            projected_painting_txtr
                                                                                                                        ),   
                                                                                ShaderUTIL::PaintingData::PaintingSmearData(
                                                                                                                                settings.smear_mode.smear_transform_strength,
                                                                                                                                settings.smear_mode.smear_blur_strength
                                                                                                                            ),   
                                                                                ShaderUTIL::PaintingData::ChannelData(
                                                                                                                        settings.color_buffer.stroke_roughness_color,
                                                                                                                        settings.color_buffer.stroke_metallic_color,
                                                                                                                        settings.color_buffer.stroke_normalMap_color,
                                                                                                                        settings.color_buffer.stroke_heightMap_color,
                                                                                                                        settings.color_buffer.stroke_ao_color
                                                                                                                    ),
                                                                                settings.painting_mode,
                                                                                settings.painting_over_data.active                            
                                                                            )
                                                );
    
    //*Vertex
    ShaderSystem::textureUpdatingShader().setMat4("orthoProjection", orthoProjection);
    ShaderSystem::textureUpdatingShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
    ShaderSystem::textureUpdatingShader().setMat4("view", getScene()->camera.viewMatrix);

    if(settings.vertex_buffer.paint_model){
        //Draw the UV of the selected model
        settings.vertex_buffer.model_mesh->Draw("updateTheTexture");         

        FBOPOOL::releaseFBO(FBO);
        
        captureTexture.removeSeams(*settings.vertex_buffer.model_mesh);
    }
    else{
        settings.vertex_buffer.box->bindBuffers();
        getBox()->draw("Painter::updateTheTexture : Applying painting to the texture");
        
        FBOPOOL::releaseFBO(FBO);
    }
    
    GL::releaseBoundTextures("paintingUTIL : updateTheTexture");
    ShaderUTIL::release_bound_shader();

    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture, txtr);
}

struct PaintedBufferData{
    glm::vec3 clr;
    Texture txtr;
    Texture corresponding_custom_material_channel;
    int channel_index = 0;
};

static std::vector<PaintedBufferData> get_painted_buffers(PaintSettings settings){
    std::vector<PaintedBufferData> result;
    
    if(settings.painted_buffers.material_painting){

        for (size_t i = 0; i < 6; i++)
        {
            PaintedBufferData data;
            if(i == 0 && settings.painted_buffers.material_channel_albedo_active){
                data.clr = settings.color_buffer.stroke_albedo_color.getRGB_normalized();
                data.txtr = settings.painted_buffers.material_channel_albedo;
                data.corresponding_custom_material_channel = custom_mat_result_channels.albedo;
            }
            if(i == 1 && settings.painted_buffers.material_channel_roughness_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_roughness_color);
                data.txtr = settings.painted_buffers.material_channel_roughness;
                data.corresponding_custom_material_channel = custom_mat_result_channels.roughness;
            }
            if(i == 2 && settings.painted_buffers.material_channel_metallic_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_metallic_color);
                data.txtr = settings.painted_buffers.material_channel_metallic;
                data.corresponding_custom_material_channel = custom_mat_result_channels.metallic;
            }
            if(i == 3 && settings.painted_buffers.material_channel_normalMap_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_normalMap_color);
                data.txtr = settings.painted_buffers.material_channel_normalMap;
                data.corresponding_custom_material_channel = custom_mat_result_channels.normalMap;
            }
            if(i == 4 && settings.painted_buffers.material_channel_heightMap_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_heightMap_color);
                data.txtr = settings.painted_buffers.material_channel_heightMap;
                data.corresponding_custom_material_channel = custom_mat_result_channels.heightMap;
            }
            if(i == 5 && settings.painted_buffers.material_channel_ao_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_ao_color);
                data.txtr = settings.painted_buffers.material_channel_ao;
                data.corresponding_custom_material_channel = custom_mat_result_channels.ambientOcclusion;
            }

            data.channel_index = i;
            result.push_back(data);
        }
    }
    else{
        PaintedBufferData data;
        data.clr = glm::vec3(settings.color_buffer.stroke_albedo_color.getRGB_normalized());
        data.txtr = settings.painted_buffers.solid_painted_texture;
        data.corresponding_custom_material_channel = custom_mat_result_channels.albedo;
        data.channel_index = 0;
        result.push_back(data);
    }

    return result;
}

#define REFRESH_BUFFER(buffer) {Framebuffer FBO = FBOPOOL::requestFBO(buffer, "Refresh buffers fbo"); glClearColor(0,0,0,0); glClear(GL_COLOR_BUFFER_BIT); FBOPOOL::releaseFBO(FBO);} 
static void refresh_buffers(Texture* projected_painting_txtr){

    REFRESH_BUFFER(*projected_painting_txtr)

    REFRESH_BUFFER(O_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(O_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(O_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(O_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(X_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(X_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(X_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(X_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(Y_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(Y_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(Y_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(Y_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(XY_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(XY_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(XY_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(XY_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(Z_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(Z_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(Z_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(Z_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(XZ_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(XZ_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(XZ_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(XZ_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(YZ_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(YZ_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(YZ_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(YZ_side.paintingBuffers.window_painting_texture)
    
    REFRESH_BUFFER(XYZ_side.paintingBuffers.depth_texture)
    REFRESH_BUFFER(XYZ_side.paintingBuffers.projected_painting_texture)
    REFRESH_BUFFER(XYZ_side.paintingBuffers.projected_painting_texture_low)
    REFRESH_BUFFER(XYZ_side.paintingBuffers.window_painting_texture)
    
}