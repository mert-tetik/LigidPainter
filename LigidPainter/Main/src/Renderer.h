/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Renderer.h : Renders the whole screen
*/

#ifndef LGDRENDERER_HPP
#define LGDRENDERER_HPP

/*
    Bugs :
    ! Multiple material nodes unconnecting bug
    ! Material editor - modifier panel bug (changing between materials)
*/
#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "3D/Model.hpp"
#include "Shader.hpp"
#include "Brush.hpp"
#include "Skybox.hpp"
#include "Box.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"
#include "TextRendering/Font.hpp"
#include "TextRendering/TextRenderer.hpp"
#include "3D/Material.hpp"
#include "Painter.hpp"


struct Camera{
    float yaw = -90.f;
    float pitch = 0.f;
    glm::vec3 cameraPos = glm::vec3(0,0,-10.f);
    glm::vec3 originPos = glm::vec3(0,0,0);
    float radius = 10.f; 
};
struct Scene{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    const float fov = 45.f;
    const float near = 0.1f;
    const float far = 1000.0f;

    glm::vec2 videoScale;

    Camera camera;

    Skybox skybox;
};
struct Library{
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Brush> brushes;

    void uniqueName(std::string &s,std::vector<std::string> sArray){
	    bool isTheSame = false;
	    for (size_t i = 0; i < sArray.size(); i++)
	    {
	    	if(sArray[i] == s){
	    		isTheSame = true;
	    	}
	    }

	    if(isTheSame){
	    	for (size_t i = 0; i < 1000; i++)
	    	{
	    		bool matched = false;
	    		for (size_t txtrI = 0; txtrI < sArray.size(); txtrI++)
	    		{
	    			if(sArray[txtrI] == s + '(' + std::to_string(i) + ')'){
	    				matched = true;
	    			}
	    		}
	    		if(!matched){
	    			s += '(' + std::to_string(i) + ')';
	    			break;
	    		}
	    	}
	    }
    }

    void uniqueNameControl(){
        for (size_t i = 0; i < textures.size(); i++)
        {
            std::vector<std::string> texturesStr;
            for (size_t istr = 0; istr < textures.size(); istr++)
            {
                if(i != istr)
                    texturesStr.push_back(textures[istr].title);
            }

            uniqueName(textures[i].title,texturesStr);    
        }

        for (size_t i = 0; i < materials.size(); i++)
        {
            std::vector<std::string> materialsStr;
            for (size_t istr = 0; istr < materials.size(); istr++)
            {
                if(i != istr)
                    materialsStr.push_back(materials[istr].title);
            }

            uniqueName(materials[i].title,materialsStr);    
        }

        for (size_t i = 0; i < brushes.size(); i++)
        {
            std::vector<std::string> brushesStr;
            for (size_t istr = 0; istr < brushes.size(); istr++)
            {
                if(i != istr)
                    brushesStr.push_back(brushes[istr].title);
            }

            uniqueName(brushes[i].title,brushesStr);    
        }
    }
};
struct Context{
    GLFWwindow* window;
    glm::ivec2 windowScale;
};
struct Shaders{
    Shader tdModelShader;
    Shader skyboxShader;
    Shader buttonShader;
    Shader prefilteringShader;
    Shader singleCurve;
    Shader colorPicker;
    Shader twoDPainting;
    Shader depth3D;
};
struct AppTextures{ //Textures those will be used in UI of the app
    //--Icons
    Texture TDModelIcon; 
    Texture softenIcon;  
    Texture smearIcon;
    Texture modifierIcon;
    Texture environmentIcon;
    Texture dropperIcon;
    Texture colorIcon;
    Texture brushIcon;
    Texture threeDIcon;
    Texture twoDIcon;
    Texture exportIcon;
    Texture areaPaintingIcon;
    Texture gearIcon;
    
    //--Textures
    Texture greetingDialogImage;  
};
struct Fonts{ //Fonts those will be used in the app
    Font Arial;
};

#include "GUI/UI.hpp"
#include "GUI/ContextMenu.hpp"
#include "GUI/Node/Node.hpp"
#include "GUI/Node/NodeIO.hpp"
#include "Project.hpp"


class Renderer
{
private:
    void initGLFW(glm::vec2 videoScale){
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
    void initGlad(){
        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }    
    }
public:
    Context context; //Holds data related to the window
    Scene scene; //3D Scene structure
    Model model; //Loaded 3D Model
    Model sphereModel; //Sphere 3D Model to display the materials

    //Structure that holds all the shaders
    //*Define shaders there then init like that shaders.tdModelShader = Shader("a.vert","a.frag");
    Shaders shaders; 

