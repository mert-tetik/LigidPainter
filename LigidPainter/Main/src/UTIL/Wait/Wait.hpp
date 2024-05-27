/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef WAIT_HPP
#define WAIT_HPP

#include "GUI/Dialogs.hpp"

extern DialogControl __wait_dialog_control;

#define WAIT_WHILE(condition)   {\
                                    int i = 0;\
                                    while(condition){\
                                        if(i == 0)\
                                            __wait_dialog_control.activate();\
                                        __wait_dialog_control.updateStart(true);\
                                        appVideos.loading.render(*getTimer(), glm::vec3(50.f, 50.f, 0.7f), glm::vec2(5.f) / glm::vec2(Settings::videoScale()->x / Settings::videoScale()->y, 1.f), 1.f, 1, true);\
                                        __wait_dialog_control.updateEnd(*getTimer(), 0.3f);\
                                        i++;\
                                    }\
                                }


#endif