/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

static Texture projectToModelTxtr;

void VectorStroke3D::projectToModel(std::vector<VertexUTIL>& vertices, glm::vec3 center){
    
    if(!vertices.size())
        return;

    glm::vec3 camPos = glm::vec3(center + vertices[0].Normal * 5.f);
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    const unsigned int resolution = 1024;

    if(!projectToModelTxtr.ID){
        projectToModelTxtr = Texture((char*)nullptr, resolution, resolution);
    }

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(projectToModelTxtr, projectToModelTxtr.getResolution(), "ThreeDBox::projectToModel fbo");

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::renderModelData().use();
    ShaderSystem::renderModelData().setMat4("view", view);
    ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
    ShaderSystem::renderModelData().setInt("state", 1);

    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::renderModelData(), *getScene()->get_selected_mesh());
    
    getScene()->get_selected_mesh()->Draw("VectorStroke3D::projectToModel");

    GL::releaseBoundTextures("VectorStroke3D : projectToModel");
    
    float* pxs = new float[resolution * resolution * 4]; 
    float* pxsNormal = new float[resolution * resolution * 4]; 
    
    glReadPixels(
                    0, 
                    0, 
                    resolution, 
                    resolution,
                    GL_RGBA,
                    GL_FLOAT,
                    pxs
                );
    
    ShaderSystem::renderModelData().setInt("state", 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ShaderSystem::renderModelData().setInt("primitiveCount", getScene()->get_selected_mesh()->indices.size() / 3);
    getScene()->get_selected_mesh()->Draw("VectorStroke3D::projectToModel");
    
    glReadPixels(
                    0, 
                    0, 
                    resolution, 
                    resolution,
                    GL_RGBA,
                    GL_FLOAT,
                    pxsNormal
                );

    for (size_t i = 0; i < vertices.size(); i++)
    {
        glm::vec3* pos = &vertices[i].Position;
        glm::vec3* normal = &vertices[i].Normal;
        glm::vec4 projectedPos = getScene()->projectionMatrix * view * glm::vec4(*pos, 1.f);
        projectedPos /= projectedPos.z;
        projectedPos /= projectedPos.w;

        projectedPos.x = (projectedPos.x + 1.f) / 2.f;
        projectedPos.y = (projectedPos.y + 1.f) / 2.f;
        
        projectedPos.x *= resolution;
        projectedPos.y *= resolution;

        int index = (((int)projectedPos.y) * resolution + (int)projectedPos.x) * 4;

        if(index + 3 < resolution * resolution * 4 && index >= 0){
            if(pxs[index + 3] != 0.f){
                pos->x = pxs[index + 0]; 
                pos->y = pxs[index + 1]; 
                pos->z = pxs[index + 2]; 
                
                normal->x = pxsNormal[index + 0]; 
                normal->y = pxsNormal[index + 1]; 
                normal->z = pxsNormal[index + 2]; 

                pos->x = pos->x * 2.f - 1.f;
                pos->y = pos->y * 2.f - 1.f;
                pos->z = pos->z * 2.f - 1.f;
                
                normal->x = normal->x * 2.f - 1.f;
                normal->y = normal->y * 2.f - 1.f;
                normal->z = normal->z * 2.f - 1.f;
            }
        }
    }

    FBOPOOL::releaseFBO(FBO);

    delete[] pxs;
}

void VectorStroke3D::genLineVertices(){
    unsigned int details = (unsigned int)(glm::distance(this->startPoint.pos, this->endPoint.pos) * 20.f) + 1;

    this->lineVertices.clear();
 
    for (size_t i = 0; i < details; i++)
    {
        VertexUTIL vert;
        vert.Position = this->startPoint.pos - (this->startPoint.pos - this->endPoint.pos) / glm::vec3(details) * glm::vec3(i);
        vert.Normal = this->startPoint.normal - (this->startPoint.normal - this->endPoint.normal) / glm::vec3(details) * glm::vec3(i);
        this->lineVertices.push_back(vert);
    }
}

void VectorStroke3D::updateLinePoints(){
    
    LigidGL::cleanGLErrors();

    this->genLineVertices();
    this->projectToModel(this->lineVertices, (startPoint.pos + endPoint.pos) / 2.f);
}

VectorStroke3D::VectorStroke3D(ThreeDPoint startPoint, ThreeDPoint endPoint){
    this->startPoint = startPoint;
    this->endPoint = endPoint;
}

void VectorStroke3D::renderLine(){
    // Use the 3D color instancing shader program
    ShaderSystem::color3DInstancing().use();
    
    // Set the transform projection matrix for each instance
    for (size_t i = 0; i < this->lineVertices.size(); i++)
    {
        glm::mat4 transMat = glm::mat4(1.f);
        transMat = glm::translate(transMat, this->lineVertices[i].Position);
        transMat = glm::scale(transMat, glm::vec3(0.005f));
        
        ShaderSystem::color3DInstancing().setMat4("modelMatrix[" + std::to_string(i) + "]", transMat);
    }

    // Set related uniforms
    ShaderSystem::color3DInstancing().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::color3DInstancing().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3DInstancing().setFloat("depthToleranceValue", 0);
    ShaderSystem::color3DInstancing().setVec4("color", glm::vec4(1.f));

    // Render
    LigidGL::cleanGLErrors();
    if(getSphereModel()->meshes.size()){
        // draw mesh
        glBindBuffer(GL_ARRAY_BUFFER, getSphereModel()->meshes[0].VBO);
        LigidGL::testGLError("VectorStroke3D::draw : Binding VBO");

        glBindVertexArray(getSphereModel()->meshes[0].VAO);
        LigidGL::testGLError("VectorStroke3D::draw : Binding VAO");

        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(getSphereModel()->meshes[0].indices.size()), GL_UNSIGNED_INT, 0, this->lineVertices.size());
        LigidGL::testGLError("VectorStroke3D::draw : Drawing Elements");
    }
}

bool VectorStroke3D::draw(Timer& timer, float edge, bool doMouseTracking, std::vector<VectorStroke3D>& strokes, int curI){
    
    bool clicked = false;

    glm::vec4 startClr = glm::vec4(1.f);
    glm::vec4 endClr = glm::vec4(1.f);
    
    // If last stroke
    if(curI == strokes.size() - 1){
        endClr = glm::vec4(148.f / 255.f, 194.f / 255.f, 132.f / 255.f, 1.f);
    }
    else{
        this->endPoint = strokes[curI + 1].startPoint;
    }

    // If first stroke 
    if(curI == 0){
        startClr = glm::vec4(194.f / 255.f, 132.f / 255.f, 177.f / 255.f, 1.f);
    }

    this->startPoint.color = startClr;
    this->endPoint.color = endClr;

    if(this->startPoint.render(timer, doMouseTracking, false, 0.015f, true))
        clicked = true;
    
    if(curI == strokes.size() - 1){
        if(this->endPoint.render(timer, doMouseTracking, false, 0.015f, true))
            clicked = true;
    }
    
    if(this->startPoint.moving || this->endPoint.moving){
        this->updateLinePoints();
    }

    this->renderLine();
    
    return clicked;
}
