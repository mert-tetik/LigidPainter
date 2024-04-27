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

std::atomic<bool> readMaterialThread_active;

std::thread thread_readMaterial;

Material* thread_readMaterial_material = nullptr;
std::string thread_readMaterial_path;
std::atomic<bool> thread_readMaterial_goodToGo;

ThreadElements readMaterialThreadElements;
void readMaterialThread(){    
    while (readMaterialThreadElements.isRunning){

        std::lock_guard<std::mutex> lock(readMaterialThreadElements.mutex);

        if(thread_readMaterial_goodToGo && thread_readMaterial_material != nullptr){
            // Start 
            readMaterialThread_active = true;
            
            getSecondContext()->window.makeContextCurrent();
            
            std::ifstream rf(thread_readMaterial_path, std::ios::out | std::ios::binary); 
            std::vector<Texture *> to_generate_txtrs;
            
            FileHandler::readMaterialData(rf, *thread_readMaterial_material, &to_generate_txtrs);

            /*
            for (Texture* texture : to_generate_txtrs)
            {
                glGenTextures(1, &texture->ID);
                texture->generateProceduralDisplayingTexture(512, true);
            }
            */
           
            thread_readMaterial_material->updateMaterialDisplayingTexture(512, false, Camera(), 0, false);

            // End
            readMaterialThread_active = false;

            thread_readMaterial_goodToGo = false;
        }

    }
}