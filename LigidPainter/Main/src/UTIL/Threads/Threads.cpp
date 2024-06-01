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
#include "UTIL/Threads/Threads.hpp"

ThreadElements projectUpdatingThreadElements;

std::atomic<bool> project_updating_thread_update_project = false;
std::atomic<bool> project_updating_thread_save_as_project = false;

std::atomic<bool> project_updating_thread_working = false;

void projectUpdatingThread(){

    while (projectUpdatingThreadElements.isRunning) {
        
        std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

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

MaterialThread material_thread;

ThreadElements readMaterialThreadElements;

static MaterialChannels material_channels;

void MaterialThread::update_thread_result(){
    if(this->readyToService && !this->active && this->actions[0].material != nullptr){    
        this->readyToService = false;

        if(!material_thread.actions[0].object_texturing_dialog_mode && !material_thread.actions[0].update_buffers_right_away){
            if(material_thread.actions[0].update_channel_flags.size() != 6){
                material_thread.actions[0].update_channel_flags = {true,true,true,true,true,true};
            }

            unsigned int res = material_thread.actions[0].resolution;

            unsigned char* albedo_pxs = new unsigned char[res * res * 4]; material_channels.albedo.getData(albedo_pxs);
            unsigned char* roughness_pxs = new unsigned char[res * res * 4]; material_channels.roughness.getData(roughness_pxs);
            unsigned char* metallic_pxs = new unsigned char[res * res * 4]; material_channels.metallic.getData(metallic_pxs);
            unsigned char* normalMap_pxs = new unsigned char[res * res * 4]; material_channels.normalMap.getData(normalMap_pxs);
            unsigned char* heightMap_pxs = new unsigned char[res * res * 4]; material_channels.heightMap.getData(heightMap_pxs);
            unsigned char* ambientOcclusion_pxs = new unsigned char[res * res * 4]; material_channels.ambientOcclusion.getData(ambientOcclusion_pxs);

            if(material_thread.actions[0].update_channel_flags[0])
                material_thread.actions[0].materialChannels->albedo.update(albedo_pxs, res, res);
            if(material_thread.actions[0].update_channel_flags[1])
                material_thread.actions[0].materialChannels->roughness.update(roughness_pxs, res, res);
            if(material_thread.actions[0].update_channel_flags[2])
                material_thread.actions[0].materialChannels->metallic.update(metallic_pxs, res, res);
            if(material_thread.actions[0].update_channel_flags[3])
                material_thread.actions[0].materialChannels->normalMap.update(normalMap_pxs, res, res);
            if(material_thread.actions[0].update_channel_flags[4])
                material_thread.actions[0].materialChannels->heightMap.update(heightMap_pxs, res, res);
            if(material_thread.actions[0].update_channel_flags[5])
                material_thread.actions[0].materialChannels->ambientOcclusion.update(ambientOcclusion_pxs, res, res);
        }

        if(material_thread.actions[0].update_the_material_displaying_texture)
            material_thread.actions[0].material->updateMaterialDisplayingTexture(material_thread.actions[0].resolution, false, Camera(), 0, false, (material_thread.actions[0].model != nullptr && material_thread.actions[0].model->meshes.size()) ? material_thread.actions[0].model->meshes[0] : Mesh(), *material_thread.actions[0].materialChannels);

        if(material_thread.actions[0].update_layer_scene_result)
            material_thread.actions[0].mesh->layerScene.update_result(material_thread.actions[0].resolution, glm::vec3(0.f), *material_thread.actions[0].mesh);

        this->actions.erase(this->actions.begin());
        if(this->actions.size()){
            this->active = true;
            this->goodToGo = true;
        }
    }
}

void MaterialThread::read_material_file(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, path, true, false, false, Texture(), {}, resolution, false);
}

void MaterialThread::apply_material(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution, bool update_buffers_right_away){
    this->use_thread(material, model, mesh, materialChannels, "", false, false, false, Texture(), {}, resolution, update_buffers_right_away);
}

void MaterialThread::update_material_displaying_texture(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", true, false, false, Texture(), {}, resolution, false);
}

void MaterialThread::update_object_texturing_dialog_result(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, Texture mask, std::vector<bool> update_channel_flags, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", false, true, true, mask, update_channel_flags, resolution, false);
}

void MaterialThread::layer_stuff(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution){
    this->use_thread(material, model, mesh, materialChannels, "", false, true, false, Texture(), {}, resolution, false);
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
                                    unsigned int resolution,
                                    bool update_buffers_right_away
                                )
{   
    if(!this->readyToService){

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
        action.update_buffers_right_away = update_buffers_right_away;
        
        actions.push_back(action);

        if(actions.size() == 1){
            this->active = true; // Make sure this flag is set to true in time
            this->goodToGo = true;
        }
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
        
        if(material_thread.goodToGo && material_thread.actions[0].material != nullptr)
        {        
            // Start 
            material_thread.active = true;
            
            if(material_thread.actions.size()){
                getSecondContext()->window.makeContextCurrent();
                
                if(material_thread.actions[0].path.size()){
                    std::ifstream rf(material_thread.actions[0].path, std::ios::out | std::ios::binary); 
                    //std::vector<Texture *> to_generate_txtrs;
                    
                    FileHandler::readMaterialData(rf, *material_thread.actions[0].material, nullptr);
                }

                if(!material_channels.albedo.ID){
                    material_channels.albedo = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.roughness = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.metallic = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.normalMap = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.heightMap = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.ambientOcclusion = Texture((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                }
                else{
                    material_channels.albedo.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.roughness.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.metallic.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.normalMap.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.heightMap.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                    material_channels.ambientOcclusion.update((char*)nullptr, material_thread.actions[0].resolution, material_thread.actions[0].resolution);
                }

                MaterialChannels* dest_channels;
                if(material_thread.actions[0].update_buffers_right_away){
                    dest_channels = material_thread.actions[0].materialChannels; 
                }
                else{
                    dest_channels = &material_channels; 
                }

                material_thread.actions[0].material->apply_material((material_thread.actions[0].model != nullptr) ? *material_thread.actions[0].model : Model(), *material_thread.actions[0].mesh, dest_channels, material_thread.actions[0].resolution, false);

                if(material_thread.actions[0].object_texturing_dialog_mode)
                {
                    if(material_thread.actions[0].update_channel_flags.size() != 6){
                        material_thread.actions[0].update_channel_flags = {true,true,true,true,true,true};
                    }
                    
                    // Mask source to material result
                    if(material_thread.actions[0].update_channel_flags[0])
                        material_thread.actions[0].materialChannels->albedo.mix(dest_channels->albedo, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[1])
                        material_thread.actions[0].materialChannels->roughness.mix(dest_channels->roughness, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[2])
                        material_thread.actions[0].materialChannels->metallic.mix(dest_channels->metallic, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[3])
                        material_thread.actions[0].materialChannels->normalMap.mix(dest_channels->normalMap, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[4])
                        material_thread.actions[0].materialChannels->heightMap.mix(dest_channels->heightMap, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                    if(material_thread.actions[0].update_channel_flags[5])
                        material_thread.actions[0].materialChannels->ambientOcclusion.mix(dest_channels->ambientOcclusion, material_thread.actions[0].object_texturing_dialog_mask, false, false, false);
                }
            }

            // End
            material_thread.active = false;
            material_thread.goodToGo = false;
            material_thread.readyToService = true;
        }

    }
}