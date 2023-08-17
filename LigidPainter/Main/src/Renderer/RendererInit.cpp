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
#include "3D/ThreeD.hpp"
#include "Renderer.h"
#include "MouseSystem/Mouse.hpp"

#include "LigidGL/LigidGL.hpp"

#include "tinyfiledialogs.h"

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);

Renderer::Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size

    if(!LigidGL::isAdmin()){
        int res = tinyfd_messageBox("Warning!", "LigidPainter has no admin priviliges (run the app as administrator)! std::filesystem might cause a crash. Do you want to proceed?", "yesno", "warning", 0);
        if(res == 0){
            LigidGL::forceClose();
        }
    }

    //Create the window and make it's OpenGL context current    
    context.window.createWindow(videoScale.x, videoScale.y, L"LigidPainter");
    
    //Show the created window
    context.window.show();

    if(!context.window.setWindowIcon(L"./LigidPainter/Resources/Icons/logo-1080x.ico"))
        LGDLOG::start<< "ERROR : Can't change the icon of the window" << LGDLOG::end;

    context.window.setWindowUserPointer(this);

    auto cursorPosFunc = [](LigidWindow w, double x, double y)
    {
        Renderer* renderer = static_cast<Renderer*>(w.getWindowUserPointer());
        if (renderer)
        {
            renderer->cursorPositionCallback(w, x, y);
        }
    };
    
    context.window.setMousePosCallback(cursorPosFunc);
    
    //Scroll callback function casting
    auto scrollFunc = [](LigidWindow w, double x, double y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->scrollCallback(w,x,y);
    };
    context.window.setScrollCallback(scrollFunc);

    //Framebuffer size callback function casting
    auto framebufferSizeFunc = [](LigidWindow w, int x, int y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->framebufferSizeCallback(w,x,y);
    };
    context.window.setWindowSizeCallback(framebufferSizeFunc);

    //Mouse button callback function casting
    auto mouseButtonFunc = [](LigidWindow w, int button, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->mouseButtonCallback(w,button,action,mods);
    };

    context.window.setMouseButtonCallback(mouseButtonFunc);

    //Key button callback function casting
    auto keyFunc = [](LigidWindow w, int key, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->keyCallback(w, key, 0, action, mods);
    };

    context.window.setKeyCallback(keyFunc);

    //Hold the videoscale value inside of the scene structure
    scene.videoScale = videoScale;

    //Get the window size
    context.windowScale = videoScale;

    //Initialize the GLAD
    initGlad();

    //Update necessary data before callbacks
    updateViewport();
    updateViewMatrix();
    updateProjectionMatrix();
    
    //--Load the app textures
    loadAppTextures();

    //Load shaders
    ShaderSystem::initShaderSystem();

    //Load the sphere model
    model.loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
    sphereModel.loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);

    //Init the skybox    
    skybox.init();

    //Create 2D square vertex buffers
    box.init();
    
    //Load the default skybox
    skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky6"); //Skybox's itself
    skybox.createPrefilterMap(videoScale); //Create prefiltered skybox
    skybox.createDisplayingTxtr(sphereModel, context.windowScale); //Create displaying texture
    
    //Load the fonts
    fonts.Arial.loadFont("./LigidPainter/Resources/Fonts/Arial.ttf");

    //Init the text renderer
    textRenderer = TextRenderer(fonts.Arial, videoScale);
    
    //Create context menus
    createContextMenus();
    
    //Init websites
    websites.ligidTools     =   Website("https://ligidtools.com/");
    websites.youTube        =   Website("https://www.youtube.com/channel/UCMVLfsYsd5WAKEWsgM7fjtA");

    //Init the userinterface
    userInterface.init(context, appTextures, websites, videoScale, sphereModel);

    //Init mouse class
    Mouse::setWindow(context.window);
    
    //Load the cursors of the LigidPainter
    Mouse::loadCursors();

    //Init the painter
    painter.initPainter(videoScale);

    //Create the mesh node
    NodeScene::addNode(Node(MESH_NODE, 0, colorPalette, appTextures, videoScale));

    //Load the inputs of the mesh node
    NodeScene::getNode(0)->uploadNewIOs(model, colorPalette);

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

void Renderer::loadAppTextures(){
    appTextures.TDModelIcon.load("./LigidPainter/Resources/Icons/TDModel.png");
    appTextures.softenIcon.load("./LigidPainter/Resources/Icons/Soften.png");
    appTextures.smearIcon.load("./LigidPainter/Resources/Icons/Smear.png");
    appTextures.modifierIcon.load("./LigidPainter/Resources/Icons/Modifier.png");
    appTextures.environmentIcon.load("./LigidPainter/Resources/Icons/Environment.png");
    appTextures.dropperIcon.load("./LigidPainter/Resources/Icons/Dropper.png");
    appTextures.colorIcon.load("./LigidPainter/Resources/Icons/Color.png");
    appTextures.brushIcon.load("./LigidPainter/Resources/Icons/Brush.png");
    appTextures.arrowR.load("./LigidPainter/Resources/Icons/ArrowR.png");
    appTextures.arrowL.load("./LigidPainter/Resources/Icons/ArrowL.png");
    appTextures.threeDIcon.load("./LigidPainter/Resources/Icons/3D.png");
    appTextures.twoDIcon.load("./LigidPainter/Resources/Icons/2D.png");
    appTextures.exportIcon.load("./LigidPainter/Resources/Icons/Export.png");
    appTextures.areaPaintingIcon.load("./LigidPainter/Resources/Icons/Area.png");
    appTextures.gearIcon.load("./LigidPainter/Resources/Icons/Gear.png");
    appTextures.nodeDisplayingMode.load("./LigidPainter/Resources/Icons/NodeDisplayingMode.png");
    appTextures.paintingDisplayingMode.load("./LigidPainter/Resources/Icons/PaintingDisplayingMode.png");
    appTextures.solidPaintingDisplayingMode.load("./LigidPainter/Resources/Icons/SolidPaintingDisplayingMode.png");
    appTextures.ligidPainterIcon.load("./LigidPainter/Resources/Icons/LigidPainter.png");
    appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
    appTextures.noMaterialConnectedToTheMeshWarningImage.load("./LigidPainter/Resources/Images/NoMaterialWarning.jpg");
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