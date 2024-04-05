/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

// OpenGL
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

// GL Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// LigidPainter
#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include "GUI/GUI.hpp"

#include "3D/Model/Model.hpp"

// System
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

static Framebuffer px_read_FBO;

static bool boxSelectionInteraction(Timer &timer, Mesh* mesh);
static void render_selection_cursor(float radius);
static void select_object(Mesh* mesh);
static void select_all_faces(std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices);
static void unselect_all_faces(std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices);
static void apply_pixels(float* pxs, glm::vec2 scale, std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices);
void updatePrimitivesArrayTexture(Mesh* mesh);

static Framebuffer capture_FBO;

int originalOBJI = 0;

static Camera prev_cam;

bool face_selection_interaction(Timer& timer, Model* model, int meshI, bool registerHistory){
    
    // Check if mesh index is valid
    if(meshI >= model->meshes.size())
        return false;

    // Get the selected mesh
    Mesh* mesh = &model->meshes[meshI];

    if(!px_read_FBO.ID){
        px_read_FBO.generate();
    }

    for (size_t i = 0; i < mesh->face_selection_data.selectedPrimitiveIDs.size(); i++)
    {
        if(mesh->face_selection_data.selectedPrimitiveIDs[i] == 1){
            mesh->face_selection_data.selectedPrimitiveIDs[i] = 0;
            mesh->face_selection_data.changedIndices.push_back(i);
        }
    }
    

    // Scale of the window
    glm::vec2 windowSize = getContext()->windowScale;
    
    // Scale of the user's primary monitor
    glm::vec2 videoScale = *Settings::videoScale();


    if(prev_cam != getScene()->camera){
        mesh->updateObjectIDsTexture();
        mesh->updateModelPrimitivesTexture();
    }
    prev_cam = getScene()->camera;
    
    boxSelectionInteraction(timer, mesh);
    
    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)){
        // Select all the faces
        if(shortcuts_CTRL_A()){
            if(registerHistory){
                registerFaceSelectionAction("Select all the faces - Painting", mesh->face_selection_data.selectedPrimitiveIDs, meshI);
            }

            select_all_faces(&mesh->face_selection_data.selectedPrimitiveIDs, &mesh->face_selection_data.changedIndices);
        }

        if(*Mouse::LClick() && registerHistory)
            registerFaceSelectionAction("Face selection - Painting", mesh->face_selection_data.selectedPrimitiveIDs, meshI);
    
        px_read_FBO.setColorBuffer(mesh->face_selection_data.modelPrimitives, GL_TEXTURE_2D);
        px_read_FBO.bind();

        glm::vec2 pos;
        glm::ivec2 scale;
        
        if(mesh->face_selection_data.selectionModeIndex == 0){
            scale = glm::vec2(model->meshes[meshI].face_selection_data.radius);
            
            pos.x = UTIL::new_value_range(Mouse::cursorPos()->x, 0, getContext()->windowScale.x, 0, mesh->objectIDs.getResolution().x);
            pos.y = UTIL::new_value_range(Mouse::cursorPos()->y, 0, getContext()->windowScale.y, 0, mesh->objectIDs.getResolution().y);
            pos.y = mesh->objectIDs.getResolution().y - pos.y;
            pos -= (glm::vec2)scale/2.f;
        }
        else if(mesh->face_selection_data.selectionModeIndex == 1){
            glm::vec2 boxSelectionStartSc = model->meshes[meshI].face_selection_data.boxSelectionStart / 100.f * *Settings::videoScale();
            glm::vec2 boxSelectionEndSc = model->meshes[meshI].face_selection_data.boxSelectionEnd / 100.f * *Settings::videoScale();
            boxSelectionStartSc.x = UTIL::new_value_range(boxSelectionStartSc.x, 0, getContext()->windowScale.x, 0, mesh->objectIDs.getResolution().x);
            boxSelectionStartSc.y = UTIL::new_value_range(boxSelectionStartSc.y, 0, getContext()->windowScale.y, 0, mesh->objectIDs.getResolution().y);
            boxSelectionEndSc.x = UTIL::new_value_range(boxSelectionEndSc.x, 0, getContext()->windowScale.x, 0, mesh->objectIDs.getResolution().x);
            boxSelectionEndSc.y = UTIL::new_value_range(boxSelectionEndSc.y, 0, getContext()->windowScale.y, 0, mesh->objectIDs.getResolution().y);
            
            boxSelectionStartSc.y = mesh->objectIDs.getResolution().y - boxSelectionStartSc.y;
            boxSelectionEndSc.y = mesh->objectIDs.getResolution().y - boxSelectionEndSc.y;
            
            scale = (boxSelectionEndSc - boxSelectionStartSc);
            pos = boxSelectionStartSc;

            if(boxSelectionStartSc.x > boxSelectionEndSc.x){
                scale.x = (boxSelectionStartSc.x - boxSelectionEndSc.x);
                pos.x = boxSelectionEndSc.x;
            } 
            if(boxSelectionStartSc.y > boxSelectionEndSc.y){
                scale.y = (boxSelectionStartSc.y - boxSelectionEndSc.y);
                pos.y = boxSelectionEndSc.y;
            } 
        }

        if(scale.x < 1)
            scale.x = 1;
        if(scale.y < 1)
            scale.y = 1;

        float* pxs =  new float[scale.x * scale.y];

        if (
                pos.x >= 0 && pos.y >= 0 &&                // Ensure pos is within the buffer
                pos.x + scale.x <= mesh->face_selection_data.modelPrimitives.getResolution().x &&            // Ensure the region does not exceed buffer width
                pos.y + scale.y <= mesh->face_selection_data.modelPrimitives.getResolution().y &&            // Ensure the region does not exceed buffer height
                scale.x > 0 && scale.y > 0
            )
        {
            glReadPixels(
                            pos.x, 
                            pos.y, 
                            scale.x, 
                            scale.y,
                            GL_RED,
                            GL_FLOAT,
                            pxs
                        );
        }
        else{
            for (size_t i = 0; i < scale.x * scale.y; i++)
            {
                pxs[i] = 0;
            }
        }

        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && (*Mouse::LClick() || mesh->face_selection_data.selectionModeIndex == 1)){
            unselect_all_faces(&mesh->face_selection_data.selectedPrimitiveIDs, &mesh->face_selection_data.changedIndices);
        }
        
        apply_pixels(pxs, scale, &mesh->face_selection_data.selectedPrimitiveIDs, &mesh->face_selection_data.changedIndices);

        delete[] pxs;
    }

    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)){
        select_object(mesh);
    }

    bool changes_done = mesh->face_selection_data.changedIndices.size();

    if(mesh->face_selection_data.changedIndices.size()){
        updatePrimitivesArrayTexture(mesh);
    }

    // Set back to default
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    render_selection_cursor(mesh->face_selection_data.radius);

    ShaderSystem::buttonShader().use();

    getBox()->bindBuffers();

    glDepthFunc(GL_LEQUAL);

    return changes_done;
}

























