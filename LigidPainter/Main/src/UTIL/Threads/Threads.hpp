/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREADS_HPP
#define THREADS_HPP

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

#include <thread>
#include <mutex>

#include "3D/Material/Material.hpp"
#include "3D/Model/Model.hpp"

struct ThreadElements{
    std::atomic<bool> updateTextures = false;
    
    std::atomic<bool> isRunning = true;
    std::mutex mutex;
    std::condition_variable exportCV;
};

extern std::atomic<bool> project_updating_thread_update_project;
extern std::atomic<bool> project_updating_thread_save_as_project;
extern ThreadElements projectUpdatingThreadElements; 
void projectUpdatingThread();

struct MaterialThreadAction{
    Material* material = nullptr;
    std::string path;
    Model* model = nullptr;
    Mesh* mesh = nullptr;
    MaterialChannels* materialChannels;
    bool update_the_material_displaying_texture = 0;
    bool update_layer_scene_result = 0;
    bool object_texturing_dialog_mode = 0;
    Texture object_texturing_dialog_mask;
    std::vector<bool> update_channel_flags;
    unsigned int resolution = 1024;
};

struct MaterialThread{
    std::atomic<bool> active = false;
    std::thread thread;

    std::vector<MaterialThreadAction> actions;
    
    void read_material_file(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path, unsigned int resolution);
    void apply_material(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution);
    void update_material_displaying_texture(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution);
    void layer_stuff(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, unsigned int resolution);
    void update_object_texturing_dialog_result(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, Texture mask, std::vector<bool> update_channel_flags, unsigned int resolution);

    
private:
    void use_thread(Material* material, Model* model, Mesh* mesh, MaterialChannels* materialChannels, std::string path, 
                    bool update_the_material_displaying_texture, bool update_layer_scene_result, bool object_texturing_dialog_mode, 
                    Texture object_texturing_dialog_mask, std::vector<bool> update_channel_flags, unsigned int resolution);

};

extern MaterialThread material_thread;
extern ThreadElements readMaterialThreadElements;

void material_thread_function();

#endif