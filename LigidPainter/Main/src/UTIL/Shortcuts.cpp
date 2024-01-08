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

bool shortcuts_CTRL_Z(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_Z) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_X(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_X) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_S(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_S) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_N(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_N) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_O(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_O) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_P(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_P) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_Y(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_Y) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_C(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_C) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_V(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_V) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_A(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_A) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_F(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_F) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_G(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_G) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_W(){
    return (IS_PRESSED(CTRL) && IS_CLICKED(LIGIDGL_KEY_W) && !IS_PRESSED(SHIFT));
}
bool shortcuts_CTRL_SHIFT_S(){
    return (IS_PRESSED(CTRL) && IS_PRESSED(SHIFT) && IS_CLICKED(LIGIDGL_KEY_S));
}
bool shortcuts_F1(){
    return (IS_CLICKED(LIGIDGL_KEY_F1));
}
bool shortcuts_F2(){
    return (IS_CLICKED(LIGIDGL_KEY_F2));
}
bool shortcuts_F3(){
    return (IS_CLICKED(LIGIDGL_KEY_F3));
}
bool shortcuts_F4(){
    return (IS_CLICKED(LIGIDGL_KEY_F4));
}
bool shortcuts_F5(){
    return (IS_CLICKED(LIGIDGL_KEY_F5));
}
bool shortcuts_F6(){
    return (IS_CLICKED(LIGIDGL_KEY_F6));
}
bool shortcuts_F7(){
    return (IS_CLICKED(LIGIDGL_KEY_F7));
}
bool shortcuts_F8(){
    return (IS_CLICKED(LIGIDGL_KEY_F8));
}
bool shortcuts_F9(){
    return (IS_CLICKED(LIGIDGL_KEY_F9));
}
bool shortcuts_F10(){
    return (IS_CLICKED(LIGIDGL_KEY_F10));
}
bool shortcuts_F11(){
    return (IS_CLICKED(LIGIDGL_KEY_F11));
}

bool shortcuts_anyShortCut(){
    return shortcuts_CTRL_Z()||
            shortcuts_CTRL_X()||
            shortcuts_CTRL_S()||
            shortcuts_CTRL_N()||
            shortcuts_CTRL_O()||
            shortcuts_CTRL_P()||
            shortcuts_CTRL_Y()||
            shortcuts_CTRL_C()||
            shortcuts_CTRL_V()||
            shortcuts_CTRL_A()||
            shortcuts_CTRL_F()||
            shortcuts_CTRL_G()||
            shortcuts_CTRL_W()||
            shortcuts_CTRL_SHIFT_S()||
            shortcuts_F1() ||
            shortcuts_F2() ||
            shortcuts_F3() ||
            shortcuts_F4() ||
            shortcuts_F5() ||
            shortcuts_F6() ||
            shortcuts_F7() ||
            shortcuts_F8() ||
            shortcuts_F9() ||
            shortcuts_F10() ||
            shortcuts_F11();
}