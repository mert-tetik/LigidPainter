/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGDRENDERER_HPP
#define LGDRENDERER_HPP

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

class Renderer
{
public:

    Fonts fonts;

    Renderer();

    void initRenderer();

    void render();
private:
    /*!
        @brief Let the getScene()->model->newModelAdded be true for an another cycle
                Ignore pls
    */
    bool previousModelNewModelAdded = false;

    double previousClickTime = 0.0;
    
    // Callbacks
    void mouseButtonCallback(LigidWindow window, int button, int action, int mods);
    void framebufferSizeCallback(LigidWindow window, int width, int height);
    void scrollCallback(LigidWindow window, double xoffset, double yoffset);
    void cursorPositionCallback(LigidWindow window, double xpos, double ypos);
    void keyCallback(LigidWindow window, int key,int scancode,int action,int mods);

    void start_render();
    void end_render();
};
#endif