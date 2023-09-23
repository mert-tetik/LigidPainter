/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include "UTIL/Util.hpp"
#include "LibrarySystem/Library.hpp"



void projectUpdatingThread(Project &project) {
    while (projectUpdatingThreadElements.isRunning) {
        
        {
            std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

            if(!project.projectLoading)
                project.updateProject();

            // for (size_t i = 0; i < Library::getTextureArraySize(); i++)
            // {
            //     Texture* txtr = Library::getTexture(i);

            //     if(txtr->needsExporting){
            //         txtr->exportTexture(project.folderPath + UTIL::folderDistinguisher() + "Saved", "PNG");
                    
            //         txtr->needsExporting = false;
            //     }
            // }
        }

        // Sleep between exports
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Adjust export frequency as needed
    }
}