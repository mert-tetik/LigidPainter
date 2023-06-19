/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init.cpp - Contains the constructor of the class Renderer - from Renderer.h

*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"

Renderer::Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size
    //Hold the videoscale value inside of the scene structure
    scene.videoScale = videoScale;

    //Initialize the GLFW, create the main window & set callbacks
    initGLFW(videoScale);

    //Get the window size
    glfwGetFramebufferSize(context.window,&context.windowScale.x,&context.windowScale.y);

    //Initialize the GLAD
    initGlad();

    //Update necessary data before callbacks
    updateViewport();
    updateViewMatrix();
    updateProjectionMatrix();
    
    //--Load the app textures
    loadAppTextures();

    //Load shaders
    loadShaders();

    //Load the sphere model
    model.loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
    sphereModel.loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);

    //Init the skybox    
    skybox.init();

    //Create 2D square vertex buffers
    box.init();
    
    //Load the default skybox
    skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky2"); //Skybox's itself
    skybox.createPrefilterMap(shaders.prefilteringShader,videoScale); //Create prefiltered skybox
    skybox.createDisplayingTxtr(shaders.skyboxBall,sphereModel,context.windowScale); //Create displaying texture
    
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
    userInterface.init(shaders,context,appTextures,websites,videoScale,sphereModel);

    //Init mouse class
    mouse = Mouse(context.window);
    
    //Load the cursors of the LigidPainter
    mouse.loadCursors();

    //Create app nodes
    createAppNodes(videoScale);

    //Init the painter
    painter.initPainter(videoScale,shaders.twoDPainting,shaders.buttonShader,shaders.tdModelShader,shaders.depth3D);

    //Create the projects folder if it's not exists
    if(!std::filesystem::exists("./Projects")){
        std::filesystem::create_directory("./Projects");
    }
}




//FUNCTIONS




void Renderer::initGLFW(glm::vec2 videoScale){
    //Init GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    context.window = glfwCreateWindow(videoScale.x, videoScale.y, "LigidPainter", NULL, NULL);
    
    if (context.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
    }
    
    glfwMakeContextCurrent(context.window);
    

    //Pointing a function that is a class member
    glfwSetWindowUserPointer(context.window, this);
    
    //Cursor position callback function casting
    auto cursorPosFunc = [](GLFWwindow* w, double x, double y)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(w))->cursorPositionCallback(w,x,y);
    };
    glfwSetCursorPosCallback(context.window, cursorPosFunc);
    
    //Scroll callback function casting
    auto scrollFunc = [](GLFWwindow* w, double x, double y)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(w))->scrollCallback(w,x,y);
    };
    glfwSetScrollCallback(context.window, scrollFunc);
    
    //Framebuffer size callback function casting
    auto framebufferSizeFunc = [](GLFWwindow* w, int x, int y)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(w))->framebufferSizeCallback(w,x,y);
    };
    glfwSetFramebufferSizeCallback(context.window, framebufferSizeFunc);
    
    //Mouse button callback function casting
    auto mouseButtonFunc = [](GLFWwindow* w, int button, int action, int mods)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(w))->mouseButtonCallback(w,button,action,mods);
    };

    glfwSetMouseButtonCallback(context.window, mouseButtonFunc);
    
    //Key button callback function casting
    auto keyFunc = [](GLFWwindow* w, int key, int scancode, int action, int mods)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(w))->keyCallback(w,key,scancode,action,mods);
    };

    glfwSetKeyCallback(context.window, keyFunc);
}

void Renderer::initGlad(){
    //Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
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
    appTextures.threeDIcon.load("./LigidPainter/Resources/Icons/3D.png");
    appTextures.twoDIcon.load("./LigidPainter/Resources/Icons/2D.png");
    appTextures.exportIcon.load("./LigidPainter/Resources/Icons/Export.png");
    appTextures.areaPaintingIcon.load("./LigidPainter/Resources/Icons/Area.png");
    appTextures.gearIcon.load("./LigidPainter/Resources/Icons/Gear.png");
    appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
}

