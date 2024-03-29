/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Mouse/Mouse.hpp"

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

Element::Element(SectionHolder sectionHolder){

    //Init as section holder
    this->sectionHolder = sectionHolder;
    panelOffset = sectionHolder.panelOffset;
    scale.y = sectionHolder.scaleY;
    state = 5;
}

Element::Element(Gizmo gizmo){

    //Init as gizmo
    this->gizmo = gizmo;
    panelOffset = gizmo.panelOffset;
    pos = gizmo.pos;
    scale = gizmo.scale;
    state = 6;
}

Element::Element(PainterColorSelection painterColorSelection){
    //Init as gizmo
    this->painterColorSelection = painterColorSelection;
    panelOffset = painterColorSelection.panelOffset;
    pos = painterColorSelection.pos;
    scale = painterColorSelection.scale;
    state = 7;
}

bool Element::isInteracted(){
    if(this->state == 0 && this->button.clicked)
        return true;
    if(this->state == 1 && this->rangeBar.valueDoneChanging || this->rangeBar.defaultPointBtnPressed || ((this->rangeBar.leftArrowHover || this->rangeBar.rightArrowHover) && *Mouse::LClick()))
        return true;
    if(this->state == 2 && this->checkBox.hover && Mouse::LClick())
        return true;
    if(this->state == 3 && this->comboBox.selectionDone)
        return true;
    if(this->state == 4 && this->textBox.active)
        return true;
    if(this->state == 6 && this->gizmo.interacted)
        return true;
    if(this->state == 7 && this->painterColorSelection.isInteracted())
        return true;
        
    return false;
}

void Element::render(Timer &timer,bool doMouseTracking){
    if(state == 0){ //Render the button
        button.pos = pos;
        button.scale = scale;
        button.panelOffset = panelOffset;
        button.doMouseTracking = doMouseTracking;
        button.render(timer,doMouseTracking);
    }
    if(state == 1){ //Render the rangeBar
        rangeBar.pos = pos;
        rangeBar.scale = scale;
        rangeBar.panelOffset = panelOffset;
        rangeBar.doMouseTracking = doMouseTracking;
        rangeBar.render(timer,doMouseTracking);
    }
    if(state == 2){ //Render the checkBox
        checkBox.pos = pos;
        checkBox.scale = scale;
        checkBox.panelOffset = panelOffset;
        checkBox.doMouseTracking = doMouseTracking;
        checkBox.render(timer,doMouseTracking);
    }
    if(state == 3){ //Render the comboxBox
        comboBox.pos = pos;
        comboBox.scale = scale;
        comboBox.panelOffset = panelOffset;
        comboBox.doMouseTracking = doMouseTracking;
        comboBox.render(timer,doMouseTracking);
    }
    if(state == 4){ //Render the textbox
        textBox.pos = pos;
        textBox.scale = scale;
        textBox.panelOffset = panelOffset;
        textBox.doMouseTracking = doMouseTracking;
        textBox.render(timer,doMouseTracking);
    }
    if(state == 5){ //Render the sectionHolder
        Texture arrowTxtr = appTextures.arrowR;
        if(sectionHolder.active)
            arrowTxtr = appTextures.arrowB;

        Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(4,2), this->sectionHolder.text, arrowTxtr, 0.f, true);
        btn.textColor = sectionHolder.textColor;
        btn.textColor2 = sectionHolder.textColor;
        btn.solidColor = true;
        btn.color = sectionHolder.sectionColor;
        btn.color2 = sectionHolder.sectionColor;
        btn.outline = false;
        btn.outlineExtra = false;

        btn.clickState1 = sectionHolder.active;

        btn.pos = pos;
        btn.scale.x = scale.x;
        btn.scale.y = sectionHolder.scaleY;
        btn.panelOffset = panelOffset;
        btn.doMouseTracking = doMouseTracking;
        btn.render(timer,doMouseTracking);

        if(!sectionHolder.fullActive)
            sectionHolder.active = btn.clickState1;
        else 
            sectionHolder.active = true;
    }
    if(state == 6){ 
        gizmo.pos = pos;
        gizmo.scale = scale;
        gizmo.panelOffset = panelOffset;
        gizmo.doMouseTracking = doMouseTracking;
        gizmo.render(timer, doMouseTracking);
    }
    if(state == 7){ 
        painterColorSelection.pos = pos;
        painterColorSelection.scale = scale;
        painterColorSelection.panelOffset = panelOffset;
        painterColorSelection.doMouseTracking = doMouseTracking;
        painterColorSelection.render(timer, doMouseTracking);
    }
}

