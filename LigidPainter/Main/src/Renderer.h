#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "Model.hpp"

class Renderer
{
private:
    /* data */
public:
    GLFWwindow* window;
    Model model;

    Renderer(glm::vec2 videoScale){
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        window = glfwCreateWindow(videoScale.x, videoScale.y, "LigidPainter", NULL, NULL);
        
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }    

        glViewport(0, 0, videoScale.x, videoScale.y);
    }

    void render(){
        glfwPollEvents();    

        model.Draw();

        glfwSwapBuffers(window);
    }
};