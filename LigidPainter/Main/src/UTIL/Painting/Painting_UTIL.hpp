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
    ShaderSystem::twoDPainting().setFloat("brush.txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, brushProperties.brushTexture.ID);
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
                                            if(settings.painting_mode == 5)\
                                                data = {};

static Texture painting_BG_texture;
Framebuffer painting_act_1_FBO;

static void window_paint(Texture* window_paint_texture, std::vector<glm::vec2> strokes, Brush brush, int frame_count){
    //Bind the painting texture to the painting framebuffer
    painting_act_1_FBO.bind();
    prep painting_act_1_FBO;

    O_side.paintingBuffers.painting_texture.duplicateTexture(painting_BG_texture);

    float resolution = 0;

    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0, 0, resolution, resolution);
    
    glm::vec2 scale = glm::vec2(0.5f);
    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 1.f);
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
    ShaderSystem::twoDPainting().setInt("redChannelOnly", !(settings.painting_mode == 2));
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

static std::vector<MirrorSide*> get_selected_mirror_sides(bool mirror_X, bool mirror_Y, bool mirror_Z){
    std::vector<MirrorSide*> mirrorSides;
    
    bool mirror_XY = mirror_X && mirror_Y;     
    bool mirror_XZ = mirror_X && mirror_Z; 
    bool mirror_YZ = mirror_Y && mirror_Z; 
    bool mirror_XYZ = mirror_X && mirror_Y && mirror_Z; 
    
    if(true)
        mirrorSides.push_back(&O_side); // 0
    if(mirror_X)
        mirrorSides.push_back(&X_side); // 1
    if(mirror_Y)
        mirrorSides.push_back(&Y_side); // 2
    if(mirror_XY)
        mirrorSides.push_back(&XY_side); // 3
    if(mirror_Z)
        mirrorSides.push_back(&Z_side); // 4
    if(mirror_XZ)
        mirrorSides.push_back(&XZ_side); // 5
    if(mirror_YZ)
        mirrorSides.push_back(&YZ_side); // 6
    if(mirror_XYZ)
        mirrorSides.push_back(&XYZ_side); // 7

    return mirrorSides;
}

static void project_window_painting_texture(
                                                // Textures
                                                Texture* projected_painting_texture, 
                                                Texture window_painting_texture, 
                                                Texture depth_texture,
                                                
                                                Camera cam,
                                                Mesh* mesh,
                                                
                                                // Painting data
                                                int painting_mode, 
                                                float brush_opacity, 
                                                bool wrapMode
                                            )
{
    glDisable(GL_BLEND);

    glm::ivec2 projectedPaintingTextureRes = projectedPaintingTexture.getResolution();


    // Generate and bind the capturing framebuffer
    // TODO Don't create the render buffer there
    paintingFBO.setRenderbuffer(depthRBOcustom);
    paintingFBO.setColorBuffer(projectedPaintingTexture, GL_TEXTURE_2D);
    paintingFBO.bind();
    depthRBOcustom.update(GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, glm::ivec2(projectedPaintingTextureRes));
    
    glClearColor(0, 0, 0, 0);
    
    if(wrapMode)
        glClear(GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0,0,projectedPaintingTextureRes.x,projectedPaintingTextureRes.y);

    // Bind the related shader program
    ShaderSystem::projectingPaintedTextureShader().use();

    //*Fragment
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingTexture", 6);
    ShaderSystem::projectingPaintedTextureShader().setInt("depthTexture", 7);
    ShaderSystem::projectingPaintedTextureShader().setFloat("paintingOpacity", brush_opacity);
    ShaderSystem::projectingPaintedTextureShader().setInt("usingMeshSelection", faceSelectionActive);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPrimitiveIDS", 8);
    ShaderSystem::projectingPaintedTextureShader().setInt("meshMask", 10);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPaintingModeIndex", selectedPaintingModeIndex);
    
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverTexture", 9);
    ShaderSystem::projectingPaintedTextureShader().setInt("usePaintingOver", this->usePaintingOver);
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverGrayScale", checkComboList_painting_over.panel.sections[0].elements[4].checkBox.clickState1);
    ShaderSystem::projectingPaintedTextureShader().setVec3("paintingColor", checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.getSelectedColor().getRGB_normalized());
    
    // Bind the painting texture
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, window_painting_texture.ID);
    
    // Bind the depth texture for the depth testing
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, depth_texture.ID);
    
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, selectedPrimitives.ID);

    // Bind the painting over texture
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, getTextureFieldScene()->painting_over_texture.ID);
    
    // Bind the mesh mask texture
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);
    
    // Painting a 3D model
    if(!twoD_painting_mode){
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 1);
        
        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", cam.viewMatrix);
        
        //Draw the UV of the selected model
        {    
            if(wrapMode){
                glEnable(GL_BLEND);
                
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_MAX, GL_MAX);
            }

            ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", getScene()->get_selected_mesh()->indices.size() / 3);
            getScene()->get_selected_mesh()->Draw(false);
            
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
        //ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getScene().gui_projection);
        //ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        twoD_painting_box.bindBuffers();
        
        ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", 2);
        LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Painting : Projecting painted texture (For 2D Scene)");
    }

    this->paintingFBO.removeRenderbuffer();

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
 
