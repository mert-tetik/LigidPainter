    /*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Box.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements.
*/


#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
    
bool Shortcuts::CTRL_Z(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_Z) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_X(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_X) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_S(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_S) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_N(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_N) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_O(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_O) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_P(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_P) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_Y(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_Y) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_C(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_C) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_V(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_V) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_A(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_A) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_F(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_F) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_G(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_G) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_W(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_W) && textRenderer.keyInput);
}
bool Shortcuts::CTRL_SHIFT_S(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_S) && textRenderer.keyInput);
}
bool Shortcuts::F11(){
    return (getContext()->window.isKeyPressed(LIGIDGL_KEY_F11) && textRenderer.keyInput);
}