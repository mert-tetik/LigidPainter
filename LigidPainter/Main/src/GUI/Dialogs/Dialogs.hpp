
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef DIALOGS_HPP
#define DIALOGS_HPP


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>


#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"

//Forward declarations
class ColorPalette;
class ContextMenu;

class DialogControl
{
private:
public:
    bool active = false;
    float mixVal = 0.;
    bool firstFrameActivated = false;

    DialogControl();
    DialogControl(bool active);
    
    void updateStart();
    void updateEnd(Timer timer,float transitionDuration);
    void activate();
    void unActivate();
    bool isActive();
    bool isComplete();
};

//!------------------------------MESSAGE BOX------------------------------

#define MESSAGEBOX_TYPE_ERROR 0
#define MESSAGEBOX_TYPE_WARNING 1
#define MESSAGEBOX_TYPE_INFO 2
#define MESSAGEBOX_TYPE_QUESTION 3

#define MESSAGEBOX_BUTTON_OK 0
#define MESSAGEBOX_BUTTON_OKCANCEL 1
#define MESSAGEBOX_BUTTON_YESNO 2
#define MESSAGEBOX_BUTTON_YESNOCANCEL 3

int showMessageBox(const std::string title, const std::string message, const int type, const int button);

//!------------------------------SELECT FILE SYSTEM OBJECT DIALOG------------------------------


#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE 0
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER 1
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE 2

#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE 0
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL 1
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL 2
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH 3
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID 4
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_FILTER 5

std::string showFileSystemObjectSelectionDialog(const std::string title, const std::string defaultPath, const int filterTemplate, const bool multipleSelection, const int type);

//!------------------------------SETTINGS DIALOG------------------------------

class NewProjectDialog;
class LoadProjectDialog;
class TextureSelectionDialog;

class Skybox;

class SettingsDialog
 {
 private:
    
 public:
    Panel panel;
    DialogControl dialogControl;

    //Constructor
    SettingsDialog();
    SettingsDialog(ColorPalette colorPalette);

    //Public member functions    
    void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer);
 };


//!------------------------------NEW TEXTURE DIALOG------------------------------


class NewTextureDialog
 {
 private:
    
 public:
     
     Panel panel;
     NewTextureDialog();
     
     DialogControl dialogControl;

    //Constructor
    NewTextureDialog(ColorPalette colorPalette);
    
    //Public member function
    void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer);
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
   
   
   AppMaterialModifiers appMaterialModifiers;
   DialogControl dialogControl;

   //Constructors
   LoadProjectDialog();
   LoadProjectDialog(ColorPalette colorPalette,AppMaterialModifiers& appMaterialModifiers);
   
   //Public member functions
   void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer,
               Project &project,bool &greetingDialogActive,bool &startScreen);
 };


//!------------------------------ DISPLAYER DIALOG ------------------------------


class DisplayerDialog
 {
 public:
    Panel panel;
    
    DialogControl dialogControl;
    std::vector<Element> skyboxes;

    //Constructors
    DisplayerDialog();
    DisplayerDialog(ColorPalette colorPalette);
    
    //Public member functions
    void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer,
                Skybox &skybox);
 };


//!------------------------------ DISPLAYER DIALOG ------------------------------


class TextureEditorDialog
 {
 public:
   Panel bgPanel;
   Panel sectionPanel;
   
   DialogControl dialogControl;
   std::vector<Element> resizeElements;
   std::vector<Element> bluringElement;
   std::vector<Element> noiseElements;
   std::vector<Element> normalMapElements;
   std::vector<Element> distortionElements;
   std::vector<Element> imagesElements;

   Button saveButton;
   Button saveAsButton;
   
   Button filterBtn;
   Filter filter;

   Button displayerBtn;

   unsigned int displayingTexture;

   int selectedSection = 0;

   //Constructors
   TextureEditorDialog();
   TextureEditorDialog(ColorPalette colorPalette);
   
   //Public member functions
   void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer, Skybox &skybox, glm::mat4 projection, Texture receivedTexture);

   void updateDisplayingTexture(Texture& receivedTexture, unsigned int destTxtr);
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
   
   Box box;
   DialogControl dialogControl;

   bool startScreen = true;
   
   //Constructors
   GreetingDialog();
   GreetingDialog(ColorPalette colorPalette);
   
   //Public member functions
   void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer,
               NewProjectDialog &newProjectDialog,LoadProjectDialog &loadProjectDialog);
};

//!------------------------------MATERIAL EDITOR DIALOG------------------------------


class MaterialEditorDialog
{
private:
    
   ColorPalette colorPalette;

