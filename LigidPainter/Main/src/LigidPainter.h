#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "Renderer.h"

class LigidPainter{
public:
    int run(){
        Renderer renderer(glm::vec2(1920,1080));
        while(!glfwWindowShouldClose(renderer.context.window))
        {   
            renderer.render();
        }
        
        glfwTerminate();
        return 1;
    }
};