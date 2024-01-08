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
#define IS_CLICKED(key) getContext()->window.isKeyClicked(key)  

bool Shortcuts::CTRL_Z(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_Z) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_X(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_X) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_S(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_S) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_N(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_N) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_O(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_O) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_P(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_P) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_Y(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_Y) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_C(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_C) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_V(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_V) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_A(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_A) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_F(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_F) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_G(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_G) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_W(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_W) && !IS_PRESSED(SHIFT));
}
bool Shortcuts::CTRL_SHIFT_S(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(SHIFT) && IS_CLICKED(LIGIDGL_KEY_S));
}
bool Shortcuts::F1(){
    return (IS_CLICKED(LIGIDGL_KEY_F1));
}
bool Shortcuts::F2(){
    return (IS_CLICKED(LIGIDGL_KEY_F2));
}
bool Shortcuts::F3(){
    return (IS_CLICKED(LIGIDGL_KEY_F3));
}
bool Shortcuts::F4(){
    return (IS_CLICKED(LIGIDGL_KEY_F4));
}
bool Shortcuts::F5(){
    return (IS_CLICKED(LIGIDGL_KEY_F5));
}
bool Shortcuts::F6(){
    return (IS_CLICKED(LIGIDGL_KEY_F6));
}
bool Shortcuts::F7(){
    return (IS_CLICKED(LIGIDGL_KEY_F7));
}
bool Shortcuts::F8(){
    return (IS_CLICKED(LIGIDGL_KEY_F8));
}
bool Shortcuts::F9(){
    return (IS_CLICKED(LIGIDGL_KEY_F9));
}
bool Shortcuts::F10(){
    return (IS_CLICKED(LIGIDGL_KEY_F10));
}
bool Shortcuts::F11(){
    return (IS_CLICKED(LIGIDGL_KEY_F11));
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
            Shortcuts::F1() ||
            Shortcuts::F2() ||
            Shortcuts::F3() ||
            Shortcuts::F4() ||
            Shortcuts::F5() ||
            Shortcuts::F6() ||
            Shortcuts::F7() ||
            Shortcuts::F8() ||
            Shortcuts::F9() ||
            Shortcuts::F10() ||
            Shortcuts::F11();
}