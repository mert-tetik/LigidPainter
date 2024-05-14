/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

void Gizmo::render(
                        Timer &timer, //Timer that handles the animations
                        bool doMouseTracking //If there is need to check if mouse hover
                    ){

    this->doMouseTracking = doMouseTracking;

    // pos value % of the video scale
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value


    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", hoverBtn.hoverMixVal / 2.f);

    hoverBtn.pos = this->pos;
    hoverBtn.scale = this->scale;
    hoverBtn.outline = false;
    hoverBtn.outlineExtra = false;
    hoverBtn.radius = scale.x / 2.f;
    hoverBtn.color2 = hoverBtn.color;
    hoverBtn.render(timer, doMouseTracking);

    this->hover = hoverBtn.hover;
    
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    // Start drawing
    ShaderSystem::gizmo().use();
    ShaderSystem::gizmo().setVec3("pos", resultPos);
    ShaderSystem::gizmo().setVec2("scale", resultScale);
    ShaderSystem::gizmo().setMat4("projection", glm::ortho(0.f,(float)getContext()->windowScale.x,(float)getContext()->windowScale.y,0.f));

    glm::mat4 rotationMat = glm::mat4(1.);

    rotationMat = glm::rotate(rotationMat, 0.f, glm::vec3(1.f, 0.f, 0.f));
    rotationMat = glm::rotate(rotationMat, pitch, glm::vec3(0.f, 0.f, 1.f));
    rotationMat = glm::rotate(rotationMat, yaw, glm::vec3(0.f, 1.f, 0.f));

    ShaderSystem::gizmo().setMat4("rotMat", rotationMat);
    ShaderSystem::gizmo().setInt("state", this->state);
    ShaderSystem::gizmo().setInt("solidShading", 0);
    
    ShaderSystem::gizmo().setInt("rHover", rHover);
    ShaderSystem::gizmo().setInt("gHover", gHover);
    ShaderSystem::gizmo().setInt("bHover", bHover);
    ShaderSystem::gizmo().setInt("rHoverN", rHoverN);
    ShaderSystem::gizmo().setInt("gHoverN", gHoverN);
    ShaderSystem::gizmo().setInt("bHoverN", bHoverN);
    
    ShaderSystem::gizmo().setFloat("rod_radius", 0.025f);

    getBox()->draw("Gizmo : Render to screen");

    ShaderUTIL::release_bound_shader();

    this->rHover = false;
    this->gHover = false;
    this->bHover = false;
    this->rHoverN = false;
    this->gHoverN = false;
    this->bHoverN = false;
    
    if(this->hover){

        if(!this->captureTxtr.ID){
            this->captureTxtr = Texture((char*)nullptr, Settings::videoScale()->x, Settings::videoScale()->y);
        }

        Framebuffer FBO = FBOPOOL::requestFBO(this->captureTxtr, "Gizmo FBO");

        ShaderSystem::gizmo().setInt("rHover", false);
        ShaderSystem::gizmo().setInt("gHover", false);
        ShaderSystem::gizmo().setInt("bHover", false);
        ShaderSystem::gizmo().setInt("rHoverN", false);
        ShaderSystem::gizmo().setInt("gHoverN", false);
        ShaderSystem::gizmo().setInt("bHoverN", false);
        
        ShaderSystem::gizmo().setFloat("rod_radius", 0.04f);
        
        ShaderSystem::gizmo().setInt("solidShading", 1);
        ShaderSystem::gizmo().setMat4("projection", glm::ortho(0.f,(float)Settings::videoScale()->x,(float)Settings::videoScale()->y,0.f));

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0,0,Settings::videoScale()->x, Settings::videoScale()->y);

        getBox()->draw("Gizmo : Render to custom fbo for checking hovering");

        unsigned char* pxs = new unsigned char[4]; 
        
        glReadPixels(
                        Mouse::cursorPos()->x, 
                        Settings::videoScale()->y - Mouse::cursorPos()->y, 
                        1, 
                        1,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        pxs
                    );

        if(pxs[0] > 200)
            this->rHover = true;
        else{

            if(pxs[1] > 200)
                this->gHover = true;
            else{

                if(pxs[2] > 200)
                    this->bHover = true;
                else{

                    if(pxs[0] > 100)
                        this->rHoverN = true;
                    else{

                        if(pxs[1] > 100)
                            this->gHoverN = true;
                        else{

                            if(pxs[2] > 100)
                                this->bHoverN = true;
                        }
                    }
                }
            }
        }
        
        FBOPOOL::releaseFBO(FBO);
    }

    ShaderUTIL::release_bound_shader();
}