    Mouse mouse; //Everything related to the mouse is done via that class

    Box box; //A class that used render 2D square vertices 

    UI userInterface; 

    //Manage time
    //Commonly used to support the transition effects (to be make sure the transition is done in certain time & not effect by rendering speed)
    Timer timer;    

    //Structure that holds the font classes that will be used by the text renderer (TextRenderer.hpp)
    Fonts fonts;

    //That class is responisble of rendering texts
    //Constructor takes a font class
    //You can change the font in the runtime
    TextRenderer textRenderer; 

    //Textures those will be used in UI of the app
    AppTextures appTextures;

    //0 = materialNode
    //1 = meshOutputNode
    std::vector<Node> appNodes;
    
    std::vector<Node> nodeScene; //Contains app nodes

    Library library;

    ColorPalette colorPalette;

    std::vector<ContextMenu> contextMenus; //0 for textures , 1 for materials, 2 for brushes

    int textureRes = 1024; //Textures will be generated with that resolution

    Project project;

    Painter painter;

    bool VSync = false; //Set glfwSwapInterval(1) if true

    Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size
        //Hold the videoscale value inside of the scene structure
        scene.videoScale = videoScale;
          

        //Initialize the GLFW, create the main window & set callbacks
        initGLFW(videoScale);

        //Get the window size
        glfwGetFramebufferSize(context.window,&context.windowScale.x,&context.windowScale.y);

        initGlad();

        //--Load the app textures
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

        //Init skyboxes
        //TODO : Remove those (maybe)
        shaders.tdModelShader = Shader("LigidPainter/Resources/Shaders/3DModel.vert","LigidPainter/Resources/Shaders/3DModel.frag",nullptr);
        shaders.depth3D = Shader("LigidPainter/Resources/Shaders/3DModel.vert","LigidPainter/Resources/Shaders/Depth3D.frag",nullptr);
        shaders.skyboxShader = Shader("LigidPainter/Resources/Shaders/Skybox.vert","LigidPainter/Resources/Shaders/Skybox.frag",nullptr);
        shaders.prefilteringShader = Shader("LigidPainter/Resources/Shaders/Skybox.vert","LigidPainter/Resources/Shaders/PrefilterSkybox.frag",nullptr);
        shaders.buttonShader = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/UI/Button.frag",nullptr);
        shaders.singleCurve = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/UI/SingleCurve.frag",nullptr);
        shaders.colorPicker = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/UI/ColorPicker.frag",nullptr);
        shaders.twoDPainting = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/2DPainting.frag",nullptr);

        //Update necessary data before callbacks
        updateViewport();
        updateViewMatrix();
        updateProjectionMatrix();