void Renderer::loadShaders(){
    shaders.tdModelShader =         Shader("LigidPainter/Resources/Shaders/3DModel.vert"    ,   "LigidPainter/Resources/Shaders/3DModel.frag"           ,nullptr);
    shaders.depth3D =               Shader("LigidPainter/Resources/Shaders/3DModel.vert"    ,   "LigidPainter/Resources/Shaders/Depth3D.frag"           ,nullptr);
    shaders.skyboxShader =          Shader("LigidPainter/Resources/Shaders/Skybox.vert"     ,   "LigidPainter/Resources/Shaders/Skybox.frag"            ,nullptr);
    shaders.skyboxBall =            Shader("LigidPainter/Resources/Shaders/3DModel.vert"    ,   "LigidPainter/Resources/Shaders/SkyboxBall.frag"        ,nullptr);
    shaders.prefilteringShader =    Shader("LigidPainter/Resources/Shaders/Skybox.vert"     ,   "LigidPainter/Resources/Shaders/PrefilterSkybox.frag"   ,nullptr);
    shaders.buttonShader =          Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert"   ,   "LigidPainter/Resources/Shaders/UI/Button.frag"         ,nullptr);
    shaders.singleCurve =           Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert"   ,   "LigidPainter/Resources/Shaders/UI/SingleCurve.frag"    ,nullptr);
    shaders.colorPicker =           Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert"   ,   "LigidPainter/Resources/Shaders/UI/ColorPicker.frag"    ,nullptr);
    shaders.twoDPainting =          Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert"   ,   "LigidPainter/Resources/Shaders/2DPainting.frag"        ,nullptr);
}

void Renderer::createContextMenus(){

    //!Create context menus                                                   0            1                  2               3                     4           5  
    //Library panel
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Rename"  , "Duplicate"       , "Copy Path"   , "Delete"}));                                      //Textures  0
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Edit"    , "Add To Scene"    , "Rename"      , "Duplicate"   ,    "Copy Path" ,  "Delete"}));    //Materials 1
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Use"     , "Apply Current"   , "Rename"      , "Duplicate"   ,    "Copy Path" ,  "Delete"}));    //Brushes   2
    //Menu Bar (navigation panel)
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Save"    , "Save as"         , "Create new"  , "Load new"    ,    "Copy Path"}));                //Project   3 
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Undo"    , "Redo"    }));                                                                        //Painting  4
    contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Website" , "YouTube" }));                                                                        //Help      5
}

void Renderer::createAppNodes(glm::vec2 videoScale){
    //Material node
    Node materialNode;
    materialNode.loadIO
    (
        {
            NodeIO("Input1",Element(Button(BUTTON_STYLE_SOLID,glm::vec2(1,8),colorPalette,shaders.buttonShader,"Input1",appTextures.TDModelIcon,2.f,false)),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale,1),
        },
        {
            NodeIO("Input1",Element(Button(BUTTON_STYLE_SOLID,glm::vec2(1,1),colorPalette,shaders.buttonShader,"Input1",appTextures.TDModelIcon,2.f,false)),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale,2),
        },
        shaders.buttonShader,
        shaders.singleCurve,
        colorPalette,
        0,
        1
    );

    //Mesh output node

    //Create inputs with meshes of the model 
    Node meshOutputNode;
    std::vector <NodeIO> meshOutputNodeInputElements;
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        meshOutputNodeInputElements.push_back(NodeIO(model.meshes[i].materialName,Element(Button(BUTTON_STYLE_SOLID,glm::vec2(1,1),colorPalette,shaders.buttonShader,model.meshes[i].materialName,Texture(),2.f,false)),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale,0));
    }
    
    //Create the mesh output node
    meshOutputNode.loadIO
    (
        meshOutputNodeInputElements,
        {

        },
        shaders.buttonShader,
        shaders.singleCurve,
        colorPalette,
        0,
        0
    );
    
    //Send nodes to the appNodes
    appNodes.push_back(meshOutputNode);
    appNodes.push_back(materialNode);

    //Send the mesh output node to the 
    nodeScene.push_back(meshOutputNode);                                                         
}