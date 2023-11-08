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
    
#define CTRL LIGIDGL_KEY_LEFT_CONTROL
#define SHIFT LIGIDGL_KEY_LEFT_SHIFT

#define IS_PRESSED(key) getContext()->window.isKeyPressed(key)  

bool Shortcuts::CTRL_Z(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_Z) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_X(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_X) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_S(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_S) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_N(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_N) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_O(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_O) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_P(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_P) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_Y(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_Y) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_C(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_C) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_V(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_V) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_A(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_A) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_F(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_F) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_G(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_G) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_W(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(LIGIDGL_KEY_W) && textRenderer.keyInput && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_SHIFT_S(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(SHIFT) && IS_PRESSED(LIGIDGL_KEY_S) && textRenderer.keyInput);
}
bool Shortcuts::F11(){
    return (IS_PRESSED(LIGIDGL_KEY_F11) && textRenderer.keyInput);
}

bool Shortcuts::anyShortCut(){
    return Shortcuts::CTRL_Z()||
            Shortcuts::CTRL_X()||
            Shortcuts::CTRL_S()||
            Shortcuts::CTRL_N()||
            Shortcuts::CTRL_O()||
            Shortcuts::CTRL_P()||
            Shortcuts::CTRL_Y()||
            Shortcuts::CTRL_C()||
            Shortcuts::CTRL_V()||
            Shortcuts::CTRL_A()||
            Shortcuts::CTRL_F()||
            Shortcuts::CTRL_G()||
            Shortcuts::CTRL_W()||
            Shortcuts::CTRL_SHIFT_S()||
            Shortcuts::F11();
}