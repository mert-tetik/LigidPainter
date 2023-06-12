/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Manage websites & open a website with default browser

*/

#include "UTIL/Util.hpp"

#include <iostream>
#include <cstdlib>

Website::Website(){}

Website::Website(std::string url){
    this->url = url;
}

void Website::open(){
    std::cout << url << std::endl;

    #if defined(_WIN32) || defined(_WIN64)
    //Windows
    std::string command = "start " + url;
    std::system(command.c_str());

    #elif defined(__APPLE__)
    //Apple device (macos OS)
    std::string command = "open " + url;
    std::system(command.c_str());
    
    #elif defined(__linux__)
    //Linux
    std::string command = "xdg-open " + url;
    std::system(command.c_str());
    
    #else
    //Unknown OS  
    std::cout << "ERROR : Can't open website. User-Unknown OS. (Switch to Windows / Linux / MacOS to solve the problem)"
    return;
    #endif
}

std::string Website::getURL(){
    return url;
}