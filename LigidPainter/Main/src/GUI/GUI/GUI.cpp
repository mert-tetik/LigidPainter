/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Rendering UI Elements

Currently all the UI elements are rendered to the default framebuffer.
Before rendering them bind 2D square vertex buffer.

Example :
    Box box();
    box.init();

    while(){
        box.bindBuffers();

        ..Render UI 

        box.unbindBuffers();
    }

    Define UI Elements inside of the UI class
    Render them inside of the render function of the UI class

    Position of the vertex is determined that way:
        monitor size / 100 * pos val

    *A panel is consists of sections
    *Sections consists of elements and a header
    *Elements consists of buttons, checkboxes, slide bars and more

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

Element::Element(){}

Element::Element(Button button){
    //Init as button
    this->button = button;
    this->pos = button.pos;
    this->scale = button.scale;
    this->panelOffset = button.panelOffset;
    this->state = 0;
}
Element::Element(RangeBar rangeBar){
    
    //Init as range bar
    this->rangeBar = rangeBar;
    pos = rangeBar.pos;
    scale = rangeBar.scale;
    panelOffset = rangeBar.panelOffset;
    state = 1;
}
Element::Element(CheckBox checkBox){
    //Init as range bar
    this->checkBox = checkBox;
    pos = checkBox.pos;
    scale = checkBox.scale;
    panelOffset = checkBox.panelOffset;
    state = 2;
}
Element::Element(ComboBox comboBox){
    //Init as combo box
    this->comboBox = comboBox;
    pos = comboBox.pos;
    scale = comboBox.scale;
    panelOffset = comboBox.panelOffset;
    state = 3;
}
Element::Element(TextBox textBox){
    //Init as text box
    this->textBox = textBox;
    pos = textBox.pos;
    scale = textBox.scale;
    panelOffset = textBox.panelOffset;
    state = 4;
}

void Element::render(Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    if(state == 0){ //Render the button
        button.pos = pos;
        button.scale = scale;
        button.panelOffset = panelOffset;
        button.doMouseTracking = doMouseTracking;
        button.render(timer,textRenderer,doMouseTracking);
    }
    if(state == 1){ //Render the rangeBar
        rangeBar.pos = pos;
        rangeBar.scale = scale;
        rangeBar.panelOffset = panelOffset;
        rangeBar.doMouseTracking = doMouseTracking;
        rangeBar.render(timer,textRenderer,doMouseTracking);
    }
    if(state == 2){ //Render the checkBox
        checkBox.pos = pos;
        checkBox.scale = scale;
        checkBox.panelOffset = panelOffset;
        checkBox.doMouseTracking = doMouseTracking;
        checkBox.render(timer,textRenderer,doMouseTracking);
    }
    if(state == 3){ //Render the comboxBox
        comboBox.pos = pos;
        comboBox.scale = scale;
        comboBox.panelOffset = panelOffset;
        comboBox.doMouseTracking = doMouseTracking;
        comboBox.render(timer,textRenderer,doMouseTracking);
    }
    if(state == 4){ //Render the textbox
        textBox.pos = pos;
        textBox.scale = scale;
        textBox.panelOffset = panelOffset;
        textBox.doMouseTracking = doMouseTracking;
        textBox.render(timer,textRenderer,doMouseTracking);
    }
}

Section::Section(){}
Section::Section(Element header,std::vector<Element> elements){
    this->header = header;
    this->elements = elements;
}

UI::UI(){} 