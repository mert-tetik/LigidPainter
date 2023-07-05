/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    See the ./README.md file for the description

*/

#ifndef LIGIDWINDOW_HPP
#define LIGIDWINDOW_HPP 

#include<string>

class LigidWindow
{
private:
    /* data */
public:
    LigidWindow(/* args */){}

    void createWindow(int w, int h, std::string title);
};

#endif