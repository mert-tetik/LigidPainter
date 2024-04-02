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

#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Project/Project.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_library;
Texture panel_library_selected_texture;

static Button add_btn(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), "Add", Texture(), 0.f, false);
static Button import_btn(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), "Import", Texture(), 0.f, false);

/* Forward declared util functions */
static void check_context_menus(Timer& timer);
static void element_interactions(Timer& timer);
static void bar_buttons_interactions(Timer& timer);
static void update_elements();

void panel_library_render(
                            Timer& timer, 
                            bool doMouseTracking
                        )
{

    //Update the library displayer panel every time library changed

    // If *clicked / double clicked / etc.* to elements  
    element_interactions(timer);
    
    // If clicked to bar buttons (import & add new)
    bar_buttons_interactions(timer);
    
    //Set library changed to false after updating some stuff after library change
    Library::setChanged(false);

    {
        std::vector<Element> elements;

        if(Library::getSelectedElementIndex() == 0){//Update textures
            for (size_t i = 0; i < Library::getTextureArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexture(i)->title       , *Library::getTexture(i), 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 1){ //Update materials
            for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getMaterial(i)->title       , Library::getMaterial(i)->displayingTexture, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 2){ //Update materials
            for (size_t i = 0; i < Library::getBrushArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getBrush(i)->title       , Library::getBrush(i)->displayingTexture, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 3){ //Update tdmodels
            for (size_t i = 0; i < Library::getModelArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getModel(i)->title       , Library::getModel(i)->displayingTxtr, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 4){ //Fonts
        
        }
        else if(Library::getSelectedElementIndex() == 5){ //Scripts
        
        }
        else if(Library::getSelectedElementIndex() == 6){ //Filters
            for (size_t i = 0; i < Library::getFilterArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getFilter(i)->title, Library::getFilter(i)->displayingTxtr, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 8){ //Texture packs
            for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
            {
                //Push texture elements into the section
                elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexturePack(i)->title, appTextures.packageIcon, 0.f,false))) ;
            }
        }

        const float btn_scale = 4.f;
        int row_count = 3;
        panel_library.sections[0].elements[0].scale.y = std::floor(elements.size() / row_count) * btn_scale + add_btn.scale.y;

        panel_library.render(timer, doMouseTracking);
        if(panel_library.resizingDone){
            panels_transform();
        }

        add_btn.scale.x = panel_library.scale.x / 2.f;
        add_btn.pos = glm::vec3(
                                    panel_library.pos.x  - panel_library.scale.x + add_btn.scale.x, 
                                    panel_library.pos.y - panel_library.scale.y + add_btn.scale.y, 
                                    panel_library.pos.z
                                );

        import_btn.scale.x = add_btn.scale.x;
        import_btn.pos = add_btn.pos;
        import_btn.pos.x += import_btn.scale.x * 2.f;

        for (size_t i = 0; i < elements.size(); i++)
        {
            Element* btn = &elements[i];

            if(panel_library.slideRatio < 1)
                btn->scale.x = (panel_library.scale.x - panel_library.sliderButton.scale.x) / row_count; 
            else
                btn->scale.x = (panel_library.scale.x) / row_count; 
            
            btn->scale.y = btn_scale;

            btn->pos.x = (panel_library.pos.x - panel_library.scale.x + btn->scale.x) + (btn->scale.x * 2.f * (i % row_count));
            btn->pos.y = (panel_library.pos.y - panel_library.scale.y + btn->scale.y) + (btn->scale.y * 2.f * std::floor(i / row_count));
            btn->pos.y += add_btn.scale.y * 2.f;
            btn->pos.y -= panel_library.slideVal;
            btn->pos.z = 0.7f;

            btn->button.textureStickToTop = true;
            btn->button.textureSizeScale = 1.5f;
            if(btn->pos.y - btn->scale.y < (panel_library.pos.y + panel_library.scale.y) && btn->pos.y + btn->scale.y > (panel_library.pos.y - panel_library.scale.y))
                btn->render(timer, true);
        }

        add_btn.render(timer, true);
        import_btn.render(timer, true);
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);

    // If right clicked to elements
    check_context_menus(timer);

    if(glIsTexture(panel_library_selected_texture.ID) == GL_FALSE)
        panel_library_selected_texture.ID = false;
}


/*

if(isLibraryDisplayer){
                        for (size_t meshI = 0; meshI < getScene()->model->meshes.size(); meshI++)
                        {
                            std::string channelName = "";
                            unsigned int txtrID = sections[sI].elements[i].button.texture.ID;

                            if(getScene()->model->meshes[meshI].albedo.ID == txtrID)
                                channelName = "Albedo";
                            if(getScene()->model->meshes[meshI].roughness.ID == txtrID)
                                channelName = "Roughness";
                            if(getScene()->model->meshes[meshI].metallic.ID == txtrID)
                                channelName = "Metallic";
                            if(getScene()->model->meshes[meshI].normalMap.ID == txtrID)
                                channelName = "Normal Map";
                            if(getScene()->model->meshes[meshI].heightMap.ID == txtrID)
                                channelName = "Height Map";
                            if(getScene()->model->meshes[meshI].ambientOcclusion.ID == txtrID)
                                channelName = "Ambient Occlusion";
                        
                            if(channelName != ""){
                                Button materialInfo = Button(ELEMENT_STYLE_SOLID, glm::vec2(sections[sI].elements[i].button.scale.x, sections[sI].elements[i].button.hoverMixVal + 0.05), getScene()->model->meshes[meshI].materialName, Texture(), 0.f, false);
                                Button channelInfo = Button(ELEMENT_STYLE_SOLID, glm::vec2(sections[sI].elements[i].button.scale.x, sections[sI].elements[i].button.hoverMixVal + 0.05), channelName, Texture(), 0.f, false);

                                materialInfo.color = ColorPalette::themeColor;
                                channelInfo.color = ColorPalette::themeColor;
                                
                                materialInfo.outlineExtra = false;
                                materialInfo.outline = false;

                                channelInfo.outlineExtra = false;
                                channelInfo.outline = false;

                                if(materialInfo.scale.y < 0.5f){
                                    materialInfo.text = "";
                                    channelInfo.text = "";
                                }

                                materialInfo.pos = sections[sI].elements[i].button.pos;
                                materialInfo.pos.y -= sections[sI].elements[i].button.scale.y - materialInfo.scale.y;
                                materialInfo.pos.z += 0.1f;
                                channelInfo.pos = materialInfo.pos;
                                channelInfo.pos.y += channelInfo.scale.y + materialInfo.scale.y;
                            
                                materialInfo.render(timer, false);
                                channelInfo.render(timer, false);
                            }
                        }
                        

                        glm::vec4 textColor = glm::vec4(1) - sections[sI].elements[i].button.color;
                        textColor.a = 1.;
                        ShaderSystem::buttonShader().setVec4("properties.color"  ,    textColor      ); //Default button color

                        textRenderer.loadTextData(
                                                    sections[sI].elements[i].button.text,
                                                    glm::vec3(sections[sI].elements[i].button.resultPos.x,sections[sI].elements[i].button.resultPos.y + sections[sI].elements[i].button.resultScale.y/1.4f,sections[sI].elements[i].button.resultPos.z),
                                                    false,
                                                    0.25f, //TODO Change with a dynamic value
                                                    sections[sI].elements[i].button.resultPos.x - sections[sI].elements[i].button.resultScale.x,
                                                    sections[sI].elements[i].button.resultPos.x + sections[sI].elements[i].button.resultScale.x,
                                                    TEXTRENDERER_ALIGNMENT_MID
                                                );

                        textRenderer.renderText();
                    }


*/

static void check_context_menus(Timer& timer){
    for (size_t elementI = 0; elementI < panel_library.sections[0].elements.size(); elementI++)
    {
        // Right clicked to an element
        if(panel_library.sections[0].elements[elementI].button.hover && *Mouse::RClick()){ 
            
            //To a texture
            if(Library::getSelectedElementIndex() == 0){
                // Show texture context menu
                int res = show_context_menu(timer, {"Rename", "Duplicate", "Copy path", "Edit", "Delete"});
                
                //Clicked to rename button
                if(res == 0){

                }
                //Clicked to duplicate button
                else if(res == 1){
                    Texture duplicatedTexture = *Library::getTexture(elementI);
                    duplicatedTexture.ID = Library::getTexture(elementI)->duplicateTexture();
                    duplicatedTexture.title += "_duplicated";
                    Library::addTexture(duplicatedTexture, "New texture via duplication");
                }
                //Clicked to copy path button
                else if(res == 2){
                    LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Textures" + UTIL::folderDistinguisher() + Library::getTexture(elementI)->title + ".png");
                }
                //Clicked to edit button
                else if(res == 3){
                    dialog_textureEditor.show(timer, Skybox(), Library::getTexture(elementI));
                }
                //Clicked to delete button
                else if(res == 4){
                    Library::eraseTexture(elementI);
                }
                
            }
            
            //To a material
            if(Library::getSelectedElementIndex() == 1){
                // Show material context menu
                int res = show_context_menu(timer, {"Rename", "Duplicate", "Copy path", "Delete", "Export"});
               
                //Clicked to edit button
                if(res == 0){
                    //Select the material that material editor will edit & show the material editor dialog
                    dialog_materialEditor.show(timer, Library::getMaterial(elementI));
                }
                //Clicked to rename button
                else if(res == 1){
                    
                }
                //Clicked to duplicate button
                else if(res == 2){
                    Material duplicatedMaterial = Library::getMaterial(elementI)->duplicateMaterial();
                    Library::addMaterial(duplicatedMaterial, "New material via duplication");
                }
                //Clicked to copy path button
                else if(res == 3){
                    LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Materials" + UTIL::folderDistinguisher() + Library::getMaterial(elementI)->title + ".lgdmaterial");
                }
                //Clicked to delete button
                else if(res == 4){
                    //Delete the material
                    Library::eraseMaterial(elementI);
                }
                //Clicked to export button
                else if(res == 5){
                    std::string exportingPath = showFileSystemObjectSelectionDialog("Export the lgdmaterial file", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE);
                    if(exportingPath.size())
                        FileHandler::writeLGDMATERIALFile(exportingPath, *Library::getMaterial(elementI));
                }
            }
            
            //To a brush
            if(Library::getSelectedElementIndex() == 2){
                
                // Show brush context menu
                int res = show_context_menu(timer, {"Edit", "Rename", "Duplicate", "Copy Path", "Delete"});
                
                //Clicked to edit brush button
                if(res == 0){
                    registerBrushChangedAction("Edit brush", Texture(), *Library::getBrush(elementI), elementI);
                    dialog_brushModification.show(timer, &Library::getBrush(elementI)->properties);
                }
                
                //Clicked to rename button
                else if(res == 1){
                    
                }
                
                //Clicked to duplicate button
                else if(res == 2 && elementI < Library::getBrushArraySize()){
                    Brush selectedBrush = *Library::getBrush(elementI);
                    Texture dupTxtr = selectedBrush.properties.brushTexture.duplicateTexture();
                    dupTxtr.proceduralProps = selectedBrush.properties.brushTexture.proceduralProps;
                    Library::addBrush(
                                        Brush(
                                                0.1f,
                                                selectedBrush.properties.spacing,
                                                selectedBrush.properties.hardness,
                                                selectedBrush.properties.sizeJitter,
                                                selectedBrush.properties.scatter,
                                                selectedBrush.properties.fade,
                                                selectedBrush.properties.rotation,
                                                selectedBrush.properties.rotationJitter,
                                                selectedBrush.properties.alphaJitter,
                                                selectedBrush.properties.individualTexture,
                                                selectedBrush.properties.sinWavePattern,
                                                selectedBrush.title + "_duplicated",
                                                dupTxtr                     
                                            ),
                                            "New brush via duplication"
                                    );
                }
                
                //Clicked to copy path button
                else if(res == 3){
                    LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Brushes" + UTIL::folderDistinguisher() + Library::getBrush(elementI)->title + ".lgdfilter");
                }
                
                //Clicked to delete button
                else if(res == 4){
                    Library::eraseBrush(elementI);
                }
            }

            //To a model
            if(Library::getSelectedElementIndex() == 3){
                // Show brush context menu
                int res = show_context_menu(timer, {"Model Info", "Use Model"});

                //Clicked to model info button
                if(res == 0){
                    dialog_modelInfo.show(timer, Library::getModel(elementI));
                }

                //Clicked to use the model button
                else if(res == 1){
                    getScene()->model = Library::getModel(elementI);
                    getScene()->model->newModelAdded = true; 
                }
            }
        }
    }
}

static void element_interactions(Timer& timer){
    //Update the selected texture
    for (size_t i = 0; i < panel_library.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
    {
        if(Library::getSelectedElementIndex() == 0){ //Textures selected
            if(panel_library.sections[0].elements[i].button.clicked){
                panel_library_selected_texture = *Library::getTexture(i); //Select the texture 
            } 
        
            if(Library::getTexture(i)->ID == panel_library_selected_texture.ID) //Highlight the selected texture
                panel_library.sections[0].elements[i].button.clickState1 = true;
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials selected
            if(panel_library.sections[0].elements[i].button.hover && *Mouse::LDoubleClick()){
                dialog_materialDisplayer.show(timer, Library::getMaterialObj(i));
            } 
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters selected
            if(panel_library.sections[0].elements[i].button.hover && *Mouse::LDoubleClick()){
                dialog_filterDisplayer.dialogControl.activate();
                dialog_filterDisplayer.filter = *Library::getFilter(i);
            } 
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs selected
            if(panel_library.sections[0].elements[i].button.clicked){
                dialog_texturePackEditor.show(timer, *Library::getTexturePack(i));
            } 
        }
    }
}

static void bar_buttons_interactions(Timer& timer){
    if(add_btn.clicked){
        if(Library::getSelectedElementIndex() == 0){//Textures
            dialog_newTexture.show(timer);
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated
            Library::addMaterial(Material("NewMaterial", {}), "New blank material");
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            Texture whiteProceduralTxtr;
            whiteProceduralTxtr.proceduralProps.proceduralID = 24; //Solid white
            whiteProceduralTxtr.proceduralProps.proceduralnverted = 0;
            whiteProceduralTxtr.proceduralProps.proceduralScale = 1.f;
            whiteProceduralTxtr.title = "AutoGenerated";
         
            Library::addBrush(
                                        Brush
                                            (    
                                                0.1f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                0.f,
                                                false,
                                                false,
                                                "NewBrush",
                                                whiteProceduralTxtr
                                            ),
                                            "New brush"
                                    );
            
        }
        if(Library::getSelectedElementIndex() == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                project_add_model(test);
            }
        }
        if(Library::getSelectedElementIndex() == 4){ //Fonts
            
        }
        if(Library::getSelectedElementIndex() == 5){ //Scripts
            
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters
            
            std::string test = showFileSystemObjectSelectionDialog("Select a filter file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_FILTER, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Filter filter;
                filter.load(test);
                Library::addFilter(filter);
            }
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs
            TexturePack texturePack;
            texturePack.title = "NewTexturePack";
            Library::addTexturePack(texturePack);
        }
    }
    if(import_btn.clicked){ //Import button
        if(Library::getSelectedElementIndex() == 0){//Textures  
            std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Texture importedTxtr;
                importedTxtr.load(test.c_str());
                Library::addTexture(importedTxtr, "New texture via import");
            }
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials
            //Select material
            std::string test = showFileSystemObjectSelectionDialog("Select a material file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Material importedMaterial("", {});
                if(FileHandler::readLGDMATERIALFile(test, importedMaterial))
                    Library::addMaterial(importedMaterial, "New material via importing");
            }
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            
            std::string test = showFileSystemObjectSelectionDialog("Select a brush file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Brush importedBrush;
                if(FileHandler::readLGDBRUSHFile(test,importedBrush)){
                    Library::addBrush(importedBrush, "New brush via import");
                }
            }
            
        }
        if(Library::getSelectedElementIndex() == 3){ //3D Models
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                project_add_model(test);
            }
        }

        if(Library::getSelectedElementIndex() == 4){ //Fonts
            
        }
        if(Library::getSelectedElementIndex() == 5){ //Scripts
            
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters
            
            std::string test = showFileSystemObjectSelectionDialog("Select a filter file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_FILTER, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Filter filter;
                filter.load(test);
                Library::addFilter(filter);
            }
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs
            std::string test = showFileSystemObjectSelectionDialog("Select a texture pack folder.", "", 0, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);

            if(test.size()){
                TexturePack texturePack;
                texturePack.load(test);
                Library::addTexturePack(texturePack);
            }
        }
    }
}