void process_3D_point(      
                            ThreeDPoint threeDPoint, 
    
                            // Return params
                            Camera* cam, 
                            std::vector<glm::vec2>* strokes, 

                            // Dynamic Variables
                            bool first_stroke
                        )
{
    float* posData = new float[4]; 
    float* normalData = new float[4]; 
    
    posData[0] = threeDPoint.pos.x;
    posData[1] = threeDPoint.pos.y;
    posData[2] = threeDPoint.pos.z;
    posData[3] = 1.f;
    
    normalData[0] = threeDPoint.normal.x;
    normalData[1] = threeDPoint.normal.y;
    normalData[2] = threeDPoint.normal.z;
    normalData[3] = 1.f;

    glm::vec3 oldCamPos = getScene()->camera.cameraPos;
    glm::vec3 oldCamOrigin = getScene()->camera.originPos;
    
    cam->cameraPos = glm::vec3(
                                                posData[0] + normalData[0] * 5.1f, 
                                                posData[1] + normalData[1] * 5.f, 
                                                posData[2] + normalData[2] * 5.f
                                            );

    cam->originPos = glm::vec3(
                                                posData[0], 
                                                posData[1], 
                                                posData[2]
                                            );
    
    cam->updateViewMatrix(cam->cameraPos, cam->originPos);

    posData[0] = (posData[0] + 1.f) / 2.f;
    posData[1] = (posData[1] + 1.f) / 2.f;
    posData[2] = (posData[2] + 1.f) / 2.f;
    
    const unsigned int resolution = 512; 

    glm::ivec2 res = glm::ivec2(resolution);
    res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

    if(!posTxtr.ID)
        posTxtr = Texture(nullptr, resolution, resolution);

    paintingFBO.setRenderbuffer(depthRBO512);
    paintingFBO.setColorBuffer(posTxtr, GL_TEXTURE_2D);
    paintingFBO.bind();

    //Clear the depth texture
    glViewport(0, 0, (float)getContext()->windowScale.x / ((float)Settings::videoScale()->x / (float)res.x), (float)getContext()->windowScale.y / ((float)Settings::videoScale()->y / (float)res.y));
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    ShaderSystem::renderModelData().use();
    ShaderSystem::renderModelData().setMat4("view", cam->viewMatrix);
    ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::renderModelData().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::renderModelData().setInt("state", 1);

    ShaderSystem::renderModelData().setInt("usingMeshSelection", this->faceSelection.activated);
    ShaderSystem::renderModelData().setInt("hideUnselected", this->faceSelection.hideUnselected);
    ShaderSystem::renderModelData().setInt("selectedPrimitiveIDS", 0);
    ShaderSystem::renderModelData().setInt("meshMask", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.selectedFaces.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);

    //Draw the selected mesh in 3D
    ShaderSystem::renderModelData().setInt("primitiveCount", getScene()->get_selected_mesh()->indices.size() / 3);
    getScene()->get_selected_mesh()->Draw(false);

    float* pxs = new float[res.x * res.y * 4]; 
    
    glReadPixels(
                    0, 
                    0, 
                    res.x, 
                    res.y,
                    GL_RGBA,
                    GL_FLOAT,
                    pxs
                );



    float* lastPosP = new float[4]; 
    lastPosP[0] = lastPos.x;
    lastPosP[1] = lastPos.y;
    lastPosP[2] = lastPos.z;
    lastPosP[3] = lastPos.w;

    glm::vec2 crsPos = findPos(res, posData, pxs);
    glm::vec2 lastCrsPos = findPos(res, lastPosP, pxs);
    
    if(crsPos != glm::vec2(-1.f) && lastCrsPos != glm::vec2(-1.f)){
        glm::vec2 lastDest;
        lastDest.x = crsPos.x - (crsPos.x - lastCrsPos.x);
        lastDest.y = crsPos.y + (crsPos.y - lastCrsPos.y);
        
        if(first_stroke)  
            lastDest = crsPos;

        *strokes = getCursorSubstitution(this->brushProperties.spacing, crsPos, lastDest);
    }
    else if(crsPos != glm::vec2(-1.f))
        strokes->push_back(crsPos);

    //std::cout << posData[0] << ", " << posData[1] << ", " << posData[2] << ", " << posData[3] << std::endl;
    //std::cout << lastPosP[0] << ", " << lastPosP[1] << ", " << lastPosP[2] << ", " << lastPosP[3] << std::endl;

    lastPos.x = posData[0];
    lastPos.y = posData[1];
    lastPos.z = posData[2];
    lastPos.w = posData[3];

    delete[] posData;
    delete[] lastPosP;
    delete[] normalData;
    delete[] pxs;

    getBox()->bindBuffers();
    ShaderSystem::twoDPainting().use();
    this->paintingFBO.bind();
    this->paintingFBO.setColorBuffer(paintingTexture, GL_TEXTURE_2D);
    this->paintingFBO.removeRenderbuffer();
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

void update_depth_texture(Texture depth_texture, Camera cam, Mesh& mesh){
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    glm::ivec2 resolution = depth_texture.getResolution();

    //Bind the capture framebuffer
    paintingFBO.setRenderbuffer(depthRBO1024);
    paintingFBO.bind();

    //Bind the depth texture (Painter class public member variable)
    paintingFBO.setColorBuffer(depth_texture, GL_TEXTURE_2D);

    //Clear the depth texture
    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    ShaderSystem::depth3D().use();
    ShaderSystem::depth3D().setMat4("view", cam.viewMatrix);
    ShaderSystem::depth3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::depth3D().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::depth3D().setInt("usingMeshSelection", this->faceSelection.activated);
    ShaderSystem::depth3D().setInt("hideUnselected", this->faceSelection.hideUnselected);
    ShaderSystem::depth3D().setInt("selectedPrimitiveIDS", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, this->faceSelection.selectedFaces.ID);
    ShaderSystem::depth3D().setInt("meshMask", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);

    //Draw the selected mesh in 3D
    ShaderSystem::depth3D().setInt("primitiveCount", mesh.indices.size() / 3);
    mesh.Draw(false);

    //!Finished
    //Set back to default shader
    ShaderSystem::buttonShader().use();

    this->paintingFBO.removeRenderbuffer();

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