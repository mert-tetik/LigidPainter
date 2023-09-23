/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init.cpp - Contains the constructor of the class Renderer - from Renderer.h

*/

#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <mutex>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "LibrarySystem/Library.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"

#include "LigidGL/LigidGL.hpp"

#include "tinyfiledialogs.h"


void Renderer::initRenderer(){
    if(!getContext()->window.setWindowIcon(L"./LigidPainter/Resources/Icons/logo-1080x.ico"))
        LGDLOG::start<< "ERROR : Can't change the icon of the window" << LGDLOG::end;

    getContext()->window.setWindowUserPointer(this);

    auto cursorPosFunc = [](LigidWindow w, double x, double y)
    {
        Renderer* renderer = static_cast<Renderer*>(w.getWindowUserPointer());
        if (renderer)
        {
            renderer->cursorPositionCallback(w, x, y);
        }
    };
    
    getContext()->window.setMousePosCallback(cursorPosFunc);
    
    //Scroll callback function casting
    auto scrollFunc = [](LigidWindow w, double x, double y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->scrollCallback(w,x,y);
    };
    getContext()->window.setScrollCallback(scrollFunc);

    //Framebuffer size callback function casting
    auto framebufferSizeFunc = [](LigidWindow w, int x, int y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->framebufferSizeCallback(w,x,y);
    };
    getContext()->window.setWindowSizeCallback(framebufferSizeFunc);

    //Mouse button callback function casting
    auto mouseButtonFunc = [](LigidWindow w, int button, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->mouseButtonCallback(w,button,action,mods);
    };

    getContext()->window.setMouseButtonCallback(mouseButtonFunc);

    //Key button callback function casting
    auto keyFunc = [](LigidWindow w, int key, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->keyCallback(w, key, 0, action, mods);
    };

    getContext()->window.setKeyCallback(keyFunc);

    //Get the window size
    getContext()->windowScale = *Settings::videoScale();

    getScene()->tiles.init();
    getScene()->axisDisplayer.init();

    //Update necessary data before callbacks
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);    
    getScene()->updateViewMatrix();
    getScene()->updateTransformMatrix();
    getScene()->updateProjectionMatrix();
    
    //--Load the app textures
    Settings::loadAppTextures();

    //Load shaders
    ShaderSystem::initShaderSystem();

    //Load the sphere model
    getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
    getSphereModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
    getMaterialDisplayerModel()->loadModel("./LigidPainter/Resources/3D Models/MaterialDisplayer.obj",true);

    //Init the skybox    
    skybox.init();

    //Create 2D square vertex buffers
    getBox()->init();
    
    //Load the default skybox
    skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky6"); //Skybox's itself
    skybox.createPrefilterMap(); //Create prefiltered skybox
    skybox.createDisplayingTxtr(); //Create displaying texture
    
    //Load the fonts
    fonts.Arial.loadFont("./LigidPainter/Resources/Fonts/Arial.ttf");

    //Init the text renderer
    textRenderer = TextRenderer(fonts.Arial);
    
    //Init websites
    websites.ligidTools     =   Website("https://ligidtools.com/");
    websites.youTube        =   Website("https://www.youtube.com/channel/UCMVLfsYsd5WAKEWsgM7fjtA");

    //Init the userinterface
    userInterface.init(websites);

    //Load the cursors of the LigidPainter
    Mouse::loadCursors();

    //Init the painter
    painter.initPainter();

    //Create the mesh node
    NodeScene::addNode(Node(MESH_NODE, 0));

    //Load the inputs of the mesh node
    NodeScene::getNode(0)->uploadNewIOs();

    try
    {
        //Create the projects folder if not exists
        if(!std::filesystem::exists("./Projects")){
            if(!std::filesystem::create_directory("./Projects"))
                LGDLOG::start<< "ERROR : Creating projects folder." << LGDLOG::end;
        }
        
        //If the tmp directory doesn't exist then create 
        if(!std::filesystem::exists("./tmp")){
            if(!std::filesystem::create_directory("./tmp"))
                LGDLOG::start<< "ERROR : Creating tmp folder." << LGDLOG::end;
        }

        //If the tmp directory exists empty the folder
        else{
            if(!std::filesystem::remove_all("./tmp"))
                LGDLOG::start<< "ERROR : Deleting tmp folder. (after deleting)" << LGDLOG::end;
            if(!std::filesystem::create_directory("./tmp"))
                LGDLOG::start<< "ERROR : Creating tmp folder. (after deleting)" << LGDLOG::end;

        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 963654 " << ex.what() << LGDLOG::end;
    }

    Settings::defaultFramebuffer()->init(*Settings::videoScale());

    // Load the source library textures
    Library::loadSourceLibTextures();

}