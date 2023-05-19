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
    glm::vec2 mousePos;
    glm::vec2 mouseOffset;
    glm::vec2 windowScale;
};

class Renderer
{
private:
    /* data */
public:
    Context context; //Holds data related to the window
    Scene scene; //3D Scene structure
    Model model; //Loaded 3D Model

    Shader tdModelShader;
    Shader skyboxShader;

    Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size

        //Hold the videoscale value inside of the scene structure
        scene.videoScale = videoScale;
        
        //Since the window size is determined by the videoscale value there is no harm to do that
        context.windowScale = scene.videoScale;  

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




        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }    



        //Load the 3D Model shader 
        //TODO : Remove that (maybe)
        tdModelShader = Shader("LigidPainter/Resources/Shaders/3DModel.vert","LigidPainter/Resources/Shaders/3DModel.frag",nullptr);
        skyboxShader = Shader("LigidPainter/Resources/Shaders/Skybox.vert","LigidPainter/Resources/Shaders/Skybox.frag",nullptr);

        //Update necessary data before callbacks
        updateViewport();
        updateViewMatrix();
        updateProjectionMatrix();

        //Loads the default model (will be removed)
        model.loadModel("./LigidPainter/Resources/3D Models/untitled.fbx",true);
        
        //Couldn't use the constructor since the glad is not initialized before definition
        //There is no need to use that function once again
        scene.skybox.init();
        
        //Loads the default skybox
        scene.skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky2");
    }

    void render(){
        glfwPollEvents();
        
        //Refresh the default framebuffer    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Render skybox
        skyboxShader.use();
        skyboxShader.setMat4("view",scene.viewMatrix);
        skyboxShader.setMat4("projection",scene.projectionMatrix);
        skyboxShader.setInt("skybox",0);
        scene.skybox.draw();

        //Render 3D Model
        tdModelShader.use();
        tdModelShader.setMat4("view",scene.viewMatrix);
        tdModelShader.setMat4("projection",scene.projectionMatrix);
        glm::mat4 modelMatrix = glm::mat4(1);
        tdModelShader.setMat4("modelMatrix",modelMatrix);
        model.Draw();

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
        context.mousePos.x = xpos;
        context.mousePos.y = ypos;
        
        //Get the mouse offset by subtracting current cursor position from last frame's cursor pos
        context.mouseOffset.x = context.mousePos.x - lastMousePos.x;
    	context.mouseOffset.y = context.mousePos.y - lastMousePos.y;

        const float sensitivity = 0.2f; //Mouse sensivity (Increase the value to go brrrrrbrbrbrb)

    	if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { //L Control + L Click
    		//Straight Movement
            //Rotates the Camera in 3 axis using mouse offset & intepreting yaw, pitch & radius values
            scene.camera.cameraPos.x -= sin(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * sensitivity * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;
    		scene.camera.originPos.x -= sin(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * sensitivity * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;

    		scene.camera.cameraPos.z += cos(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * sensitivity * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;
    		scene.camera.originPos.z += cos(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * sensitivity * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;

    		scene.camera.cameraPos.y -= cos(glm::radians(scene.camera.pitch)) * context.mouseOffset.y * sensitivity * (sensitivity / 2);
    		scene.camera.originPos.y -= cos(glm::radians(scene.camera.pitch)) * context.mouseOffset.y * sensitivity * (sensitivity / 2);
    	}
    	else if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS)) { //L Click
    	    scene.camera.yaw += context.mouseOffset.x * sensitivity;
    		scene.camera.pitch += context.mouseOffset.y * sensitivity;

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
        lastMousePos.x = context.mousePos.x;
        lastMousePos.y = context.mousePos.y;
    }

};
#endif