static bool __lastMousePressState = false;

bool boxSelectionInteraction(Timer &timer, Mesh* mesh){
    
    if(*Mouse::LClick()){
        mesh->face_selection_data.boxSelectionStart = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    }

    mesh->face_selection_data.boxSelectionEnd = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    
    glm::vec2 btnScale = (mesh->face_selection_data.boxSelectionEnd - mesh->face_selection_data.boxSelectionStart) / 2.f;

    if(mesh->face_selection_data.boxSelectionStart.x > mesh->face_selection_data.boxSelectionEnd.x){
        btnScale.x = (mesh->face_selection_data.boxSelectionStart.x - mesh->face_selection_data.boxSelectionEnd.x) / 2.f;
    } 
    if(mesh->face_selection_data.boxSelectionStart.y > mesh->face_selection_data.boxSelectionEnd.y){
        btnScale.y = (mesh->face_selection_data.boxSelectionStart.y - mesh->face_selection_data.boxSelectionEnd.y) / 2.f;
    } 
   
    /*
    Button btn = Button(ELEMENT_STYLE_SOLID, btnScale, "", Texture(), 0.f, false);
    btn.pos = glm::vec3(mesh->face_selection_data.boxSelectionStart + btn.scale, 0.9f); 
    
    if(mesh->face_selection_data.boxSelectionStart.x > mesh->face_selection_data.boxSelectionEnd.x)
        btn.pos.x = mesh->face_selection_data.boxSelectionEnd.x + btn.scale.x;
    if(mesh->face_selection_data.boxSelectionStart.y > mesh->face_selection_data.boxSelectionEnd.y)
        btn.pos.y = mesh->face_selection_data.boxSelectionEnd.y + btn.scale.y;
    
    if(*Mouse::LPressed())
        btn.render(timer, false); 
    */

    bool applyBox = __lastMousePressState && !*Mouse::LPressed();
    __lastMousePressState = *Mouse::LPressed();
    
    return applyBox;
}

