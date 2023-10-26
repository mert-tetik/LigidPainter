/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

CHAT GPT (GPT-3.5) preview notes :

1 - Lack of Synchronization: It appears that multiple threads can call project.updateProject() concurrently. Since updateProject seems to modify shared state (e.g., projectProcessing, file system operations), you must ensure proper synchronization to avoid race conditions. Using a std::mutex to lock critical sections is a good start, but you need to be careful about what's being accessed and modified.

2 - File Operations: Multiple threads are performing file operations (e.g., std::filesystem::exists, std::filesystem::remove_all, std::filesystem::create_directory). File operations can be tricky in a multi-threaded environment, and you should ensure that file access is synchronized properly to prevent conflicts.

3 - Shared Resources: If Library::getTextureObj, Library::getMaterialObj, and Library::getBrushObj are accessing shared data, you must ensure that access to these resources is properly synchronized. Otherwise, you might encounter data corruption or crashes.

4 - Thread Safety: You have statements like getCopyContext()->window.makeContextCurrent() and getCopyContext()->window.releaseContext(). These suggest that you are dealing with OpenGL contexts. OpenGL contexts are typically not thread-safe, so you should ensure that context operations are performed in a way that avoids conflicts between threads.

5 - Sleep in a Loop: Using std::this_thread::sleep_for in a loop (as you do in projectUpdatingThread) is generally discouraged because it can lead to busy-waiting and excessive CPU usage. Consider using condition variables or other synchronization mechanisms to achieve the desired timing behavior.

6 - Error Handling: It seems that some errors are logged (e.g., "ERROR CAN'T UPDATE THE PROJECT FOLDER"), but in some cases, the function continues executing. Depending on the error, it might be more appropriate to terminate the function or thread gracefully.

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
#include "SettingsSystem/Settings.hpp"

ThreadElements projectUpdatingThreadElements;

void projectUpdatingThread(Project &project) {
    
    while (projectUpdatingThreadElements.isRunning) {
        {
            std::lock_guard<std::mutex> lock(projectUpdatingThreadElements.mutex);

            if(project.folderPath != ""){
                
                while(true){
                    if(!project.projectProcessing)
                        break;
                }
                
                project.updateProject(true, true);
                projectUpdatingThreadElements.updateTextures = false;
            }
        }

        // Sleep between exports
        const int durationSec = 10;
        for (size_t i = 0; i < durationSec; i++)
        {
            if(!projectUpdatingThreadElements.isRunning)
                break;
                
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
        
    }
}