
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef DIALOGS_HPP
#define DIALOGS_HPP

//Forward declarations
class ColorPalette;

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"


class DialogControl
{
private:
public:
    bool active = false;
    float mixVal = 0.;
    bool firstFrameActivated = false;

    DialogControl();
    DialogControl(bool active);
    
    void updateStart(Shader buttonShader);
    void updateEnd(Timer timer,Shader buttonShader,float transitionDuration);
    void activate();
    void unActivate();
    bool isActive();
    bool isComplete();
};

//!------------------------------SETTINGS DIALOG------------------------------

class NewProjectDialog;
class LoadProjectDialog;
class TextureSelectionDialog;

class Skybox;

class SettingsDialog
 {
 private:
    Context context;
 public:
    Panel panel;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructor
    SettingsDialog();
    SettingsDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

    //Public member functions    
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,
                glm::vec2 videoScale,int &textureRes,bool &VSync);
 };


//!------------------------------NEW TEXTURE DIALOG------------------------------


class NewTextureDialog
 {
 private:
    Context context;
 public:
     
     Panel panel;
     NewTextureDialog();
     Shader buttonShader;
     bool active = false;

    //Constructor
    NewTextureDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
    
    //Public member function
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                Library &library,glm::vec2 videoScale,int textureRes);
 };


//!------------------------------LOAD PROJECT DIALOG------------------------------


class LoadProjectDialog
 {
 public:
    Panel bgPanel;
    Panel projectsPanel;
    Button loadButton;
    Button textBtn1;
    Button textBtn2;
    Button textBtn3;
    Button textBtn4;
    Box box;
    Shader buttonShader;
    AppTextures appTextures;
    DialogControl dialogControl;

    //Constructors
    LoadProjectDialog();
    LoadProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
    
    //Public member functions
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model);
 };


//!------------------------------GREETING DIALOG DIALOG------------------------------


class DisplayerDialog
 {
 private:
    Context context;
    Shader prefilteringShader;
    Shader skyboxBallShader;
    Model sphereModel;
 public:
    Panel panel;
    Shader buttonShader;
    DialogControl dialogControl;
    std::vector<Element> skyboxes;

    //Constructors
    DisplayerDialog();
    DisplayerDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,
                    Shader prefilteringShader,Shader skyboxBallShader,Model &sphereModel);
    
    //Public member functions
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                Library &library,glm::vec2 videoScale,Skybox &skybox);
 };


//!------------------------------GREETING DIALOG DIALOG------------------------------


class GreetingDialog
{
public:
   Button loadProjectButton;
   Button createProjectButton;
   Button textureDisplayerButton;
   Button textButton1;
   Panel bgPanel;
   Shader buttonShader;
   Box box;
   DialogControl dialogControl;
   
   //Constructors
   GreetingDialog();
   GreetingDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
   
   //Public member functions
   void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
               glm::vec2 videoScale, NewProjectDialog &newProjectDialog,LoadProjectDialog &loadProjectDialog);
};

//!------------------------------MATERIAL EDITOR DIALOG------------------------------


class MaterialEditorDialog
{
private:
    Shader buttonShader;
    Shader tdModelShader;
    ColorPalette colorPalette;
    Model sphereModel;

    //Private member functions
    void updateLayerPanel(Material &material,int &textureRes,Box &box,Context &context);
    void checkLayerPanel(Material &material);
    void checkModifiersPanel(Material &material,float textureRes,Box box,Context context,Mouse &mouse,TextureSelectionDialog &textureSelectionDialog);
    void updateLayerPanelElements(Material &material,int &textureRes,Box &box,Context &context);
    void checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Library &library,float textureRes,Box box,Context context);

public:
    DialogControl dialogControl;
    glm::vec3 pos = glm::vec3(50.f,50.f,0.8f); ///Position of the dialog
    glm::vec2 scale = glm::vec2(40,40); ///Scale of the dialog
    glm::vec3 posLayer = glm::vec3(20.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleLayer = glm::vec2(10,40); ///Scale of the layer panel
    glm::vec3 posModifier = glm::vec3(80.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleModifier = glm::vec2(10,40); ///Scale of the layer panel
    Button materialDisplayer; //That buttons displays the material with a texture
    Panel bgPanel; //To cover the bg
    Panel layerPanel; //Modifiers will be displayed in there
    Panel modifiersPanel; //Modifiers will be displayed in there
    int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting
    AppMaterialModifiers appMaterialModifiers;
    int selectedMaterialModifierIndex = 0;
    
    //Constructors
    MaterialEditorDialog();
    MaterialEditorDialog(Shader buttonShader,Shader tdModelShader,ColorPalette colorPalette,AppTextures appTextures,Model &sphereModel);

    //Public member functions
    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,TextureSelectionDialog &textureSelectionDialog,Library &library,
        Material &material, int textureRes,Box box,Context context);
    void activate();
    void deactivate(TextureSelectionDialog &textureSelectionDialog);
    void updateMaterial(Material &material,float textureRes,Box box,Context context);
};


//!------------------------------NEW PROJECT DIALOG------------------------------


class NewProjectDialog
 {
 private:
    AppTextures appTextures;
 public:
    Panel panel;
    Box box;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructors
    NewProjectDialog();
    NewProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

    //Public member functions
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
 {
 private:
    
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    AppTextures appTextures;
    Context context;
 public:
    Panel panel;
    Box box;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructors
    ExportDialog();
    ExportDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

    //Public member functions
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model,MaterialEditorDialog &materialEditorDialog
                ,std::vector<Node> &nodeScene);
 };


//!------------------------------TEXTURE SELECTION DIALOG------------------------------

class TextureSelectionDialog
{
private:
    ColorPalette colorPalette;
    Shader buttonShader;
public:
    Panel texturePanel;
    int selectedTextureIndex = 1000; 
    bool clicked = false; 
    glm::vec3 pos = glm::vec3(50,50,0.95f);
    glm::vec2 scale = glm::vec2(10); 
    DialogControl dialogControl;

    //Constructors
    TextureSelectionDialog();
    TextureSelectionDialog(Shader buttonShader,ColorPalette colorPalette);

    //Public member functions
    void render(glm::vec2 videoScale,Mouse &mouse,Timer &timer,TextRenderer &textRenderer,Library library);
};

//!------------------------------CONTEXT MENU------------------------------


class ContextMenu
{
private:
    /* data */
public:
    Panel contextPanel;
    glm::vec3 pos;
    int selectedElement = 0;
    bool active = false; 
    
    //Constructors
    ContextMenu();
    ContextMenu(Shader buttonShader,ColorPalette colorPalette,std::vector<std::string> elements);
    
    /// @brief Public member function to render the context menu
    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer);
};

#endif