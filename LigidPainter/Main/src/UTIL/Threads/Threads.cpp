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

#include "GUI/Dialogs.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/Threads/Threads.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"
#include "Toolkit/Layers/Layers.hpp"

ThreadElements projectUpdatingThreadElements;

std::atomic<bool> project_updating_thread_update_project = false;
std::atomic<bool> project_updating_thread_save_as_project = false;

std::atomic<bool> project_updating_thread_working = false;

void projectUpdatingThread(){

    while (projectUpdatingThreadElements.isRunning) {
        
        std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

        if(!dialog_materialSelection.dialogControl.isActive()){
            /* Auto update every *durationSec* seconds*/
            if(project_path() != ""){
                getCopyContext()->window.makeContextCurrent();
                
                project_update(true, true, &project_updating_thread_working);
                
                getCopyContext()->window.releaseContext();
            }

            // Sleep between exports
            const int durationSec = 15;
            for (size_t i = 0; i < durationSec; i++)
            {
                /* Update on user request */
                if(project_updating_thread_update_project){
                    if(project_path() != ""){
                        getCopyContext()->window.makeContextCurrent();
                        
                        project_update(true, false, &project_updating_thread_working);
                        
                        getCopyContext()->window.releaseContext();
                    }
                    project_updating_thread_update_project = false;
                }
                
                /* Save as on user request */
                if(project_updating_thread_save_as_project){
                    if(project_path() != ""){
                        getCopyContext()->window.makeContextCurrent();

                        project_update(true, false, &project_updating_thread_working);
                        project_save_as("", &project_updating_thread_working);

                        getCopyContext()->window.releaseContext();
                    }
                    project_updating_thread_save_as_project = false;
                }

                if(!projectUpdatingThreadElements.isRunning)
                    break;
                    
                std::this_thread::sleep_for(std::chrono::seconds(1)); 
            }
        }
    }
}

MaterialThread material_thread;

ThreadElements readMaterialThreadElements;
ThreadElements vectorThreadElements;


void MaterialThread::read_material_file(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, path, true, false, false, Texture(), {}, resolution);
}

void MaterialThread::apply_material(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", false, false, false, Texture(), {}, resolution);
}

void MaterialThread::update_material_displaying_texture(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", true, false, false, Texture(), {}, resolution);
}

void MaterialThread::update_object_texturing_dialog_result(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, Texture mask, std::vector<bool> update_channel_flags, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", false, true, true, mask, update_channel_flags, resolution);
}

void MaterialThread::layer_stuff(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", false, true, false, Texture(), {}, resolution);
}

void MaterialThread::use_thread(
                                    Material* material, 
                                    Model* model, 
                                    Mesh* mesh, 
                                    MaterialChannels* materialChannels, 
                                    std::string path, 
                                    bool update_the_material_displaying_texture, 
                                    bool update_layer_scene_result,
                                    bool object_texturing_dialog_mode,
                                    Texture object_texturing_dialog_mask,
                                    std::vector<bool> update_channel_flags,
                                    unsigned int resolution
                                )
{   
    if(true){

        MaterialThreadAction action;

        action.material = material;
        action.path = path;
        action.model = model;
        action.mesh = mesh;
        action.materialChannels = materialChannels;
        action.update_the_material_displaying_texture = update_the_material_displaying_texture;
        action.update_layer_scene_result = update_layer_scene_result;
        action.object_texturing_dialog_mode = object_texturing_dialog_mode;
        action.object_texturing_dialog_mask = object_texturing_dialog_mask;
        action.update_channel_flags = update_channel_flags;
        action.resolution = resolution;
        
        actions.push_back(action);

        if(actions.size() == 1){
            this->active = true; // Make sure this flag is set to true in time
        }
    }
}