void updatePrimitivesArrayTexture(Mesh* mesh){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->face_selection_data.selectedFaces.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(mesh->face_selection_data.changedIndices.size() > 50){
        while (glGetError() != GL_NO_ERROR)
        {
        }

        int prim_txtr_res = int(ceil(sqrt(mesh->face_selection_data.selectedPrimitiveIDs.size())));
    
        unsigned char* pxs = new unsigned char[prim_txtr_res * prim_txtr_res];
        for (size_t i = 0; i < prim_txtr_res * prim_txtr_res; i++)
        {
            if(i < mesh->face_selection_data.selectedPrimitiveIDs.size())
                pxs[i] = mesh->face_selection_data.selectedPrimitiveIDs[i] * 126;
            else
                pxs[i] = mesh->face_selection_data.selectedPrimitiveIDs[mesh->face_selection_data.selectedPrimitiveIDs.size()-1] * 126;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, prim_txtr_res, prim_txtr_res, GL_RED, GL_UNSIGNED_BYTE, pxs);
        glGenerateMipmap(GL_TEXTURE_2D);

        if(glGetError() == 1281){
            LGDLOG::start << "ERROR : Face selection : This mesh's vertex data is too high to apply a face selection." << LGDLOG::end;
        }

        delete[] pxs;
    }

    for (size_t i = 0; i < mesh->face_selection_data.changedIndices.size(); i++)
    {
        if(mesh->face_selection_data.changedIndices[i] < mesh->face_selection_data.selectedPrimitiveIDs.size() && mesh->face_selection_data.changedIndices[i] >= 0){
            unsigned char pxs[1];
            pxs[0] = (unsigned char)(mesh->face_selection_data.selectedPrimitiveIDs[mesh->face_selection_data.changedIndices[i]] * 126);

            float prim_txtr_res = int(ceil(sqrt(mesh->face_selection_data.selectedPrimitiveIDs.size())));
            float prim_height = floor((float)mesh->face_selection_data.changedIndices[i] / prim_txtr_res);
            glm::ivec2 offset = glm::ivec2((float)mesh->face_selection_data.changedIndices[i] - (prim_height * prim_txtr_res), prim_height);

            glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, pxs);
        }
    }
    
    mesh->face_selection_data.changedIndices.clear();
}

static void select_all_faces(std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices){
    std::fill(selectedPrimitiveIDs->begin(), selectedPrimitiveIDs->end(), 2);
    
    for (size_t i = 0; i < selectedPrimitiveIDs->size(); i++)
    {
        changedIndices->push_back(i);
    }
}

static void unselect_all_faces(std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices){
    for (size_t i = 0; i < selectedPrimitiveIDs->size(); i++)
    {
        if((*selectedPrimitiveIDs)[i] != 0)
            changedIndices->push_back(i);
    }
    
    std::fill(selectedPrimitiveIDs->begin(), selectedPrimitiveIDs->end(), 0);
}

