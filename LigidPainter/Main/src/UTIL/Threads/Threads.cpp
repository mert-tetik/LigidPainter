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

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Threads/Threads.hpp"

ThreadElements projectUpdatingThreadElements;

std::atomic<bool> project_updating_thread_update_project = false;
std::atomic<bool> project_updating_thread_save_as_project = false;

void projectUpdatingThread(){

    /* Make copy context current for this thread */
    getCopyContext()->window.makeContextCurrent();

    while (projectUpdatingThreadElements.isRunning) {
        
        std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

        /* Auto update every *durationSec* seconds*/
        if(project_path() != ""){
            project_update(true, true);
        }

        // Sleep between exports
        const int durationSec = 15;
        for (size_t i = 0; i < durationSec; i++)
        {
            /* Update on user request */
            if(project_updating_thread_update_project){
                if(project_path() != ""){
                    project_update(true, false);
                }
                project_updating_thread_update_project = false;
            }
            
            /* Save as on user request */
            if(project_updating_thread_save_as_project){
                if(project_path() != ""){
                    project_update(true, false);
                    
                    project_save_as("");
                }
                project_updating_thread_save_as_project = false;
            }

            if(!projectUpdatingThreadElements.isRunning)
                break;
                
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
        
    }
}

void updatePrimitivesArrayTexture(Mesh* mesh, bool update_all);

MaterialThread material_thread;

ThreadElements readMaterialThreadElements;

void MaterialThread::update_thread_result(){
    if(this->readyToService && !this->active && this->material != nullptr){    
        this->readyToService = false;

        if(material_thread.update_layer_scene_result)
            material_thread.mesh->layerScene.update_result(1024, glm::vec3(0.f), *material_thread.mesh);
    }
}

void MaterialThread::read_material_file(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path){
    this->use_thread(material, model, mesh, materialChannels, path, true, false);
}

void MaterialThread::apply_material(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels){
    this->use_thread(material, model, mesh, materialChannels, "", false, false);
}

void MaterialThread::update_material_displaying_texture(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels){
    this->use_thread(material, model, mesh, materialChannels, "", true, false);
}

void MaterialThread::layer_stuff(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels){
    this->use_thread(material, model, mesh, materialChannels, "", false, true);
}

void MaterialThread::use_thread(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path, bool update_the_material_displaying_texture, bool update_layer_scene_result){   
    if(!this->active && !this->readyToService){
        this->active = true; // Make sure this flag is set to true in time

        this->material = material;
        this->path = path;
        this->model = model;
        this->mesh = mesh;
        this->materialChannels = materialChannels;
        this->update_the_material_displaying_texture = update_the_material_displaying_texture;
        this->update_layer_scene_result = update_layer_scene_result;
        
        this->goodToGo = true;
    }
}


void material_thread_function(){    
    // Create the copy context for another threads    
    getSecondContext()->window.makeContextCurrent();

    if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
    {
        LGDLOG::start<< "Failed to initialize GLAD for material thread" << LGDLOG::end;
    }   

    getSecondContext()->window.releaseContext();

    while (readMaterialThreadElements.isRunning){
        std::lock_guard<std::mutex> lock(readMaterialThreadElements.mutex);

        if(getSecondContext()->window.isContextCurrent())
            getSecondContext()->window.releaseContext();
        
        if(material_thread.goodToGo && material_thread.material != nullptr){        
            // Start 
            material_thread.active = true;
            
            getSecondContext()->window.makeContextCurrent();
            
            if(material_thread.path.size()){
                std::ifstream rf(material_thread.path, std::ios::out | std::ios::binary); 
                std::vector<Texture *> to_generate_txtrs;
                
                FileHandler::readMaterialData(rf, *material_thread.material, &to_generate_txtrs);
            }
            
            material_thread.material->apply_material((material_thread.model != nullptr) ? *material_thread.model : Model(), *material_thread.mesh, material_thread.materialChannels, 1024, false);
            
            if(material_thread.update_the_material_displaying_texture)
                material_thread.material->updateMaterialDisplayingTexture(1024, false, Camera(), 0, false, (material_thread.model != nullptr) ? *material_thread.model : Model());

            // End
            material_thread.active = false;
            material_thread.goodToGo = false;
            material_thread.readyToService = true;
        }

    }
}