        //Loads the default model (will be removed)
        model.loadModel("./LigidPainter/Resources/3D Models/plane.fbx",true);
        sphereModel.loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true);
        

        //Couldn't use the constructor since the glad is not initialized before definition
        //There is no need to use that function once again
        scene.skybox.init();

        //Create 2D square vertex buffers
        box.init();
        
        //Loads the default skybox
        scene.skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky2");
        scene.skybox.createPrefilterMap(shaders.prefilteringShader,videoScale);
        
        //Load the fonts
        fonts.Arial.loadFont("./LigidPainter/Resources/Fonts/Arial.ttf");

        //Init the text renderer
        textRenderer = TextRenderer(fonts.Arial);

        //Create context menus                                                     0            1               2               3                     4           5  
        contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Rename"  , "Duplicate"       , "Copy Path"   , "Delete"})); //Textures 
        contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Edit"    , "Add To Scene"    , "Rename"      , "Duplicate"   ,    "Copy Path" ,  "Delete"})); //Materials
        contextMenus.push_back(ContextMenu(shaders.buttonShader,colorPalette,{"Use"     ,"Apply Current"    , "Rename"       , "Duplicate"  ,    "Copy Path" ,  "Delete"})); //Brushes

        //Init the userinterface
        userInterface.init(shaders,context,appTextures,videoScale,sphereModel);

        //Init mouse class
        mouse = Mouse(context.window);
        //Load the cursors of the LigidPainter
        mouse.loadCursors();


        //Load the textures
        //*Is a part of the project folder reader
        for (const auto & entry : std::filesystem::directory_iterator("C:/Users/CASPER/Desktop/textures")){
            std::string texturePath = entry.path().string();

            Texture texture;
            texture.load(texturePath.c_str());

            library.textures.push_back(texture);
        }
        //Load the brushes
        //*Is a part of the project folder reader
        for (const auto & entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Brushes")){
            std::string texturePath = entry.path().string();

            Brush brush;
            brush.readFile(texturePath);
            brush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);


            library.brushes.push_back(brush);
        }

        //Create app nodes
        Node materialNode;
        materialNode.loadIO
        (
            {
                NodeIO("Input1",Element(Button(1,glm::vec2(1,8),colorPalette,shaders.buttonShader,"Input1",appTextures.TDModelIcon,2.f,false),1),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale),
            },
            {
                NodeIO("Input1",Element(Button(1,glm::vec2(1,1),colorPalette,shaders.buttonShader,"Input1",appTextures.TDModelIcon,2.f,false),2),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale),
            },
            shaders.buttonShader,
            shaders.singleCurve,
            colorPalette,
            12.f,
            0,
            1
        );

        Node meshOutputNode;
        std::vector <NodeIO> meshOutputNodeInputElements;
        
        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            meshOutputNodeInputElements.push_back(NodeIO(model.meshes[i].materialName,Element(Button(1,glm::vec2(1,1),colorPalette,shaders.buttonShader,model.meshes[i].materialName,Texture(),2.f,false),0),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale));
        }
        
        meshOutputNode.loadIO
        (
            meshOutputNodeInputElements,
            {

            },
            shaders.buttonShader,
            shaders.singleCurve,
            colorPalette,
            25.f,
            0,
            0
        );
        

        //materialNode.loadNode();
        appNodes.push_back(materialNode);
        appNodes.push_back(meshOutputNode);

        nodeScene.push_back(meshOutputNode);

        //Init the painter
        painter.initPainter(videoScale,shaders.twoDPainting,shaders.buttonShader,shaders.tdModelShader,shaders.depth3D);
    }

    void render(){
       glfwPollEvents();
        
        //Update timer data
        if(timer.runTimer())
            std::cout << timer.FPS << std::endl; //Print the fps every second
        
        updateViewport();

        //VSYNC
        if(VSync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
        
        //Refresh the default framebuffer    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Render skybox
        shaders.skyboxShader.use();
        shaders.skyboxShader.setMat4("view",scene.viewMatrix);
        shaders.skyboxShader.setMat4("projection",scene.projectionMatrix);

        shaders.skyboxShader.setInt("skybox",0);
        scene.skybox.draw();

        //Render 3D Model
        shaders.depth3D.use();
        shaders.depth3D.setMat4("view",scene.viewMatrix);
        shaders.depth3D.setMat4("projection",scene.projectionMatrix);

        if(painter.updateTheDepthTexture){
            painter.updateDepthTexture(model);
            painter.updateTheDepthTexture = false;
        }
        
        shaders.tdModelShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP,scene.skybox.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP,scene.skybox.IDPrefiltered);
        
        
        shaders.tdModelShader.setInt("useTransformUniforms",0);
        shaders.tdModelShader.setInt("render2D", 0);
        shaders.tdModelShader.setInt("skybox",0);
        shaders.tdModelShader.setInt("prefilterMap",1);
        shaders.tdModelShader.setInt("albedoTxtr",2);
        shaders.tdModelShader.setInt("roughnessTxtr",3);
        shaders.tdModelShader.setInt("metallicTxtr",4);
        shaders.tdModelShader.setInt("normalMapTxtr",5);
        shaders.tdModelShader.setInt("heightMapTxtr",6);
        shaders.tdModelShader.setInt("ambientOcclusionTxtr",7);
        shaders.tdModelShader.setInt("paintingTexture",8);
        shaders.tdModelShader.setInt("depthTexture",9);
        shaders.tdModelShader.setVec3("viewPos",scene.camera.cameraPos);
        shaders.tdModelShader.setMat4("view",scene.viewMatrix);
        shaders.tdModelShader.setMat4("projection",scene.projectionMatrix);
        glm::mat4 modelMatrix = glm::mat4(1);
        shaders.tdModelShader.setMat4("modelMatrix",modelMatrix);

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D,painter.paintingTexture);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D,painter.depthTexture);

        std::vector<Material> nodeMaterials = getTheMaterialsConnectedToTheMeshNode(nodeScene,library);

        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            if(library.materials.size()){
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].albedo.ID);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].roughness.ID);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].metallic.ID);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].normalMap.ID);
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].heightMap.ID);
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].ambientOcclusion.ID);
            }
            model.meshes[i].Draw();
        }
        

        //model.Draw();

        //Clear the depth buffer before rendering the UI elements (prevent coliding)
        glClear(GL_DEPTH_BUFFER_BIT);

        //Render UI

        //Bind 2D square vertex buffers
        box.bindBuffers();
       
        //Update the UI projection using window size
        userInterface.projection = glm::ortho(0.f,(float)context.windowScale.x,(float)context.windowScale.y,0.f);
        userInterface.render(scene.videoScale,mouse,timer,textRenderer,context,box,library,appNodes,nodeScene,contextMenus,textureRes,project,painter,VSync);//Render the UI



        //Painting
        if(mouse.LPressed){
            painter.doPaint(    
                                mouse,
                                userInterface.paintingPanel.sections[1].elements[0].rangeBar.value,
                                userInterface.paintingPanel.sections[1].elements[2].rangeBar.value/10.f,
                                userInterface.paintingPanel.sections[1].elements[1].rangeBar.value/100.f,
                                userInterface.paintingPanel.sections[1].elements[3].rangeBar.value,
                                1.f - userInterface.paintingPanel.sections[2].elements[0].rangeBar.value/100.f,
                                1.f - userInterface.paintingPanel.sections[2].elements[3].rangeBar.value/100.f,
                                1.f - userInterface.paintingPanel.sections[2].elements[1].rangeBar.value/100.f,
                                userInterface.paintingPanel.sections[2].elements[7].rangeBar.value,
                                1.f - userInterface.paintingPanel.sections[2].elements[8].rangeBar.value/100.f,
                                1.f - userInterface.paintingPanel.sections[2].elements[9].rangeBar.value/100.f,
                                userInterface.paintingPanel.sections[2].elements[6].checkBox.clickState1,
                                userInterface.paintingPanel.sections[2].elements[2].checkBox.clickState1,
                                library.textures
                            );

            
        }

        if((painter.refreshable && !mouse.LPressed) || mouse.RClick || mouse.MClick){
            //TODO Prevent updating all the materials
            for (size_t i = 0; i < library.materials.size(); i++)
            {
                userInterface.materialEditorDialog.updateMaterial(library.materials[i],textureRes,box,context);
            }
            
            painter.updateTexture(library.textures,model,textureRes);
            painter.refreshPainting();
            painter.refreshable = false;
        }

        box.unbindBuffers(); //Finish rendering the UI

        //Set mouse states to default
        mouse.LClick = false;
        mouse.RClick = false;
        mouse.MClick = false;
        mouse.LDoubleClick = false;
        mouse.mouseOffset = glm::vec2(0);
        mouse.mods = 0;

        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;


        //Cursor is changing there
        //Sets the active cursor (mouse.activeCursor) as the cursor
        //Than changes the active cursor as default cursor
        mouse.updateCursor();  


        glfwSwapBuffers(context.window);
        
    }