static void apply_pixels(float* pxs, glm::vec2 scale, std::vector<byte>* selectedPrimitiveIDs, std::vector<int>* changedIndices){
    for (size_t i = 0; i < scale.x * scale.y; i++)
    {
        pxs[i] -= 1;
        
        if(pxs[i] < selectedPrimitiveIDs->size() && pxs[i] >= 0){
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                if((*selectedPrimitiveIDs)[pxs[i]] == 0){
                    if(*Mouse::LPressed())
                        (*selectedPrimitiveIDs)[pxs[i]] = 2;
                    else
                        (*selectedPrimitiveIDs)[pxs[i]] = 1;
                    changedIndices->push_back(pxs[i]);
                }
            }
            else{
                if((*selectedPrimitiveIDs)[pxs[i]] == 2){
                    if(*Mouse::LPressed()){
                        (*selectedPrimitiveIDs)[pxs[i]] = 0;
                        changedIndices->push_back(pxs[i]);
                    }
                }
            }
            
        }
    }
}

static void select_object(Mesh* mesh){
    int objI = -1;
    int meshI = -1;

    std::vector<int> selected_object_indices;

    if(shortcuts_CTRL_A()){
        for (size_t i = 0; i < mesh->objects.size(); i++)
        {
            selected_object_indices.push_back(i);
        }
    }
    else{
        px_read_FBO.setColorBuffer(mesh->objectIDs, GL_TEXTURE_2D);
        px_read_FBO.bind();

        glm::vec2 pos;
        pos.x = UTIL::new_value_range(Mouse::cursorPos()->x, 0, getContext()->windowScale.x, 0, mesh->objectIDs.getResolution().x);
        pos.y = UTIL::new_value_range(Mouse::cursorPos()->y, 0, getContext()->windowScale.y, 0, mesh->objectIDs.getResolution().y);
        pos.y = mesh->objectIDs.getResolution().y - pos.y;

        float* pxs = new float[2]; 
        
        glReadPixels(
                        pos.x, 
                        pos.y, 
                        1, 
                        1,
                        GL_RG,
                        GL_FLOAT,
                        pxs
                    );

        objI = pxs[0];
        meshI = pxs[1] - 1;
        
        delete[] pxs;
        
        if(meshI != -1){
            if(*Mouse::LClick()){
                if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                    selected_object_indices.push_back(objI);
                }
            }

            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                Mouse::setCursor(*Mouse::pointerCursor());
            else
                Mouse::setCursor(*Mouse::pointerXCursor());
            for (size_t oI = 0; oI < mesh->objects.size(); oI++)
            {
                // Check the selected objects
                bool contains = false;
                for (size_t i = 0; i < selected_object_indices.size(); i++)
                {
                    if(selected_object_indices[i] == oI)
                        contains = true;
                }
                
                if(contains || (oI == objI)){
                    for (size_t i = mesh->objects[oI].vertIndices.x / 3; i < mesh->objects[oI].vertIndices.y / 3; i++)
                    {
                        if(i < mesh->face_selection_data.selectedPrimitiveIDs.size()){
                            // Update changed indices array
                            mesh->face_selection_data.changedIndices.push_back(i);

                            // Update selected primitives array
                            if(contains) // Is selected
                                mesh->face_selection_data.selectedPrimitiveIDs[i] = 2;
                            else if(mesh->face_selection_data.selectedPrimitiveIDs[i] != 2) // Is hovered
                                mesh->face_selection_data.selectedPrimitiveIDs[i] = 1;
                        }
                    }
                }
            }
        }
    }
}

static void render_selection_cursor(float radius){
    getBox()->bindBuffers();
    
    /* Use the circle shader */
    ShaderSystem::circleShader().use();

    /* Set the transform data & the projection */
    ShaderSystem::circleShader().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::circleShader().setVec3("pos", glm::vec3(Mouse::cursorPos()->x, Mouse::cursorPos()->y, 1));
    ShaderSystem::circleShader().setVec2("scale", glm::vec2(UTIL::new_value_range(radius, 0, 1024, 0, Settings::videoScale()->x)));

    /* Hide the cursor */
    getContext()->window.setCursorVisibility(false);

    /* Render the circle s*/
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Rendering 2D brush cursor");

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    ShaderSystem::buttonShader().use();
}