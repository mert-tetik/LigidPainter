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
static void bar_buttons_interactions(Timer& timer);

void panel_library_render(
                            Timer& timer, 
                            bool doMouseTracking
                        )
{

    //Update the library displayer panel every time library changed
    
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

            if(btn->button.texture.ID == panel_library_selected_texture.ID) //Highlight the selected texture
                btn->button.color = ColorPalette::themeColor;

            if(btn->pos.y - btn->scale.y < (panel_library.pos.y + panel_library.scale.y) && btn->pos.y + btn->scale.y > (panel_library.pos.y - panel_library.scale.y))
                btn->render(timer, true);

                glm::vec4 textColor = glm::vec4(1) - btn->button.color;
                textColor.a = 1.;
                ShaderSystem::buttonShader().setVec4("properties.color"  ,    textColor      ); //Default button color

                textRenderer.loadTextData(
                                            btn->button.text,
                                            glm::vec3(btn->button.resultPos.x,btn->button.resultPos.y + btn->button.resultScale.y/1.4f,btn->button.resultPos.z),
                                            false,
                                            0.25f, //TODO Change with a dynamic value
                                            btn->button.resultPos.x - btn->button.resultScale.x,
                                            btn->button.resultPos.x + btn->button.resultScale.x,
                                            TEXTRENDERER_ALIGNMENT_MID
                                        );

                textRenderer.renderText();

                if(Library::getSelectedElementIndex() == 0){ //Textures selected
                    if(btn->button.hover && *Mouse::LClick()){
                        panel_library_selected_texture = *Library::getTexture(i); //Select the texture 
                    } 
                }
                if(Library::getSelectedElementIndex() == 1){ //Materials selected
                    if(btn->button.hover && *Mouse::LDoubleClick()){
                        dialog_materialDisplayer.show(timer, Library::getMaterial(i));
                    } 
                }
                if(Library::getSelectedElementIndex() == 6){ //Filters selected
                    if(btn->button.hover && *Mouse::LDoubleClick()){
                        dialog_filterDisplayer.dialogControl.activate();
                        dialog_filterDisplayer.filter = *Library::getFilter(i);
                    } 
                }
                if(Library::getSelectedElementIndex() == 8){ //Texture Packs selected
                    if(btn->button.hover && *Mouse::LClick()){
                        dialog_texturePackEditor.show(timer, *Library::getTexturePack(i));
                    } 
                }

            // Right clicked to an element
            if(btn->button.hover && *Mouse::RClick()){ 
                
                //To a texture
                if(Library::getSelectedElementIndex() == 0){
                    // Show texture context menu
                    int res = show_context_menu(timer, {"Rename", "Duplicate", "Copy path", "Edit", "Delete"});
                    
                    //Clicked to rename button
                    if(res == 0){
                        dialog_renaming.show(timer, glm::vec2(btn->pos.x, btn->pos.y + btn->scale.y/1.4f), btn->scale.x * 1.5f, &Library::getTexture(i)->title, 20);
                    }
                    //Clicked to duplicate button
                    else if(res == 1){
                        Texture duplicatedTexture = *Library::getTexture(i);
                        duplicatedTexture = Library::getTexture(i)->duplicateTexture("panel_library_render");
                        duplicatedTexture.title += "_duplicated";
                        Library::addTexture(duplicatedTexture, "New texture via duplication");
                    }
                    //Clicked to copy path button
                    else if(res == 2){
                        LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Textures" + UTIL::folderDistinguisher() + Library::getTexture(i)->title + ".png");
                    }
                    //Clicked to edit button
                    else if(res == 3){
                        dialog_textureEditor.show(timer, Skybox(), Library::getTexture(i));
                    }
                    //Clicked to delete button
                    else if(res == 4){
                        Library::eraseTexture(i);
                    }
                    
                }
                
                //To a material
                if(Library::getSelectedElementIndex() == 1){
                    // Show material context menu
                    int res = show_context_menu(timer, {"Edit", "Rename", "Duplicate", "Copy path", "Delete", "Export"});
                
                    //Clicked to edit button
                    if(res == 0){
                        //Select the material that material editor will edit & show the material editor dialog
                        dialog_materialEditor.show(timer, Library::getMaterial(i));
                    }
                    //Clicked to rename button
                    else if(res == 1){
                        dialog_renaming.show(timer, glm::vec2(btn->pos.x, btn->pos.y + btn->scale.y/1.4f), btn->scale.x * 1.5f, &Library::getMaterial(i)->title, 20);
                    }
                    //Clicked to duplicate button
                    else if(res == 2){
                        Material duplicatedMaterial = Library::getMaterial(i)->duplicateMaterial();
                        Library::addMaterial(duplicatedMaterial, "New material via duplication");
                    }
                    //Clicked to copy path button
                    else if(res == 3){
                        LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Materials" + UTIL::folderDistinguisher() + Library::getMaterial(i)->title + ".lgdmaterial");
                    }
                    //Clicked to delete button
                    else if(res == 4){
                        //Delete the material
                        Library::eraseMaterial(i);
                    }
                    //Clicked to export button
                    else if(res == 5){
                        std::string exportingPath = showFileSystemObjectSelectionDialog("Export the lgdmaterial file", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE);
                        if(exportingPath.size())
                            FileHandler::writeLGDMATERIALFile(exportingPath, *Library::getMaterial(i));
                    }
                }
                
                //To a brush
                if(Library::getSelectedElementIndex() == 2){
                    
                    // Show brush context menu
                    int res = show_context_menu(timer, {"Edit", "Rename", "Duplicate", "Copy Path", "Delete"});
                    
                    //Clicked to edit brush button
                    if(res == 0){
                        dialog_log.registerBrushChangedAction("Edit brush", Texture(), *Library::getBrush(i), i);
                        dialog_brushModification.show(timer, &Library::getBrush(i)->properties);
                    }
                    
                    //Clicked to rename button
                    else if(res == 1){
                        dialog_renaming.show(timer, glm::vec2(btn->pos.x, btn->pos.y + btn->scale.y/1.4f), btn->scale.x * 1.5f, &Library::getBrush(i)->title, 20);
                    }
                    
                    //Clicked to duplicate button
                    else if(res == 2 && i < Library::getBrushArraySize()){
                        Brush selectedBrush = *Library::getBrush(i);
                        Texture dupTxtr = selectedBrush.properties.brushTexture.duplicateTexture("panel_library_render");
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
                        LigidGL::setClipboardText(project_path() + UTIL::folderDistinguisher() + "Brushes" + UTIL::folderDistinguisher() + Library::getBrush(i)->title + ".lgdfilter");
                    }
                    
                    //Clicked to delete button
                    else if(res == 4){
                        Library::eraseBrush(i);
                    }
                }

                //To a model
                if(Library::getSelectedElementIndex() == 3){
                    // Show brush context menu
                    int res = show_context_menu(timer, {"Model Info", "Use Model"});

                    //Clicked to model info button
                    if(res == 0){
                        dialog_modelInfo.show(timer, Library::getModel(i));
                    }

                    //Clicked to use the model button
                    else if(res == 1){
                        getScene()->model = Library::getModel(i);
                        getScene()->model->newModelAdded = true; 
                    }
                }
            }
        }

        add_btn.render(timer, true);
        import_btn.render(timer, true);
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);

    if(glIsTexture(panel_library_selected_texture.ID) == GL_FALSE)
        panel_library_selected_texture.ID = 0;
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