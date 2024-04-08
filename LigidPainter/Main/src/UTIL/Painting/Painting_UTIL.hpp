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
    if(brushProperties.brushTexture.ID){
        ShaderSystem::twoDPainting().setFloat("brush.txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, brushProperties.brushTexture.ID);
    }
    else{
        ShaderSystem::twoDPainting().setFloat("brush.txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, appTextures.white.ID);   
    }
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

static Texture painting_BG_texture;
static Framebuffer window_paint_FBO;
static void window_paint(Texture* window_paint_texture, std::vector<glm::vec2> strokes, Brush brush, int frame_count, bool paint_offset_value){
    if(!window_paint_FBO.ID){
        window_paint_FBO = Framebuffer(*window_paint_texture, GL_TEXTURE_2D, "window_paint");
        painting_BG_texture = Texture(nullptr, 1, 1);
    }
    else{
        window_paint_FBO.setColorBuffer(*window_paint_texture, GL_TEXTURE_2D);
    }
    
    //Bind the painting texture to the painting framebuffer
    window_paint_FBO.colorBuffer.duplicateTexture(painting_BG_texture);

    window_paint_FBO.bind();

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
    ShaderSystem::twoDPainting().setInt("bgTxtr", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, painting_BG_texture.ID);
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
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    //Painting
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::doPaint : Rendering 2D painted");
    
    //Finish
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);
}

static Framebuffer project_window_painting_texture_FBO;
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
    // Create FBO
    if(!project_window_painting_texture_FBO.ID){
        project_window_painting_texture_FBO = Framebuffer(*projected_painting_texture, GL_TEXTURE_2D, get_renderbuffer_for_texture(*projected_painting_texture), "project_window_painting_texture_FBO");
    }

    // Update color buffer
    else{
        project_window_painting_texture_FBO.setColorBuffer(*projected_painting_texture, GL_TEXTURE_2D);
    }

    // Update renderbuffer
    if(projected_painting_texture->getResolution() != project_window_painting_texture_FBO.renderBuffer.getResolution()){
        project_window_painting_texture_FBO.setRenderbuffer(get_renderbuffer_for_texture(*projected_painting_texture));
    }

    glDisable(GL_BLEND);

    project_window_painting_texture_FBO.bind();
    
    glClearColor(0, 0, 0, 0);
    
    if(wrapMode)
        glClear(GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, projected_painting_texture->getResolution().x, projected_painting_texture->getResolution().y);

    // Bind the related shader program
    ShaderSystem::projectingPaintedTextureShader().use();

    //*Fragment
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingTexture", 6); glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, window_painting_texture.ID);
    ShaderSystem::projectingPaintedTextureShader().setInt("depthTexture", 7); glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, depth_texture.ID);
    ShaderSystem::projectingPaintedTextureShader().setFloat("paintingOpacity", brush_opacity);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPaintingModeIndex", painting_mode);
    
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverTexture", 9); glActiveTexture(GL_TEXTURE9); glBindTexture(GL_TEXTURE_2D, painting_over_data.texture_field_scene->painting_over_texture.ID);
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

        ShaderSystem::projectingPaintedTextureShader().setInt("usingMeshSelection", vertex_buffer_data.model_mesh->face_selection_data.activated);
        ShaderSystem::projectingPaintedTextureShader().setInt("selectedPrimitiveIDS", 8); glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_2D, vertex_buffer_data.model_mesh->face_selection_data.selectedFaces.ID);
        ShaderSystem::projectingPaintedTextureShader().setInt("meshMask", 10); glActiveTexture(GL_TEXTURE10); glBindTexture(GL_TEXTURE_2D, vertex_buffer_data.model_mesh->face_selection_data.meshMask.ID);

        //Draw the UV of the selected model
        {    
            if(wrapMode){
                glEnable(GL_BLEND);
                
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_MAX, GL_MAX);
            }

            ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", vertex_buffer_data.model_mesh->indices.size() / 3);
            vertex_buffer_data.model_mesh->Draw(false);
            
            if(wrapMode){
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                
                glDisable(GL_BLEND);
            }
        }
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
        LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Painting : Projecting painted texture (For 2D Scene)");
    }

    glEnable(GL_BLEND);
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

static Framebuffer process3DPointFBO;

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

