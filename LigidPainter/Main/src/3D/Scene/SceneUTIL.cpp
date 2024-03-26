/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <iostream>

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include "GUI/Panels.hpp"

void Scene::updateProjectionMatrix(float ratio){
    if(getContext()->windowScale.x){
        if(ratio == 0.f)
            ratio = (float)getContext()->windowScale.x / (float)getContext()->windowScale.y;
        
        if(!useOrtho){
            this->projectionMatrix = glm::perspective(glm::radians(this->fov), 
                                                ratio, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                                this->aNear, 
                                                this->aFar);
        }
        else{
            this->projectionMatrix = glm::ortho((-this->fov * ratio)/10.f, (this->fov * ratio)/10.f, (-this->fov)/10.f, (this->fov)/10.f, (this->aNear), (this->aFar));
        }
    }
}

void Scene::updateTransformMatrix(){
    this->transformMatrix = glm::mat4(1.f);
    this->transformMatrix = glm::translate(this->transformMatrix, transformLocation);
    this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.x), glm::vec3(0.f, 1.f, 0.f));
    this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.y), glm::vec3(1.f, 0.f, 0.f));
    this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.z), glm::vec3(0.f, 0.f, 1.f));
}

static Mesh empty_mesh;
Mesh* Scene::get_selected_mesh(){
    if(button_mesh_selection.selectedMeshI < this->model->meshes.size())
        return &this->model->meshes[button_mesh_selection.selectedMeshI];

    return &empty_mesh;
}