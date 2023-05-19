#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "Model.hpp"
#include "Shader.hpp"

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

    Renderer(glm::vec2 videoScale){//Videoscale is the resolution value that will be used for viewport & window size
        //Hold the videoscale value inside of the scene structure
        scene.videoScale = videoScale;

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
        //Pointing a function that is a class member
        glfwMakeContextCurrent(context.window);
        //Cursor position callback function casting
        glfwSetWindowUserPointer(context.window, this);
        auto cursorPosFunc = [](GLFWwindow* w, double x, double y)
        {
            static_cast<Renderer*>(glfwGetWindowUserPointer(w))->cursor_position_callback(w,x,y);
        };
        glfwSetCursorPosCallback(context.window, cursorPosFunc);



        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }    

        //Load the 3D Model shader 
        //TODO : Remove that (maybe)
        tdModelShader = Shader("LigidPainter/Resources/Shaders/3DModel.vert","LigidPainter/Resources/Shaders/3DModel.frag",nullptr);

        //Update necessary data before callbacks
        updateViewport();
        updateViewMatrix();
        updateProjectionMatrix();

        //Loads the default model (will be removed)
        model.loadModel("./LigidPainter/Resources/3D Models/untitled.fbx",true);
    }

    void render(){
        glfwPollEvents();    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tdModelShader.setMat4("view",scene.viewMatrix);
        tdModelShader.setMat4("projection",scene.projectionMatrix);
        glm::mat4 modelMatrix = glm::mat4(1);
        tdModelShader.setMat4("modelMatrix",modelMatrix);

        tdModelShader.use();
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
                                         scene.videoScale.x / scene.videoScale.y, 
                                         scene.near, 
                                         scene.far);
    }
    void updateViewport(){
        glViewport(0, 
                   0, 
                   scene.videoScale.x, 
                   scene.videoScale.y);
    }
    glm::vec2 lastMousePos;
    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        context.mousePos.x = xpos;
        context.mousePos.y = ypos;
        
        context.mouseOffset.x = context.mousePos.x - lastMousePos.x;
    	context.mouseOffset.y = context.mousePos.y - lastMousePos.y;

        const float sensitivity = 0.2f;

    	if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS) && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    		context.mouseOffset.x *= sensitivity;
    		context.mouseOffset.y *= sensitivity;

    		//Straight Movement
    		scene.camera.cameraPos.x -= sin(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;
    		scene.camera.originPos.x -= sin(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * (sensitivity / 2) + cos(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;

    		scene.camera.cameraPos.z += cos(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;
    		scene.camera.originPos.z += cos(glm::radians(scene.camera.yaw)) * context.mouseOffset.x * (sensitivity / 2) - sin(glm::radians(scene.camera.yaw)) * (sin(glm::radians(scene.camera.pitch)) * context.mouseOffset.y) / 4;

    		scene.camera.cameraPos.y -= cos(glm::radians(scene.camera.pitch)) * context.mouseOffset.y * (sensitivity / 2);
    		scene.camera.originPos.y -= cos(glm::radians(scene.camera.pitch)) * context.mouseOffset.y * (sensitivity / 2);
    	}
    	else if ((glfwGetMouseButton(context.window, 1) == GLFW_PRESS)) {

    		context.mouseOffset.x *= sensitivity;
    		context.mouseOffset.y *= sensitivity;

    	    scene.camera.yaw += context.mouseOffset.x;
    		scene.camera.pitch += context.mouseOffset.y;

    		//Disable 90+ degrees rotations in y axis
    		// if (pitch > 89.0f)
    		// 	pitch = 89.0f;
    		// if (pitch < -89.0f)
    		// 	pitch = -89.0f;

    
    		//Helical Movement
    		scene.camera.cameraPos.y = sin(glm::radians(scene.camera.pitch)) * - scene.camera.radius + scene.camera.originPos.y;
    		scene.camera.cameraPos.x = cos(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.x;
    		scene.camera.cameraPos.z = sin(glm::radians(scene.camera.yaw)) * cos(glm::radians(scene.camera.pitch)) * scene.camera.radius + scene.camera.originPos.z;
    	}
        updateViewMatrix();
        
        lastMousePos.x = context.mousePos.x;
        lastMousePos.y = context.mousePos.y;
    }

};