static glm::vec2 process_3D_point_calculate_2D_location(Camera cam, ThreeDPoint threeDPoint, const unsigned int resolution, Mesh* mesh){
    process3DPointFBO.bind();

    glViewport(0, 0, resolution, resolution);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    ShaderSystem::renderModelData().use();
    ShaderSystem::renderModelData().setMat4("view", cam.viewMatrix);
    ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::renderModelData().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::renderModelData().setInt("state", 1);

    ShaderSystem::renderModelData().setInt("usingMeshSelection", mesh->face_selection_data.activated);
    ShaderSystem::renderModelData().setInt("hideUnselected", mesh->face_selection_data.hideUnselected);
    ShaderSystem::renderModelData().setInt("selectedPrimitiveIDS", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, mesh->face_selection_data.selectedFaces.ID);
    ShaderSystem::renderModelData().setInt("meshMask", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, mesh->face_selection_data.meshMask.ID);

    //Draw the selected mesh in 3D
    ShaderSystem::renderModelData().setInt("primitiveCount", mesh->indices.size() / 3);
    mesh->Draw(false);

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

    return res;
}

static void process_3D_point(      
                            ThreeDPoint threeDPoint, 
                            ThreeDPoint threeDPoint2, 
                            MirrorSide mirrorSide,

                            // Return params
                            Camera* cam, 
                            std::vector<glm::vec2>* strokes,
                            Mesh* mesh, 

                            // Dynamic Variables
                            bool first_stroke,
                            float spacing
                        )
{
    process_3D_point_calculate_cam(cam, threeDPoint);
    
    const unsigned int resolution = 512; 
    if(!process3DPointFBO.ID){
        process3DPointFBO = Framebuffer(Texture(nullptr, resolution, resolution), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "process3DPointFBO");
    }

    glm::vec2 crs_pos = process_3D_point_calculate_2D_location(*cam, threeDPoint, resolution, mesh);
    ThreeDPoint center;
    center.pos = (threeDPoint.pos + threeDPoint2.pos) / 2.f;
    center.normal = (threeDPoint.normal + threeDPoint2.normal) / 2.f;
    Camera centerCam;
    process_3D_point_calculate_cam(&centerCam, center);
    glm::vec2 crs_pos_2 = process_3D_point_calculate_2D_location(*cam, threeDPoint2, resolution, mesh);

    if(crs_pos != glm::vec2(-1.f) && crs_pos_2 != glm::vec2(-1.f)){
        crs_pos_2.y = getContext()->windowScale.y - crs_pos_2.y;
        *strokes = getCursorSubstitution(spacing, crs_pos, crs_pos_2);
    }
    else if(crs_pos != glm::vec2(-1.f))
        strokes->push_back(crs_pos);

    getBox()->bindBuffers();
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

static Framebuffer update_depth_texture_FBO;
void update_depth_texture(Texture depth_texture, Camera cam, Mesh* mesh){
    // Create FBO
    if(!update_depth_texture_FBO.ID){
        update_depth_texture_FBO = Framebuffer(depth_texture, GL_TEXTURE_2D, get_renderbuffer_for_texture(depth_texture), "update_depth_texture_FBO");
    }

    // Update color buffer
    else{
        update_depth_texture_FBO.setColorBuffer(depth_texture, GL_TEXTURE_2D);
    }

    // Update renderbuffer
    if(depth_texture.getResolution() != update_depth_texture_FBO.renderBuffer.getResolution()){
        update_depth_texture_FBO.setRenderbuffer(get_renderbuffer_for_texture(depth_texture));
    }
    
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    //Clear the depth texture
    glViewport(0, 0, depth_texture.getResolution().x, depth_texture.getResolution().y);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    ShaderSystem::depth3D().use();
    ShaderSystem::depth3D().setMat4("view", cam.viewMatrix);
    ShaderSystem::depth3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::depth3D().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::depth3D().setInt("usingMeshSelection", mesh->face_selection_data.activated);
    ShaderSystem::depth3D().setInt("hideUnselected", mesh->face_selection_data.hideUnselected);
    ShaderSystem::depth3D().setInt("selectedPrimitiveIDS", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, mesh->face_selection_data.selectedFaces.ID);
    ShaderSystem::depth3D().setInt("meshMask", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, mesh->face_selection_data.meshMask.ID);

    //Draw the selected mesh in 3D
    ShaderSystem::depth3D().setInt("primitiveCount", mesh->indices.size() / 3);
    mesh->Draw(false);

    //!Finished
    //Set back to default shader
    ShaderSystem::buttonShader().use();

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
} 

void generate_projected_painting_texture(Framebuffer* FBO, bool mirror_X, bool mirror_Y, bool mirror_Z, bool use_low_resolution_buffers){
    getBox()->bindBuffers();
        
    if(mirror_X || mirror_Y || mirror_Z){
        // Generate and bind the capturing framebuffer
        FBO->bind();
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, FBO->colorBuffer.getResolution().x, FBO->colorBuffer.getResolution().y);

        ShaderSystem::projectedPaintingTextureMixerShader().use();

        ShaderSystem::projectedPaintingTextureMixerShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::projectedPaintingTextureMixerShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::projectedPaintingTextureMixerShader().setVec2("scale", glm::vec2(0.5f));

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("redChannel", false);
        
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr1", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? O_side.paintingBuffers.projected_painting_texture_low.ID : O_side.paintingBuffers.projected_painting_texture.ID);
        
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2Active", mirror_X);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? X_side.paintingBuffers.projected_painting_texture_low.ID : X_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3Active", mirror_Y);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? Y_side.paintingBuffers.projected_painting_texture_low.ID : Y_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4Active", mirror_X && mirror_Y);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4", 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? XY_side.paintingBuffers.projected_painting_texture_low.ID : XY_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5Active", mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5", 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? Z_side.paintingBuffers.projected_painting_texture_low.ID : Z_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6Active", mirror_X && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6", 6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? XZ_side.paintingBuffers.projected_painting_texture_low.ID : XZ_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7Active", mirror_Y && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7", 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? YZ_side.paintingBuffers.projected_painting_texture_low.ID : YZ_side.paintingBuffers.projected_painting_texture.ID);

        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8Active", mirror_X && mirror_Y && mirror_Z);
        ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8", 8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, (use_low_resolution_buffers) ? XYZ_side.paintingBuffers.projected_painting_texture_low.ID : XYZ_side.paintingBuffers.projected_painting_texture.ID);

        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::generateMirroredProjectedPaintingTexture : Render result");
    }
    else{
        if(use_low_resolution_buffers)
            O_side.paintingBuffers.projected_painting_texture_low.duplicateTexture(FBO->colorBuffer);
        else
            O_side.paintingBuffers.projected_painting_texture.duplicateTexture(FBO->colorBuffer);
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
    O_side.paintingBuffers.window_painting_texture.duplicateTexture(mirrorSide->paintingBuffers.window_painting_texture);

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

void bucket_paint_texture(Texture texture, Color color, float opacity){
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

    delete[] pxs;
}

void register_history_actions(int painting_mode, PaintSettings::PaintedBuffers painted_buffers){
    std::string actionTitle = "Unknown painting mode";
    
    if(painting_mode == 0)
        actionTitle = "Painting";
    if(painting_mode == 1)
        actionTitle = "Softening";
    if(painting_mode == 2)
        actionTitle = "Smearing";
    if(painting_mode == 3)
        actionTitle = "Normal map painting";
    if(painting_mode == 4)
        actionTitle = "Filter painting";
    if(painting_mode == 6)
        actionTitle = "Bucket painting";
    
    if(painted_buffers.material_painting){
        registerPaintingAction(
                                    "Multi-channel painting", 
                                    Texture(), 
                                    painted_buffers.material_channel_albedo, painted_buffers.material_channel_albedo_active, 
                                    painted_buffers.material_channel_roughness, painted_buffers.material_channel_roughness_active,
                                    painted_buffers.material_channel_metallic, painted_buffers.material_channel_metallic_active,
                                    painted_buffers.material_channel_normalMap, painted_buffers.material_channel_normalMap_active,
                                    painted_buffers.material_channel_heightMap, painted_buffers.material_channel_heightMap_active,
                                    painted_buffers.material_channel_ao, painted_buffers.material_channel_ao_active
                                );
    }
    else{
        registerPaintingAction(
                                    actionTitle, 
                                    Texture(), 
                                    painted_buffers.solid_painted_texture, true, 
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false
                                );
    }
}

static Mesh customMatMesh;
static Material prevCustomMaterial;
static int prevMeshVBO = 0;
static int prevMeshIndicesSize = 0;
static void update_custom_material_mesh(PaintSettings::ColorBuffer color_buffer, Mesh* mesh, glm::ivec2 resolution){
    if(prevCustomMaterial != color_buffer.material || prevMeshVBO != mesh->VBO || prevMeshIndicesSize != mesh->indices.size()){
        customMatMesh.EBO = mesh->EBO;
        customMatMesh.VBO = mesh->VBO;
        customMatMesh.VAO = mesh->VAO;
        customMatMesh.indices = mesh->indices;
        
        if(!customMatMesh.albedo.ID){
            customMatMesh.albedo = Texture(nullptr, resolution.x, resolution.y);
            customMatMesh.roughness = Texture(nullptr, resolution.x, resolution.y);
            customMatMesh.metallic = Texture(nullptr, resolution.x, resolution.y);
            customMatMesh.normalMap = Texture(nullptr, resolution.x, resolution.y);
            customMatMesh.heightMap = Texture(nullptr, resolution.x, resolution.y);
            customMatMesh.ambientOcclusion = Texture(nullptr, resolution.x, resolution.y);
        }
        else{
            customMatMesh.albedo.update(nullptr, resolution.x, resolution.y);
            customMatMesh.roughness.update(nullptr, resolution.x, resolution.y);
            customMatMesh.metallic.update(nullptr, resolution.x, resolution.y);
            customMatMesh.normalMap.update(nullptr, resolution.x, resolution.y);
            customMatMesh.heightMap.update(nullptr, resolution.x, resolution.y);
            customMatMesh.ambientOcclusion.update(nullptr, resolution.x, resolution.y);
        }

        color_buffer.material.apply_material(*getScene()->model, customMatMesh, resolution.x, false);
    }

    prevCustomMaterial = color_buffer.material;
    prevMeshVBO = mesh->VBO;
    prevMeshIndicesSize = mesh->indices.size();
}

static void captureTxtrToSourceTxtr(unsigned int &captureTexture, glm::ivec2 textureRes, unsigned int &selectedTextureID){
    //Bind the capture texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, captureTexture);
    
    //Get the pixels of the capture texture
    char* pixels = new char[textureRes.x * textureRes.y * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    Texture selectedTextureOBJ = Texture(selectedTextureID);
    selectedTextureOBJ.update(pixels, textureRes.x, textureRes.y);

    delete[] pixels; //Remove the capture texture's pixels out of the memory
    glDeleteTextures(1, &captureTexture);
}

static void updateTheTexture(
                                Texture txtr, 
                                int channelI,
                                float channel_strength, 
                                PaintSettings settings ,
                                Framebuffer projected_painting_FBO
                            )
{
    glm::vec2 destScale = glm::vec2(txtr.getResolution());

    glActiveTexture(GL_TEXTURE0);

    Texture captureTexture = Texture(nullptr, destScale.x, destScale.y, GL_LINEAR);
    Framebuffer captureFBO = Framebuffer(captureTexture, GL_TEXTURE_2D, "Painter::updateTheTexture");
    
    captureFBO.bind();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);

    //Set the viewport to the resolution of the texture
    glViewport(0,0,destScale.x,destScale.y);
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);

    ShaderSystem::buttonShader().use();
    
    ShaderSystem::buttonShader().setMat4("projection", glm::ortho(0.f, destScale.x, destScale.y, 0.f));
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(destScale.x  / 2.f, destScale.y / 2.f, 0.1));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(destScale / 2.f));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::updateTheTexture : Rendering the original texture (background)");

    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );

    ShaderSystem::textureUpdatingShader().use();

    //*Fragment
    ShaderSystem::textureUpdatingShader().setInt("txtr", 5); glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, txtr.ID);
    ShaderSystem::textureUpdatingShader().setInt("paintingTexture", 6); glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, projected_painting_FBO.colorBuffer.ID);
    ShaderSystem::textureUpdatingShader().setInt("brushModeState", settings.painting_mode);
    ShaderSystem::textureUpdatingShader().setInt("usePaintingOver", settings.painting_over_data.active);
    ShaderSystem::textureUpdatingShader().setFloat("smearTransformStrength", settings.smear_mode.smear_transform_strength);
    ShaderSystem::textureUpdatingShader().setFloat("smearBlurStrength", settings.smear_mode.smear_blur_strength);
    ShaderSystem::textureUpdatingShader().setInt("multiChannelsPaintingMod", panel_displaying_modes.selectedElement == 1);
    ShaderSystem::textureUpdatingShader().setInt("channelI", channelI);
    ShaderSystem::textureUpdatingShader().setFloat("channelStrength", channel_strength);

    if(settings.vertex_buffer.paint_model){
        //Draw the UV of the selected model
        {
            //*Vertex
            ShaderSystem::textureUpdatingShader().setMat4("orthoProjection", orthoProjection);
            ShaderSystem::textureUpdatingShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
            ShaderSystem::textureUpdatingShader().setMat4("view", getScene()->camera.viewMatrix);

            settings.vertex_buffer.model_mesh->Draw(false);         
        }
    }
    else{
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 0);

        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getContext()->ortho_projection);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        settings.vertex_buffer.box->bindBuffers();
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::updateTheTexture : Applying painting to the texture");
    }
    
    if(!twoD_painting_mode){
        captureTexture.removeSeams(*getScene()->get_selected_mesh(), destScale);
    }

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, false);

    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture.ID, destScale, txtr.ID);
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
                data.corresponding_custom_material_channel = customMatMesh.albedo;
            }
            if(i == 1 && settings.painted_buffers.material_channel_roughness_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_roughness_color);
                data.txtr = settings.painted_buffers.material_channel_roughness;
                data.corresponding_custom_material_channel = customMatMesh.roughness;
            }
            if(i == 2 && settings.painted_buffers.material_channel_metallic_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_metallic_color);
                data.txtr = settings.painted_buffers.material_channel_metallic;
                data.corresponding_custom_material_channel = customMatMesh.metallic;
            }
            if(i == 3 && settings.painted_buffers.material_channel_normalMap_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_normalMap_color);
                data.txtr = settings.painted_buffers.material_channel_normalMap;
                data.corresponding_custom_material_channel = customMatMesh.normalMap;
            }
            if(i == 4 && settings.painted_buffers.material_channel_heightMap_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_heightMap_color);
                data.txtr = settings.painted_buffers.material_channel_heightMap;
                data.corresponding_custom_material_channel = customMatMesh.heightMap;
            }
            if(i == 5 && settings.painted_buffers.material_channel_ao_active){
                data.clr = glm::vec3(settings.color_buffer.stroke_ao_color);
                data.txtr = settings.painted_buffers.material_channel_ao;
                data.corresponding_custom_material_channel = customMatMesh.ambientOcclusion;
            }

            data.channel_index = i;
            result.push_back(data);
        }
    }
    else{
        PaintedBufferData data;
        data.clr = glm::vec3(settings.color_buffer.stroke_albedo_color.getRGB_normalized());
        data.txtr = settings.painted_buffers.solid_painted_texture;
        data.corresponding_custom_material_channel = customMatMesh.albedo;
        data.channel_index = 0;
        result.push_back(data);
    }

    return result;
}

Framebuffer refresh_FBO;
static void refresh_buffers(Framebuffer* projected_painting_FBO){
    if(!refresh_FBO.ID)
        refresh_FBO = Framebuffer(projected_painting_FBO->colorBuffer.ID, GL_TEXTURE_2D, "Refresh buffers fbo");
    
    refresh_FBO.bind();

    refresh_FBO.setColorBuffer(projected_painting_FBO->colorBuffer, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);

    refresh_FBO.setColorBuffer(O_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(O_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(O_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(O_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(X_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(X_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(X_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(X_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(Y_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Y_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Y_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Y_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(XY_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XY_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XY_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XY_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(Z_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Z_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Z_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(Z_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(XZ_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XZ_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XZ_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XZ_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(YZ_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(YZ_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(YZ_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(YZ_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    refresh_FBO.setColorBuffer(XYZ_side.paintingBuffers.depth_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XYZ_side.paintingBuffers.projected_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XYZ_side.paintingBuffers.projected_painting_texture_low, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    refresh_FBO.setColorBuffer(XYZ_side.paintingBuffers.window_painting_texture, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
}