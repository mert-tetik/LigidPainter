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

void projectUpdatingThread(){
    while (projectUpdatingThreadElements.isRunning) {
        {
            std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

            if(project_path() != ""){
                project_update(true, true);
            }
        }

        // Sleep between exports
        const int durationSec = 15;
        for (size_t i = 0; i < durationSec; i++)
        {
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
    if(this->readyToService && !this->active && this->model != nullptr && this->material != nullptr){    
        this->readyToService = false;
    }
}

void MaterialThread::read_material_file(Material* material, Model* model, int mesh_index, std::string path){
    this->use_thread(material, model, mesh_index, path, true);
}

void MaterialThread::apply_material(Material* material, Model* model, int mesh_index){
    this->use_thread(material, model, mesh_index, "", false);
}

void MaterialThread::update_material_displaying_texture(Material* material, Model* model, int mesh_index){
    this->use_thread(material, model, mesh_index, "", true);
}

void MaterialThread::use_thread(Material* material, Model* model, int mesh_index, std::string path, bool update_the_material_displaying_texture){   
    if(!this->active && !this->readyToService){
        this->active = true; // Make sure this flag is set to true in time

        this->material = material;
        this->path = path;
        this->model = model;
        this->mesh_index = mesh_index;
        this->update_the_material_displaying_texture = update_the_material_displaying_texture;
        
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

        if(material_thread.goodToGo && material_thread.material != nullptr){        
            // Start 
            material_thread.active = true;
            
            getSecondContext()->window.makeContextCurrent();
            
            if(material_thread.path.size()){
                std::ifstream rf(material_thread.path, std::ios::out | std::ios::binary); 
                std::vector<Texture *> to_generate_txtrs;
                
                FileHandler::readMaterialData(rf, *material_thread.material, &to_generate_txtrs);
            }
            
            material_thread.material->apply_material(*material_thread.model, material_thread.model->meshes[material_thread.mesh_index], 1024, false);
            
            if(material_thread.update_the_material_displaying_texture)
                material_thread.material->updateMaterialDisplayingTexture(1024, false, Camera(), 0, false, *material_thread.model);


            // End
            material_thread.active = false;
            material_thread.goodToGo = false;
            material_thread.readyToService = true;
        }

    }
}
