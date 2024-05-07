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

#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "Toolkit/Toolkits.hpp"

#include "GUI/Panels.hpp"
#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

static void renderCursor(glm::vec3 pos, glm::vec3 normal, float radius){
    for (size_t i = 0; i < getTDBrushCursorModel()->meshes.size(); i++)
    {
        glm::mat4 transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(pos[0], pos[1], pos[2]));
        
        glm::vec3 initialForward(0.0f, 0.0f, 1.0f);

        // Desired normal vector
        glm::vec3 normalVector(normal[0], normal[1], normal[2]);
        normalVector = glm::normalize(normalVector); // Ensure it's a unit vector

        // Compute the rotation axis using cross product
        glm::vec3 rotationAxis = glm::cross(initialForward, normalVector);
        float rotationAngle = glm::acos(glm::dot(initialForward, normalVector));

        // Create a rotation matrix to align the model's initial forward direction with the normal vector
        transform = glm::rotate(transform, rotationAngle, rotationAxis);
        
        if(i == 1)
            transform = glm::scale(transform, glm::vec3(0.05f));
        else if(i == 0)
            transform = glm::scale(transform, glm::vec3(0.01f + radius * 4.f));
                    
        ShaderSystem::color3d().setMat4("modelMatrix", transform);

        getTDBrushCursorModel()->meshes[i].Draw("3DCursor : renderCursor");
    }
} 

void render_3D_cursor(float radius)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    ShaderSystem::color3d().use();
    ShaderSystem::color3d().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::color3d().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3d().setVec4("color", glm::vec4(1.f));
    ShaderSystem::color3d().setFloat("depthToleranceValue", 0);
    
    float* posData = new float[4]; 
    float* normalData = new float[4]; 

    getScene()->get_selected_mesh()->getPosNormalOverPoint(
                                                            glm::vec2(
                                                                        Mouse::cursorPos()->x, 
                                                                        getContext()->windowScale.y - Mouse::cursorPos()->y
                                                                    ),
                                                            posData,
                                                            normalData,
                                                            true
                                                        );

    glm::vec3 pos = glm::vec3(posData[0], posData[1], posData[2]);
    glm::vec3 normal = glm::vec3(normalData[0], normalData[1], normalData[2]);

    Settings::defaultFramebuffer()->FBO.bind();

    for (MirrorSide* mirrorSide : painting_get_selected_mirror_sides(checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1, checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1, checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1))
    {
        glm::vec3 offset = glm::vec3(
                                        checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value,
                                        checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value,
                                        checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value
                                    );

        offset *= glm::max(mirrorSide->effectAxis, 0.f);

        ThreeDPoint des_point = ThreeDPoint(glm::vec3(pos * (mirrorSide->effectAxis * -1.f) - offset * 2.f), glm::vec3(normal * (mirrorSide->effectAxis * -1.f)));

        if(posData[3] != 0.f)
            renderCursor(des_point.pos, des_point.normal, radius);
    }
    

    delete[] posData;
    delete[] normalData;
}