   //Private member functions
   void updateLayerPanel(Material &material,Box &box);
   void checkLayerPanel(Material &material,std::vector<ContextMenu> &contextMenus);
   void checkModifiersPanel(Material &material,Box box,TextureSelectionDialog &textureSelectionDialog);
   void updateLayerPanelElements(Material &material,Box &box,std::vector<ContextMenu> contextMenus);
   void checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Box box);
   void manageContextMenuActions( Material &material, Box box, std::vector<ContextMenu> &contextMenus);

   bool updateTheMaterial = false;
   bool prevUpdateTheMaterial = false;
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
   ComboBox displayModeComboBox;
   Button barButton; //Modifiers will be displayed in there
   int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting TODO REMOVE THAT
   AppMaterialModifiers appMaterialModifiers;
   int selectedMaterialModifierIndex = 0;

   Camera displayerCamera;
    
   //Constructors
   MaterialEditorDialog();
   MaterialEditorDialog(ColorPalette colorPalette);

   //Public member functions
   void render(Timer &timer,TextRenderer &textRenderer,TextureSelectionDialog &textureSelectionDialog,
               Material &material, Box box, std::vector<ContextMenu> &contextMenus);
   void activate();
   void deactivate(TextureSelectionDialog &textureSelectionDialog);
};


//!------------------------------NEW PROJECT DIALOG------------------------------


class NewProjectDialog
 {
 private:
    
    AppMaterialModifiers appMaterialModifiers;
 public:
    Panel panel;
    Box box;
    
    DialogControl dialogControl;

    //Constructors
    NewProjectDialog();
    NewProjectDialog(ColorPalette colorPalette,AppMaterialModifiers& appMaterialModifiers);

    //Public member functions
    void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer,
                Project &project,bool &greetingDialogActive,bool &startScreen);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
 {
 private:
    
    
 public:
    Panel panel;
    Box box;
    
    DialogControl dialogControl;

    //Constructors
    ExportDialog();
    ExportDialog(ColorPalette colorPalette);

    //Public member functions
    void render(ColorPalette colorPalette,Timer timer,TextRenderer &textRenderer,
                Project &project,bool &greetingDialogActive,MaterialEditorDialog &materialEditorDialog);
 };


//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showTextureSelectionDialog(Texture& txtr, Filter& filter, int displayingTextureRes, bool filterSelection);
bool wasTextureSelectionDialogActive();

class TextureSelectionDialog
{
private:
    ColorPalette colorPalette;
    
    int selectedTextureMode = 0;
public:
   Panel bgPanel;
   Panel subPanel;
   Panel selectedTextureDisplayingPanel;
   Panel textureSelectingPanel;

   Texture displayingTexture;
   int selectedTextureIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50,50,0.95f);
   glm::vec2 scale = glm::vec2(40); 
   DialogControl dialogControl;

   //Constructors
   TextureSelectionDialog();
   TextureSelectionDialog( ColorPalette colorPalette);


   //Public member functions
   void show(Timer &timer, glm::mat4 guiProjection, Texture& receivedTexture, Filter& receivedFilter, TextRenderer& textRenderer, int displayingTextureRes, bool filterSelection);

   void generateDisplayingTexture(Texture& txtr, Filter filter, int displayingTextureRes, bool filterSelection);

};

//!------------------------------CONTEXT MENU------------------------------


class ContextMenu
{
private:
    
public:
    Panel contextPanel;
    glm::vec3 pos;
    int selectedElement = 0;
    DialogControl dialogControl; 
    
    //Constructors
    ContextMenu();
    ContextMenu(ColorPalette colorPalette,std::vector<std::string> elements);
    
    /// @brief Public member function to render the context menu
    void render(Timer &timer,TextRenderer &textRenderer);
};

//!-------------------------------LOG DIALOG--------------------------------

namespace LGDLOG {
   class LogMsg {
   public:
      // Constructor
      LogMsg() = default;
      
      // Overload << operator for custom manipulations
      template <typename T>
      LogMsg& operator<<(const T& value) {
         // Convert value to string and add to the internal buffer
         buffer << value;

         std::string a;
         std::ostringstream outputBuffer; // Use istringstream to store input
         outputBuffer << value;
         a = outputBuffer.str();

         if(a == "$#")
            std::cout << std::endl;
         else
            std::cout << a;

         return *this;

      }

      // Overload >> operator for input
      LogMsg& operator>>(std::string& input) {
         // Read input from the user and store it in 'input'
         input = buffer.str();
         return *this;
      }

      // Converts the internal buffer to a string
      std::string str() const {
         return buffer.str();
      }

   private:
      std::ostringstream buffer;
   };

   extern LogMsg start;
   extern std::string end;
}

#define TEXTURE_UPDATING_ACTION 0
#define TEXTURE_DELETION_ACTION 1
#define TEXTURE_ADDITION_ACTION 2
#define NODE_ACTION 3

void registerTextureAction(const std::string title, const Texture icon, Texture texture);
void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerNodeAction(const std::string title, const Texture icon);

class LogDialog
{
 private:
   
   AppMaterialModifiers appMaterialModifiers;
 
 public:
   Panel panel;
   Button logBtn;
   Button logBtnR;
   Button logBtnL;

   glm::vec2 pos = glm::vec2(50.f);

   float panelXAxisMixVal = 0.f;
   float panelYAxisMixVal = 0.f;

   bool messagesActive = false;
   bool actionHistoryActive = false;

   DialogControl dialogControl;

   //Constructors
   LogDialog();
   LogDialog(ColorPalette colorPalette,AppMaterialModifiers& appMaterialModifiers);

   //Public member functions
   void render(ColorPalette colorPalette,Timer timer, TextRenderer &textRenderer);
};

#endif