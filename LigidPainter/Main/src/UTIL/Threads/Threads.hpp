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

struct MaterialChannelsPxs{
    unsigned char albedo[1024 * 1024 * 4];
    unsigned char roughness[1024 * 1024 * 4];
    unsigned char metallic[1024 * 1024 * 4];
    unsigned char normalMap[1024 * 1024 * 4];
    unsigned char heightMap[1024 * 1024 * 4];
    unsigned char ao[1024 * 1024 * 4];
};

struct MaterialThread{
    std::atomic<bool> active = false;
    std::atomic<bool> goodToGo = false;
    std::atomic<bool> readyToService = false;
    std::thread thread;
    Material* material = nullptr;
    std::string path;
    Model* model = nullptr;
    int mesh_index = 0;
    bool update_the_material_displaying_texture = 0;

    MaterialChannelsPxs material_channels_pxs;

    void update_thread_result();
    
    void read_material_file(Material* material, Model* model, int mesh_index, std::string path);
    void apply_material(Material* material, Model* model, int mesh_index);
    void update_material_displaying_texture(Material* material, Model* model, int mesh_index);
    
private:
    void use_thread(Material* material, Model* model, int mesh_index, std::string path, bool update_the_material_displaying_texture);

};

extern MaterialThread material_thread;

void material_thread_function();

#endif