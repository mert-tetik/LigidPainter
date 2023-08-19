/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
    /*!
        @brief Render 2D square 
    */
    Box box; 

    /*!
        @brief The GUI
    */
    UI userInterface; 

    /*!
        @brief Manage time
    */
    Timer timer;    

    Fonts fonts;

    Websites websites;

    TextRenderer textRenderer; 

    ColorPalette colorPalette;

    std::vector<ContextMenu> contextMenus; //0 for textures , 1 for materials, 2 for brushes

    Project project;

    Skybox skybox;

    Painter painter;

    //Constructor
    Renderer();

    void render();
private:
    /*!
        @brief Let the getModel()->newModelAdded be true for an another cycle
                Ignore pls
    */
    bool previousModelNewModelAdded = false;

    void initGlad();
    void loadAppTextures();
    void createContextMenus();

    void renderSkyBox();
    void set3DUniforms();

    double previousClickTime = 0.0;
    glm::vec2 lastMousePos;//This will be used as "last frame's cursor pos" for the cursor offset 
    
    void mouseButtonCallback(LigidWindow window, int button, int action, int mods);
    void framebufferSizeCallback(LigidWindow window, int width, int height);
    void scrollCallback(LigidWindow window, double xoffset, double yoffset);
    void cursorPositionCallback(LigidWindow window, double xpos, double ypos);
    void keyCallback(LigidWindow window, int key,int scancode,int action,int mods);

};
#endif