private:
    std::vector<Material> getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &nodeScene,Library &library){
        std::vector<Material> materials;

        //Check all the inputs of the mesh node
        for (size_t i = 0; i < nodeScene[0].inputs.size(); i++)
        {
            int materialID = 1000;
            if(nodeScene[0].inputs[i].connections.size())
                materialID = nodeScene[nodeScene[0].inputs[i].connections[0].nodeIndex].materialID;
            else
                materials.push_back(Material());

            if(materialID != 1000){
                for (size_t matI = 0; matI < library.materials.size(); matI++)
                {
                    if(library.materials[matI].ID == materialID)
                        materials.push_back(library.materials[matI]);
                }
            }
            
        }
        return materials;
    }
    void updateViewMatrix(){
        scene.viewMatrix = glm::lookAt(scene.camera.cameraPos, 
                                       scene.camera.originPos, 
                                       glm::vec3(0.0, 1.0, 0.0));
    }
    void updateProjectionMatrix(){
        scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), 
                                         (float)context.windowScale.x / (float)context.windowScale.y, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                         scene.near, 
                                         scene.far);
    }
    void updateViewport(){
        glViewport(0, 
                   0, 
                   context.windowScale.x, 
                   context.windowScale.y);
    }

    double previousClickTime = 0.0;
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double currentTime = glfwGetTime();
            double timeSinceLastClick = currentTime - previousClickTime;

            if (timeSinceLastClick < 0.3) {
                mouse.LDoubleClick = true;
            }
        
            previousClickTime = currentTime;
        }
        
        if(button == 0){ //Left
            if(action == 1)
                mouse.LClick = true;
        }  
        if(button == 1){ //Right
            if(action == 1)
                mouse.RClick = true;
        }  
        if(button == 2){ //Mid
            if(action == 1)
                mouse.MClick = true;
        }  

        
        if(button == 0){ //Left
            mouse.LPressed = action;
        }  
        if(button == 1){ //Right
            mouse.RPressed = action;
        }  
        if(button == 2){ //Mid
            mouse.MPressed = action;
        }  
        

        mouse.mods = mods;
    }
    
    void framebufferSizeCallback(GLFWwindow* window, int width, int height){
        //Goes the global variable
        context.windowScale.x = width;
        context.windowScale.y = height;
        updateProjectionMatrix(); //Since the 3D Model is effected by window resizing we update the projection matrix 
        updateViewport(); //And ofc the OpenGL viewport
    }

    glm::vec2 lastMousePos;//This will be used as "last frame's cursor pos" for the cursor offset
    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
        
        float originCameraDistance = glm::distance(scene.camera.originPos,scene.camera.cameraPos)/10;
	    
        if (yoffset > 0 && scene.camera.radius > 1) {
	    	scene.camera.radius -= originCameraDistance;
	    }
	    else if (yoffset < 0) {
	    	scene.camera.radius += originCameraDistance;
	    }
	    //Zoom in-out
	    scene.camera.cameraPos.x = cos(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.x;
	    scene.camera.cameraPos.y = sin(glm::radians(scene.camera.pitch)) * -scene.camera.radius + scene.camera.originPos.y;
	    scene.camera.cameraPos.z = sin(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.z;

        updateViewMatrix();

        painter.updateTheDepthTexture = true;
    }
    
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        //Take the cursor position data to a global variable
        mouse.cursorPos.x = xpos;
        mouse.cursorPos.y = ypos;
        
        //Get the mouse offset by subtracting current cursor position from last frame's cursor pos
        mouse.mouseOffset.x = mouse.cursorPos.x - lastMousePos.x;
    	mouse.mouseOffset.y = mouse.cursorPos.y - lastMousePos.y;

        const float sensitivity = 0.2f; //Mouse sensivity (Increase the value to go brrrrrbrbrbrb)

    	if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { //L Control + L Click
    		//Straight Movement
            //Rotates the Camera in 3 axis using mouse offset & intepreting yaw, pitch & radius values
            const float half_sensitivity = sensitivity / 2.0f;
            const float sin_yaw = sin(glm::radians(scene.camera.yaw));
            const float cos_yaw = cos(glm::radians(scene.camera.yaw));
            const float sin_pitch = sin(glm::radians(scene.camera.pitch));
            const float cos_pitch = cos(glm::radians(scene.camera.pitch));

            // Straight Movement
            float x_offset = sin_yaw * mouse.mouseOffset.x * sensitivity * half_sensitivity;
            float z_offset = cos_yaw * mouse.mouseOffset.x * sensitivity * half_sensitivity;
            
            if(scene.camera.pitch > 60.0f || scene.camera.pitch < -60.0f){
                x_offset += cos_yaw * sin_pitch * mouse.mouseOffset.y * sensitivity * half_sensitivity;
                z_offset -= sin_yaw * sin_pitch * mouse.mouseOffset.y * sensitivity * half_sensitivity;
            }
            
            const float y_offset = cos_pitch * mouse.mouseOffset.y * sensitivity * half_sensitivity;

            scene.camera.cameraPos.x -= x_offset;
            scene.camera.originPos.x -= x_offset;

            scene.camera.cameraPos.z += z_offset;
            scene.camera.originPos.z += z_offset;

            scene.camera.cameraPos.y += y_offset;
            scene.camera.originPos.y += y_offset;

            painter.updateTheDepthTexture = true;
    	}
    	else if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS)) { //L Click
    	    scene.camera.yaw += mouse.mouseOffset.x * sensitivity;
    		scene.camera.pitch -= mouse.mouseOffset.y * sensitivity;

    		//Disable 90+ degrees rotations in y axis
    		if (scene.camera.pitch > 89.0f)
    			scene.camera.pitch = 89.0f;
    		if (scene.camera.pitch < -89.0f)
    			scene.camera.pitch = -89.0f;
    
    		//Helical Movement
            //Rotates the Camera in 3 axis using yaw, pitch & radius values
    		scene.camera.cameraPos.y = sin(glm::radians(scene.camera.pitch)) * - scene.camera.radius + scene.camera.originPos.y;
    		scene.camera.cameraPos.x = cos(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.x;
    		scene.camera.cameraPos.z = sin(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.z;
         
            painter.updateTheDepthTexture = true;
    	}
        //Update the view matrix so we can see the changes
        updateViewMatrix();
        
        //This will be used as "last frame's cursor pos" for the cursor offset
        lastMousePos.x = mouse.cursorPos.x;
        lastMousePos.y = mouse.cursorPos.y;
    }
    void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods)
    {
        if(action == GLFW_PRESS || action == GLFW_REPEAT){ //1 or 2
            textRenderer.keyInput = true;
            textRenderer.key = key;
            textRenderer.mods = mods;
        }
    }

};
#endif