static void update_elements(){
    
    //Remove all the elements of the library panel displayer
    panel_library.sections.clear(); 
        
    //Create a new section
    Section libSection;
    libSection.header = Element(Button()); //Has no section button
    //Fill the elements of the section using the data in the library structure
    if(Library::getSelectedElementIndex() == 0){//Update textures
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexture(i)->title       , *Library::getTexture(i), 0.f,false))) ;
        }
    }
    else if(Library::getSelectedElementIndex() == 1){ //Update materials
        for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getMaterial(i)->title       , Library::getMaterial(i)->displayingTexture, 0.f,false))) ;
        }
    }
    else if(Library::getSelectedElementIndex() == 2){ //Update materials
        for (size_t i = 0; i < Library::getBrushArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getBrush(i)->title       , Library::getBrush(i)->displayingTexture, 0.f,false))) ;
        }
    }
    else if(Library::getSelectedElementIndex() == 3){ //Update tdmodels
        for (size_t i = 0; i < Library::getModelArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getModel(i)->title       , Library::getModel(i)->displayingTxtr, 0.f,false))) ;
        }
    }
    else if(Library::getSelectedElementIndex() == 4){ //Fonts
    
    }
    else if(Library::getSelectedElementIndex() == 5){ //Scripts
    
    }
    else if(Library::getSelectedElementIndex() == 6){ //Filters
        for (size_t i = 0; i < Library::getFilterArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getFilter(i)->title, Library::getFilter(i)->displayingTxtr, 0.f,false))) ;
        }
    }
    else if(Library::getSelectedElementIndex() == 8){ //Texture packs
        for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
        {
            //Push texture elements into the section
            libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexturePack(i)->title, appTextures.packageIcon, 0.f,false))) ;
        }
    }

    //Give the section
    panel_library.sections.push_back(Section(Element(Button()),libSection.elements));
} 