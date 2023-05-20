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

//GL_TEXTURE0 is used for random stuff - bind as default

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "Model.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "Box.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"

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
struct Context{
    GLFWwindow* window;
    glm::vec2 windowScale;
};

struct Shaders{
    Shader tdModelShader;
    Shader skyboxShader;
    Shader buttonShader;
};

#include "GUI/UI.hpp"

class Renderer
{
private:
    void initGLFW(glm::vec2 videoScale){
        //Init GLFW
        glfwInit();
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

    //Structure that holds all the shaders
    //*Define shaders there then init like that shaders.tdModelShader = Shader("a.vert","a.frag");
    Shaders shaders; 

    Mouse mouse; //Everything related to the mouse is done via that class

    Box box; //A class that used render 2D square vertices 

    UI userInterface; 

    //Manage time
    //Commonly used to support the transition effects (to be make sure the transition is done in certain time & not effect by rendering speed)
    Timer timer;    

    Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size
        //Hold the videoscale value inside of the scene structure
        scene.videoScale = videoScale;
        
        //Since the window size is determined by the videoscale value there is no harm to do that
        context.windowScale = scene.videoScale;  

        //Initialize the GLFW, create the main window & set callbacks
        initGLFW(videoScale);

        initGlad();

        //Init skyboxes
        //TODO : Remove those (maybe)
        shaders.tdModelShader = Shader("LigidPainter/Resources/Shaders/3DModel.vert","LigidPainter/Resources/Shaders/3DModel.frag",nullptr);
        shaders.skyboxShader = Shader("LigidPainter/Resources/Shaders/Skybox.vert","LigidPainter/Resources/Shaders/Skybox.frag",nullptr);
        shaders.buttonShader = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/UI/Button.frag",nullptr);

        //Update necessary data before callbacks
        updateViewport();
        updateViewMatrix();
        updateProjectionMatrix();

        //Loads the default model (will be removed)
        model.loadModel("./LigidPainter/Resources/3D Models/untitled.fbx",true);
        
        //Couldn't use the constructor since the glad is not initialized before definition
        //There is no need to use that function once again
        scene.skybox.init();

        //Create 2D square vertex buffers
        box.init();
        
        //Loads the default skybox
        scene.skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky2");
        
        //Init the userinterface
        userInterface.init(shaders,context);
    }

    void render(){
        glfwPollEvents();
        
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
        shaders.tdModelShader.use();
        shaders.tdModelShader.setMat4("view",scene.viewMatrix);
        shaders.tdModelShader.setMat4("projection",scene.projectionMatrix);
        glm::mat4 modelMatrix = glm::mat4(1);
        shaders.tdModelShader.setMat4("modelMatrix",modelMatrix);
        model.Draw();

        //Clear the depth buffer before rendering the UI elements (prevent coliding)
        glClear(GL_DEPTH_BUFFER_BIT);

        //Render UI

        //Bind 2D square vertex buffers
        box.bindBuffers();
       
        //Update the UI projection using window size
        userInterface.projection = glm::ortho(0.f,context.windowScale.x,context.windowScale.y,0.f);
        userInterface.render(scene.videoScale,mouse);//Render the UI

        box.unbindBuffers(); //Finish rendering the UI



        //Set mouse states to default
        mouse.LClick = false;
        mouse.RClick = false;
        mouse.MClick = false;
        mouse.mouseOffset = glm::vec2(0);
        mouse.mods = 0;


        glfwSwapBuffers(context.window);
    }



private:
    void updateViewMatrix(){
        scene.viewMatrix = glm::lookAt(scene.camera.cameraPos, 
                                       scene.camera.originPos, 
                                       glm::vec3(0.0, 1.0, 0.0));
    }
    void updateProjectionMatrix(){
        scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), 
                                         context.windowScale.x / context.windowScale.y, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                         scene.near, 
                                         scene.far);
    }
    void updateViewport(){
        glViewport(0, 
                   0, 
                   context.windowScale.x, 
                   context.windowScale.y);
    }


    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        if(button == 0){ //Left
            if(action == 1)
                mouse.LClick = true;
        }  
        if(button == 1){ //Right
            if(action == 1)
                mouse.LClick = true;
        }  
        if(button == 2){ //Mid
            if(action == 1)
                mouse.LClick = true;
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
            scene.camera.cameraPos.x -= sin(glm::radians(scene.camera.yaw)) * mouse.mouseOffset.x * sensitivity * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y) / 4;
    		scene.camera.originPos.x -= sin(glm::radians(scene.camera.yaw)) * mouse.mouseOffset.x * sensitivity * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y) / 4;

    		scene.camera.cameraPos.z += cos(glm::radians(scene.camera.yaw)) * mouse.mouseOffset.x * sensitivity * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y) / 4;
    		scene.camera.originPos.z += cos(glm::radians(scene.camera.yaw)) * mouse.mouseOffset.x * sensitivity * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y) / 4;

    		scene.camera.cameraPos.y -= cos(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y * sensitivity * (sensitivity / 2);
    		scene.camera.originPos.y -= cos(glm::radians(scene.camera.pitch)) * mouse.mouseOffset.y * sensitivity * (sensitivity / 2);
    	}
    	else if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS)) { //L Click
    	    scene.camera.yaw += mouse.mouseOffset.x * sensitivity;
    		scene.camera.pitch += mouse.mouseOffset.y * sensitivity;

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
    	}
        //Update the view matrix so we can see the changes
        updateViewMatrix();
        
        //This will be used as "last frame's cursor pos" for the cursor offset
        lastMousePos.x = mouse.cursorPos.x;
        lastMousePos.y = mouse.cursorPos.y;
    }

};
#endif