static MaterialChannels material_channels;

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
        
        if(material_thread.actions.size())
        {        
            if(material_thread.actions[0].material != nullptr){
                // Start 
                material_thread.active = true;
            
                while(!getSecondContext()->window.makeContextCurrent()){}
                
                if(material_thread.actions[0].path.size()){
                    std::ifstream rf(material_thread.actions[0].path, std::ios::out | std::ios::binary); 
                    //std::vector<Texture *> to_generate_txtrs;
                    
                    FileHandler::readMaterialData(rf, *material_thread.actions[0].material, nullptr);
                }

                if(!material_channels.albedo.ID){
                    material_channels.generate_channels(glm::ivec2(material_thread.actions[0].resolution));
                }
                else{
                    material_channels.update_channels(glm::ivec2(material_thread.actions[0].resolution));
                }

                material_thread.actions[0].material->apply_material((material_thread.actions[0].model != nullptr) ? *material_thread.actions[0].model : Model(), *material_thread.actions[0].mesh, &material_channels, material_thread.actions[0].resolution, false);

                if(material_thread.actions[0].update_channel_flags.size() != 6){
                    material_thread.actions[0].update_channel_flags = {true,true,true,true,true,true};
                }

                if(material_thread.actions[0].object_texturing_dialog_mode)
                {
                    // Mask source to material result
                    if(material_thread.actions[0].update_channel_flags[0])
                        material_thread.actions[0].materialChannels->albedo.mix(material_channels.albedo, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[1])
                        material_thread.actions[0].materialChannels->roughness.mix(material_channels.roughness, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[2])
                        material_thread.actions[0].materialChannels->metallic.mix(material_channels.metallic, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[3])
                        material_thread.actions[0].materialChannels->normalMap.mix(material_channels.normalMap, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[4])
                        material_thread.actions[0].materialChannels->heightMap.mix(material_channels.heightMap, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[5])
                        material_thread.actions[0].materialChannels->ambientOcclusion.mix(material_channels.ambientOcclusion, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                }
                else{
                    material_thread.actions[0].materialChannels->apply_another_material_channels(material_channels, material_thread.actions[0].update_channel_flags);
                }

                if(material_thread.actions[0].update_the_material_displaying_texture)
                    material_thread.actions[0].material->updateMaterialDisplayingTexture(material_thread.actions[0].resolution, false, Camera(), 0, false, (material_thread.actions[0].model != nullptr && material_thread.actions[0].model->meshes.size()) ? material_thread.actions[0].model->meshes[0] : Mesh(), *material_thread.actions[0].materialChannels);

                if(material_thread.actions[0].update_layer_scene_result)
                    material_thread.actions[0].mesh->layerScene.update_result(material_thread.actions[0].resolution, glm::vec3(0.f), *material_thread.actions[0].mesh);

                material_thread.actions.erase(material_thread.actions.begin());

                // End
                material_thread.active = false;
            }
        }
    }
}

VectorThread vector_thread;


void VectorThread::use_thread(
                                    unsigned int resolution, 
                                    VectorScene* vector_scene, 
                                    VectorLayer* vector_layer,
                                    Mesh* mesh
                                )
{   
    if(true){

        VectorThreadAction action;

        action.resolution = resolution;
        action.vector_layer = vector_layer;
        action.vector_scene = vector_scene;
        action.mesh = mesh;
        
        actions.push_back(action);
        
        if(actions.size() == 1){
            this->active = true; // Make sure this flag is set to true in time
        }
    }
}

MaterialChannels result_channels;

void vector_thread_function(){    
    // Create the copy context for another threads    
    getSecondContext()->window.makeContextCurrent();

    if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
    {
        LGDLOG::start<< "Failed to initialize GLAD for vector thread" << LGDLOG::end;
    }   

    getSecondContext()->window.releaseContext();

    while (vectorThreadElements.isRunning){
        std::lock_guard<std::mutex> lock(vectorThreadElements.mutex);

        if(getSecondContext()->window.isContextCurrent())
            getSecondContext()->window.releaseContext();
        
        if(vector_thread.actions.size())
        {        
            while(!getSecondContext()->window.makeContextCurrent()){}

            if(vector_thread.actions[0].vector_scene != nullptr){
                vector_thread.active = true;
                
                std::this_thread::sleep_for(std::chrono::seconds(1)); 

                if(!result_channels.albedo.ID){
                    result_channels.generate_channels(glm::ivec2(vector_thread.actions[0].resolution));
                }
                else{
                    result_channels.update_channels(glm::ivec2(vector_thread.actions[0].resolution));
                }

                PaintSettings paint_settings = vector_thread.actions[0].vector_layer->get_painting_settings(vector_thread.actions[0].resolution, *vector_thread.actions[0].mesh);
                paint_settings.painted_buffers = PaintSettings::PaintedBuffers(true, Texture(), true, result_channels.albedo, true, result_channels.roughness, true, result_channels.metallic, true, result_channels.normalMap, true, result_channels.heightMap, true, result_channels.ambientOcclusion);

                vector_thread.actions[0].vector_scene->apply_strokes(
                                                                        true, 
                                                                        false, 
                                                                        paint_settings                                
                                                                    );
                
                vector_thread.actions[0].vector_layer->result.apply_another_material_channels(result_channels, {true,true,true,true,true,true});

                vector_thread.actions[0].mesh->layerScene.update_result(vector_thread.actions[0].resolution, glm::vec3(0.f), *vector_thread.actions[0].mesh);
            
                if(vector_thread.actions.size() > 1)
                    vector_thread.actions = {vector_thread.actions.back()};
                else{
                    vector_thread.actions.clear();
                }

                vector_thread.active = false;
            }
        }
    }
}