bool Element::operator==(const Element& element) const{
    if(this->state != element.state)
        return false;

    if(state == 0){ //Render the button
        if(this->button.text != element.button.text)
            return false;
        if(this->button.clickState1 != element.button.clickState1)
            return false;
        if(this->button.textureSelection2D != element.button.textureSelection2D)
            return false;
        if(this->button.textureSelection3D != element.button.textureSelection3D)
            return false;
        if(this->button.colorSelection != element.button.colorSelection)
            return false;
        if(this->button.meshSelection != element.button.meshSelection)
            return false;
        if(this->button.materialSelection != element.button.materialSelection)
            return false;
        if(this->button.filterSelection != element.button.filterSelection)
            return false;
        if(this->button.keepPressingState != element.button.keepPressingState)
            return false;
    }
    if(state == 1){ //Render the rangeBar
        if(this->rangeBar.text != element.rangeBar.text)
            return false;
        if(this->rangeBar.value != element.rangeBar.value)
            return false;
        if(this->rangeBar.maxValue != element.rangeBar.maxValue)
            return false;
        if(this->rangeBar.minValue != element.rangeBar.minValue)
            return false;
        if(this->rangeBar.isNumeric != element.rangeBar.isNumeric)
            return false;
        if(this->rangeBar.constructValue != element.rangeBar.constructValue)
            return false;
    }
    if(state == 2){ //Render the checkBox
        if(this->checkBox.clickState1 != element.checkBox.clickState1)
            return false;
        if(this->checkBox.text != element.checkBox.text)
            return false;
    }
    if(state == 3){ //Render the comboxBox
        if(this->comboBox.text != element.comboBox.text)
            return false;
        if(this->comboBox.texts.size() != element.comboBox.texts.size())
            return false;

        for (size_t i = 0; i < this->comboBox.texts.size(); i++)
        {
            if(this->comboBox.texts[i] != element.comboBox.texts[i])
                return false;
        }
        
        if(this->comboBox.selectedIndex != element.comboBox.selectedIndex)
            return false;
    }
    if(state == 4){ //Render the textbox
        if(this->textBox.text != element.textBox.text)
            return false;
    }
    if(state == 5){ //Render the sectionHolder
        if(this->sectionHolder.text != element.sectionHolder.text)
            return false;
        if(this->sectionHolder.containerColor != element.sectionHolder.containerColor)
            return false;
        if(this->sectionHolder.textColor != element.sectionHolder.textColor)
            return false;
    }
    if(state == 6){ 
        if(this->gizmo.pitch != element.gizmo.pitch)
            return false;
        if(this->gizmo.yaw != element.gizmo.yaw)
            return false;
    }
    if(state == 7){ 
        if(this->painterColorSelection.clr1_Btn.color != element.painterColorSelection.clr1_Btn.color)
            return false;
        if(this->painterColorSelection.clr2_Btn.color != element.painterColorSelection.clr2_Btn.color)
            return false;
        if(this->painterColorSelection.clr3_Btn.color != element.painterColorSelection.clr3_Btn.color)
            return false;
    }
}   

bool Element::operator!=(const Element& element) const{
    return !(*this == element);
}

Section::Section(){}
Section::Section(Element header, std::vector<Element> elements){
    this->header = header;
    this->elements = elements;
}

bool Section::operator==(const Section& section) const{
    
    if(this->elements.size() != section.elements.size()) {
        return false;
    }

    for (size_t i = 0; i < section.elements.size(); i++)
    {
        if(this->elements[i] != section.elements[i])
            return false;
    }    
    
    return true;
}

bool Section::operator!=(const Section& section) const{
    return !(*this == section);
}