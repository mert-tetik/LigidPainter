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
#include <cstdlib> 

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/Library/Library.hpp"
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
    introShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/IntroTxtrRendering.frag", "introShader");
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

    // Release the intro shader
    ShaderUTIL::release_bound_shader();
}

std::atomic<bool> shader_system_initialised = false;
std::atomic<bool> load_ligidpainter_done = false;

void load_ligidpainter(){
    getLoadingContext()->window.makeContextCurrent();

    shader_system_initialised = false;
    ShaderSystem::initShaderSystem();
    shader_system_initialised = true;

    //--Load the app textures
    Debugger::block("LOAD : App Textures"); //Start 160567296 153.13 MB
    Settings::loadAppTextures(); 
    Debugger::block("LOAD : App Textures"); //End

    //--Load the app textures
    Debugger::block("LOAD : App Videos"); //Start 160567296 153.13 MB
    Settings::loadAppVideos();
    Debugger::block("LOAD : App Videos"); //End

    //Load shaders 
    Debugger::block("LOAD : SOURCE LIB TEXTURES"); //Start 16916480
    // Load the source library textures
    Library::loadSourceLibTextures();
    Debugger::block("LOAD : SOURCE LIB TEXTURES"); //End
    
    Debugger::block("LOAD : Models"); //Start
    //Load the 3D models 345235456
    getScene()->model->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true, false);
    getSphereModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true, true);
    getPlaneModel()->loadModel("./LigidPainter/Resources/3D Models/plane.fbx",true, true);
    getMaterialDisplayerModel()->loadModel("./LigidPainter/Resources/3D Models/MaterialDisplayer.obj", true, true);
    getMaterialDisplayingModel()->loadModel("./LigidPainter/Resources/3D Models/MaterialDisplayer.obj", true, true);
    getTDBrushCursorModel()->loadModel("./LigidPainter/Resources/3D Models/TDBrushCursor.fbx", true, true);
    Debugger::block("LOAD : Models"); //End

    Debugger::block("LOAD : Skybox"); //Start 305233920 291.12 MB
    getScene()->skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky6"); //Skybox textures
    getScene()->skybox.init(); // Skybox vertex buffers
    getScene()->skybox.createPrefilterMap(); //Create prefiltered skybox
    getScene()->skybox.createDisplayingTxtr(); //Create displaying texture
    Debugger::block("LOAD : Skybox"); //End 
    
    getLoadingContext()->window.releaseContext();

    load_ligidpainter_done = true;
    
    LGDLOG::start.clear();
    dialog_log.messages.clear();

    return;
}

class LigidPainter{
public:
    int run(){

        // Side OpenGL contexts
        getLoadingContext()->window.createWindow(1, 1, L"Loading Context");
        getCopyContext()->window.createWindow(1, 1, L"Copy Thread");
        getSecondContext()->window.createWindow(1, 1, L"Second Context");
        // Create the main context
        getContext()->window.createWindow(Settings::videoScale()->x, Settings::videoScale()->y, L"LigidPainter");

        // Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    
        
        // .x : width | .y : height | .z : refresh rate  
        glm::ivec3 primaryMonitorData;
        LigidGL::getPrimaryMonitorData(primaryMonitorData.x, primaryMonitorData.y, primaryMonitorData.z);

        // Update global monitor resolution value
        *Settings::videoScale() = glm::vec2(primaryMonitorData.x, primaryMonitorData.y);

        // Release the lastly created context for sharing prep
        getContext()->window.releaseContext();

        // Let side contexts share data with main contexts
        if(!getContext()->window.shareContext(getSecondContext()->window)){
            std::cerr << "ERROR *CRITICAL* : Can't share OpenGL contexts between main context & second context." << std::endl; 
            abort();
        }
        if(!getContext()->window.shareContext(getCopyContext()->window)){
            std::cerr << "ERROR *CRITICAL* : Can't share OpenGL contexts between main context & copy context." << std::endl; 
            abort();
        }
        if(!getContext()->window.shareContext(getLoadingContext()->window))
        {
            std::cerr << "ERROR *CRITICAL* : Can't share OpenGL contexts between main context & loading context!" << std::endl; 
            abort();
        }

        // Make main context current for the main thread
        getContext()->window.makeContextCurrent();

        // 
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Init boxes for both contexs (getBox function returns bound context specific box object)
        Debugger::block("LOAD : Box"); //Start 305233920 291.12 MB
        getSecondContext()->window.makeContextCurrent();
        getBox()->init();
        getContext()->window.makeContextCurrent();
        getBox()->init();
        Debugger::block("LOAD : Box"); //Start 305233920 291.12 MB
        
        // Render the LigidPainter intro
        render_intro();

        getScene()->model->meshes.push_back(Mesh({}, {}, "", false));
        getSphereModel()->meshes.push_back(Mesh({}, {}, "", false));
        getPlaneModel()->meshes.push_back(Mesh({}, {}, "", false));
        getMaterialDisplayerModel()->meshes.push_back(Mesh({}, {}, "", false));
        getMaterialDisplayingModel()->meshes.push_back(Mesh({}, {}, "", false));
        getTDBrushCursorModel()->meshes.push_back(Mesh({}, {}, "", false));


        Settings::initAppTextures();

        // Load textures in another thread
        std::thread load_ligidpainter_thread(load_ligidpainter);

        getScene()->skybox.init(); // Skybox vertex buffers

        // Wait until shaders are initialised
        while (!shader_system_initialised){}

        // Init renderer
        Debugger::block("LOAD : Overall renderer"); //Start
        Renderer renderer;
        renderer.initRenderer();
        Debugger::block("LOAD : Overall renderer"); //End

        LGDLOG::start.clear();

        if(ligidPainter_ONLY_INIT)
            return 1;
        
        // Start the export thread
        //std::thread projectUpdatingThreadX(projectUpdatingThread);
        // Start the material processing thread
        material_thread.thread = std::thread(material_thread_function);

        getContext()->window.style(0);
        getContext()->window.setWindowSize(Settings::videoScale()->x, Settings::videoScale()->y);
        getContext()->window.setWindowPos(0, 0);

        //dialog_greeting.dialogControl.activate();    

        while((!dialog_log.windowShouldClose && !Settings::properties()->cat_hide) || (!getContext()->window.shouldClose() && Settings::properties()->cat_hide))
        {   
            renderer.render();
            
            // Notify the testThread to print "working"
            {
                //std::unique_lock<std::mutex> lock(testMutex);
                //exportCV.notify_one();
            }
        }
        
        while(!load_ligidpainter_done){}

        if(std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp"))
            std::filesystem::remove_all(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp");

        // Signal the projectUpdatingThread to exit
        //projectUpdatingThreadElements.exportCV.notify_one();
        projectUpdatingThreadElements.isRunning = false;
        readMaterialThreadElements.isRunning = false;

        project_discard_update_flag = true;

        // Wait for the projectUpdatingThread to finish
        //projectUpdatingThreadX.join();
        material_thread.thread.join();
        load_ligidpainter_thread.join();

        getContext()->window.deleteContext();
        getCopyContext()->window.deleteContext();
        getSecondContext()->window.deleteContext();
        getLoadingContext()->window.deleteContext();

        return 1;
    }
};