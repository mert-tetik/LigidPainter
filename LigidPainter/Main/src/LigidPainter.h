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
#include <thread>
#include <atomic>
#include <mutex>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Project/Project.hpp"

#include "Renderer.h"


extern bool ligidPainter_ONLY_INIT;

Renderer::Renderer(){

}

class LigidPainter{
public:
    int run(){

        // //Create the copy context for another threads    
        getCopyContext()->window.createWindow(1, 1, L"Copy Thread");
        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    
        
        // .x : width | .y : height | .z : refresh rate  
        glm::ivec3 primaryMonitorData;
        LigidGL::getPrimaryMonitorData(primaryMonitorData.x, primaryMonitorData.y, primaryMonitorData.z);

        *Settings::videoScale() = glm::vec2(primaryMonitorData.x, primaryMonitorData.y);

        //Create the window and make it's OpenGL context current    
        getContext()->window.createWindow(Settings::videoScale()->x, Settings::videoScale()->y, L"LigidPainter");

        //Show the created window
        getContext()->window.show();

        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //--Load the app textures
        Debugger::block("LOAD : App Textures"); //Start 160567296 153.13 MB
        Settings::loadAppTextures();
        Debugger::block("LOAD : App Textures"); //End
        
        //--Load the app textures
        Debugger::block("LOAD : App Videos"); //Start 160567296 153.13 MB
        Settings::loadAppVideos();
        Debugger::block("LOAD : App Videos"); //End

        Debugger::block("LOAD : Shaders"); //Start 12300288
    
        //Load shaders 
        ShaderSystem::initShaderSystem();
        
        Debugger::block("LOAD : Shaders"); //End

        Debugger::block("LOAD : Overall renderer"); //Start
        Renderer renderer;
        renderer.initRenderer();
        Debugger::block("LOAD : Overall renderer"); //End
        
        Debugger::block("LOAD : Init other threads"); //Start
        // Start the export thread
        std::thread projectUpdatingThreadX(projectUpdatingThread);
        Debugger::block("LOAD : Init other threads"); //End

        LGDLOG::start.clear();

        if(ligidPainter_ONLY_INIT)
            return 1;

        while((!dialog_log.windowShouldClose && !Settings::properties()->cat_hide) || (!getContext()->window.shouldClose() && Settings::properties()->cat_hide))
        {   
            renderer.render();
            
            // Notify the testThread to print "working"
            {
                //std::unique_lock<std::mutex> lock(testMutex);
                //exportCV.notify_one();
            }
        }

        if(std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp"))
            std::filesystem::remove_all(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp");

        // Signal the projectUpdatingThread to exit
        //projectUpdatingThreadElements.exportCV.notify_one();
        projectUpdatingThreadElements.isRunning = false;

        project_discard_update_flag = true;

        // Wait for the projectUpdatingThread to finish
        projectUpdatingThreadX.join();

        getContext()->window.deleteContext();
        getCopyContext()->window.deleteContext();

        return 1;
    }
};