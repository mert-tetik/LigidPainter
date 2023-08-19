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
#include "3D/ThreeD.hpp"
#include "Renderer.h"

#include "LigidGL/LigidGL.hpp"

#include "tinyfiledialogs.h"

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);

Renderer::Renderer(){//Settings::Videoscale() is the resolution value that will be used for viewport & window size

    if(!LigidGL::isAdmin()){
        int res = tinyfd_messageBox("Warning!", "LigidPainter has no admin priviliges (run the app as administrator)! std::filesystem might cause a crash. Do you want to proceed?", "yesno", "warning", 0);
        if(res == 0){
            LigidGL::forceClose();
        }
    }

    //Create the window and make it's OpenGL context current    
    getContext()->window.createWindow(Settings::videoScale()->x, Settings::videoScale()->y, L"LigidPainter");
    
    //Show the created window
    getContext()->window.show();

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

    //Initialize the GLAD
    initGlad();

    //Update necessary data before callbacks
    updateViewport();
    updateViewMatrix();
    updateProjectionMatrix();
    
    //--Load the app textures
    Settings::loadAppTextures();

    //Load shaders
    ShaderSystem::initShaderSystem();

    //Load the sphere model
    getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
    getSphereModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);

    //Init the skybox    
    skybox.init();

    //Create 2D square vertex buffers
    box.init();
    
    //Load the default skybox
    skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky6"); //Skybox's itself
    skybox.createPrefilterMap(); //Create prefiltered skybox
    skybox.createDisplayingTxtr(); //Create displaying texture
    
    //Load the fonts
    fonts.Arial.loadFont("./LigidPainter/Resources/Fonts/Arial.ttf");

    //Init the text renderer
    textRenderer = TextRenderer(fonts.Arial);
    
    //Create context menus
    createContextMenus();
    
    //Init websites
    websites.ligidTools     =   Website("https://ligidtools.com/");
    websites.youTube        =   Website("https://www.youtube.com/channel/UCMVLfsYsd5WAKEWsgM7fjtA");

    //Init the userinterface
    userInterface.init(websites, *getSphereModel());

    //Load the cursors of the LigidPainter
    Mouse::loadCursors();

    //Init the painter
    painter.initPainter();

    //Create the mesh node
    NodeScene::addNode(Node(MESH_NODE, 0, colorPalette));

    //Load the inputs of the mesh node
    NodeScene::getNode(0)->uploadNewIOs(colorPalette);

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




//FUNCTIONS


void Renderer::initGlad(){
    //Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
    {
        LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
    }    
}

void Renderer::createContextMenus(){

    //!Create context menus                                                   0            1                  2               3                     4           5  
    //Library panel
    contextMenus.push_back(ContextMenu(colorPalette,{"Rename"  , "Duplicate"       , "Copy Path"   , "Delete"})); //Textures  0
    contextMenus.push_back(ContextMenu(colorPalette,{"Edit"    , "Add To Scene"    , "Rename"      , "Duplicate"   ,    "Copy Path" ,  "Delete",   "Export"}));    //Materials 1
    contextMenus.push_back(ContextMenu(colorPalette,{"Use"     , "Apply Current"   , "Rename"      , "Duplicate"   ,    "Copy Path" ,  "Delete"}));    //Brushes   2
    
    //Menu Bar (navigation panel)
    contextMenus.push_back(ContextMenu(colorPalette,{"Save"    , "Save as"         , "Create new"  , "Load new"    ,    "Copy Path", "File Explorer"})); //Project   3 
    contextMenus.push_back(ContextMenu(colorPalette,{"Undo"    , "Redo"    })); //Painting  4
    contextMenus.push_back(ContextMenu(colorPalette,{"Website" , "YouTube" })); //Help      5
    
    //Material editor
    contextMenus.push_back(ContextMenu(colorPalette,{"Delete"  , "Move To Top"     , "Move To Bottom", "Change Mask"}));// Material modifier context menu 6                                           
    
    //Nodes
    contextMenus.push_back(ContextMenu(colorPalette,{"Delete"})); //Node context menu 7

    //Material editor
    contextMenus.push_back(ContextMenu(colorPalette,{"Texture Modifier", "Dust Modifier", "Asphalt Modifier", "Fabric Modifier", "Moss Modifier", "Rust Modifier", "Skin Modifier", "Solid Modifier", "Wooden Modifier"}));// Material modifier selection context menu 8                                        
    
    //Node panel
    contextMenus.push_back(ContextMenu(colorPalette,{"Add Material ID Node", "Add Material Mask Node"}));// Mesh node scene panel context menu 9                                        
}