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
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

#include "Renderer.h"


extern bool ligidPainter_ONLY_INIT;

Renderer::Renderer(){

}

static void render_intro(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader introShader;
    Texture bgImage;
    bgImage.load("LigidPainter/Resources/Images/IntroImage.jpg");
    introShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/IntroTxtrRendering.frag");
    introShader.use();
    introShader.setMat4("projection", glm::ortho(0.,1.,1.,0.));
    introShader.setVec3("pos", glm::vec3(0.5f));
    introShader.setVec2("scale", glm::vec2(0.5f));
    introShader.setInt("txtr", GL::bindTexture_2D(bgImage.ID, "Rendering intro"));
    
    glm::vec2 intro_scale = *Settings::videoScale() / 2.5f;

    glViewport(0, 0, intro_scale.x, intro_scale.y);
    
    //Show the created window
    getContext()->window.style(1);
    getContext()->window.setWindowSize(intro_scale.x, intro_scale.y);
    getContext()->window.setWindowPos(Settings::videoScale()->x / 2 - intro_scale.x / 2, Settings::videoScale()->y / 2 - intro_scale.y / 2);
    getContext()->window.show();

    getBox()->draw("Rendering intro");

    getContext()->window.swapBuffers();
}

class LigidPainter{
public:
    int run(){

        // Create the copy context for another threads    
        getCopyContext()->window.createWindow(1, 1, L"Copy Thread");
        
        getSecondContext()->window.createWindow(1, 1, L"Second Context");

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
        getContext()->window.releaseContext();

        if(!getContext()->window.shareContext(getSecondContext()->window)){
            LGDLOG::start << "ERROR *CRITICAL* : Can't share OpenGL contexts between main context & second context. Multi-threading features won't be used!" << LGDLOG::end; 
        }
        if(!getContext()->window.shareContext(getCopyContext()->window)){
            LGDLOG::start << "ERROR *CRITICAL* : Can't share OpenGL contexts between main context & copy context. Multi-threading features won't be used!" << LGDLOG::end; 
        }

        getContext()->window.makeContextCurrent();

        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        Debugger::block("LOAD : Box"); //Start 305233920 291.12 MB
        getSecondContext()->window.makeContextCurrent();
        getBox()->init();
        
        getContext()->window.makeContextCurrent();
        getBox()->init();
        Debugger::block("LOAD : Box"); //Start 305233920 291.12 MB

        render_intro();

        //--Load the app textures
        Debugger::block("LOAD : App Textures"); //Start 160567296 153.13 MB
        Settings::loadAppTextures();
        Debugger::block("LOAD : App Textures"); //End

        


        
        //--Load the app textures
        Debugger::block("LOAD : App Videos"); //Start 160567296 153.13 MB
        Settings::loadAppVideos();
        Debugger::block("LOAD : App Videos"); //End

        //Load shaders 
        Debugger::block("LOAD : Shaders"); //Start 12300288
        ShaderSystem::initShaderSystem();
        MaterialModifierShaders::init_shaders();
        Debugger::block("LOAD : Shaders"); //End
        
        Debugger::block("LOAD : Overall renderer"); //Start
        Renderer renderer;
        renderer.initRenderer();
        Debugger::block("LOAD : Overall renderer"); //End

        Debugger::block("LOAD : Init other threads"); //Start
        // Start the export thread
        //std::thread projectUpdatingThreadX(projectUpdatingThread);
        material_thread.thread = std::thread(material_thread_function);
        Debugger::block("LOAD : Init other threads"); //End
        
        LGDLOG::start.clear();

        if(ligidPainter_ONLY_INIT)
            return 1;

        getContext()->window.style(0);
        getContext()->window.setWindowSize(Settings::videoScale()->x, Settings::videoScale()->y);
        getContext()->window.setWindowPos(0, 0);    

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
        //projectUpdatingThreadX.join();
        material_thread.thread.join();

        getContext()->window.deleteContext();
        getCopyContext()->window.deleteContext();
        getSecondContext()->window.deleteContext();

        return 1;
    }
};