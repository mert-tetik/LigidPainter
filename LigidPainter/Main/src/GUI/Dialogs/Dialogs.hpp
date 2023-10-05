
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
   Button systemSettingsBtn; 
   Button TDRendererSettingsBtn; 
   Section systemSettings;
   Section TDRendererSettings;
   Panel panel;
   DialogControl dialogControl;

   int selectedSettingsMode = 0;

   //Constructor
   SettingsDialog();

   //Public member functions    
   void render(Timer timer, Painter& painter);
};


//!------------------------------NEW TEXTURE DIALOG------------------------------


class NewTextureDialog
{
private:
   
public:
     
   Panel panel;
     
   DialogControl dialogControl;

   //Constructor
   NewTextureDialog();
    
   //Public member function
   void render(Timer timer);
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
   
   AppMaterialModifiers appMaterialModifiers;
   DialogControl dialogControl;

   //Constructors
   LoadProjectDialog();
   LoadProjectDialog(AppMaterialModifiers& appMaterialModifiers);
   
   //Public member functions
   void render(Timer timer,
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
    
    //Public member functions
    void render(Timer timer,
                Skybox &skybox);
 };


//!------------------------------ BAKING DIALOG ------------------------------


class BakingDialog
{
public:
   Panel bgPanel;
   Panel exportChannelsIntoLibraryPanel;
   Panel selectMeshPanel;
   CheckBox pbrResultCheckbox;
   CheckBox pbrUseLightingCheckbox;
   CheckBox pbrRayTracingCheckbox;
   CheckBox vertexPositionCheckbox;
   CheckBox vertexNormalsCheckbox;
   CheckBox reflectanceCheckbox;
   Button bakeButton;
   
   Button meshSelectionText;
   Button exportMaterialChannelsText;
   Button bakingMaterialChannelsText;

   short selectedBakeMode = 0;  


   DialogControl dialogControl;
   std::vector<Element> skyboxes;

   //Constructors
   BakingDialog();
   
   //Public member functions
   void render(Timer timer, Skybox skybox);
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
   std::vector<Element> coloringElements;
   std::vector<Element> normalMapElements;
   std::vector<Element> distortionElements;
   std::vector<Element> imagesElements;

   Button saveButton;
   Button saveAsButton;
   Button maskTextureButton;
   
   Button filterBtn;
   Button textureBtn;
   Filter filter;

   Button displayerBtn;

   Texture displayingTexture;

   int selectedSection = 0;

   //Constructors
   TextureEditorDialog();
   
   //Public member functions
   void render(Timer timer, Skybox &skybox, glm::mat4 projection, Texture receivedTexture);

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
   
   DialogControl dialogControl;

   bool startScreen = true;
   
   //Constructors
   GreetingDialog();
   
   //Public member functions
   void render(Timer timer,
               NewProjectDialog &newProjectDialog,LoadProjectDialog &loadProjectDialog);
};

//!------------------------------MATERIAL EDITOR DIALOG------------------------------


class MaterialEditorDialog
{
private:
   //Private member functions
   void updateLayerPanel(Material &material);
   void checkLayerPanel(Material &material);
   void checkModifiersPanel(Material &material, TextureSelectionDialog &textureSelectionDialog);
   void updateLayerPanelElements(Material &material);
   void checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material);
   void manageContextMenuActions( Material &material);

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

   //Public member functions
   void render(Timer &timer,TextureSelectionDialog &textureSelectionDialog,
               Material &material);
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

   Button propertiesBtn;
   Button tdModelsBtn;
   Button sectionsBg;
   
   int activeSection = 0;
   Button activeSectionIndicator;
   float activeSectionIndicatorMixVal = 0.f;

   Button createBtn;

   Button titleTextboxText;
   Button pathTextboxText;
   TextBox titleTextbox;
   TextBox pathTextbox;
   ComboBox resolutionCombobox;

   Panel tdModelsPanel;
   Button addTDModelBtn;
   Button deleteTDModelBtn;
    
   DialogControl dialogControl;

   //Constructors
   NewProjectDialog();
   NewProjectDialog(AppMaterialModifiers& appMaterialModifiers);

   //Public member functions
   void render(Timer timer,
               Project &project,bool &greetingDialogActive,bool &startScreen);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
{
private:
   
   
public:
   Panel panel;
   Panel subPanel;
   Panel propertiesPanel;

   Section meshNodeSection;
   Section libraryTexturesSection;
   Section libraryMaterialsSection;

   int activeSection = 0; 
    
   DialogControl dialogControl;

   //Constructors
   ExportDialog();

   //Public member functions
   void render(Timer timer,Project &project,bool &greetingDialogActive,MaterialEditorDialog &materialEditorDialog);
 };

//!------------------------------MATERIAL DISPLAYER------------------------------

class MaterialDisplayerDialog
 {
 public:
   Panel panel;
   
   DialogControl dialogControl;
   
   Material material;

   Camera displayingCam;

   //Constructors
   MaterialDisplayerDialog();

   //Public member functions
   void render(Timer timer);
 };


//!------------------------------MATERIAL DISPLAYER------------------------------

class FilterDisplayerDialog
{
public:
   Panel panel;
   
   DialogControl dialogControl;
   
   Filter filter;

   //Constructors
   FilterDisplayerDialog();

   //Public member functions
   void render(Timer timer, glm::mat4 projection);
};


//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showTextureSelectionDialog(Texture& txtr, int displayingTextureRes, bool twoDMode);
bool wasTextureSelectionDialogActive();

class TextureSelectionDialog
{
private:
   int selectedTextureMode = 0;
   void selectTheTexture(Texture& receivedTxtr, int displayingTextureRes);
   void renderPanels(Timer& timer, glm::mat4 guiProjection);

public:
   Panel bgPanel;
   
   Panel textureModesPanel;
   Panel subPanel;
   Panel subPanelTxtrPack;
   Panel subPanelSmartTextures;
   
   Panel selectedTextureDisplayingPanel;
   Panel textureSelectingPanel;

   Panel smartPositionTexturePanel;
   bool smartPositionTexturePanelActive = false;
   Panel smartStripesTexturePanel;
   bool smartStripesTexturePanelActive = false;
   Panel edgeWearTexturePanel;
   bool edgeWearTexturePanelActive = false;
   Panel smartDistanceTexturePanel;
   bool smartDistanceTexturePanelActive = false;

   Texture whiteTxtr;

   std::vector<Texture> smartTextureDisplayingTextures;

   Texture displayingTexture;
   int selectedTextureIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50,50,0.95f);
   glm::vec2 scale = glm::vec2(40); 
   DialogControl dialogControl;

   //Constructors
   TextureSelectionDialog();

   //Public member functions
   void show(Timer &timer, glm::mat4 guiProjection, Texture& receivedTexture, int displayingTextureRes, bool twoDMode);


};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showFilterSelectionDialog(Filter& filter, int displayingTextureRes);

class FilterSelectionDialog
{
public:
   Panel bgPanel;
   Panel subPanel;
   Panel selectedTextureDisplayingPanel;
   Panel textureSelectingPanel;

   /// @brief Used to display the selected filter without modifying the original filter
   Filter selectedFilter;

   Texture displayingTexture;
   int selectedFilterIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50,50,0.95f);
   glm::vec2 scale = glm::vec2(34.f); 
   DialogControl dialogControl;

   //Constructors
   FilterSelectionDialog();

   //Public member functions
   void show(Timer &timer, glm::mat4 guiProjection, Filter& receivedFilter, int displayingTextureRes);
};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showMeshSelectionDialog(int& selectedMeshI);

class MeshSelectionDialog
{
private:
    int selectedTextureMode = 0;
public:
   Panel bgPanel;
   Panel subPanel;

   Texture displayingTexture;
   int selectedMeshIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50.f, 48.f, 0.95f);
   glm::vec2 scale = glm::vec2(40); 
   DialogControl dialogControl;

   //Constructors
   MeshSelectionDialog();

   //Public member functions
   void show(Timer &timer, glm::mat4 guiProjection, int& selectedMeshI);
};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showTexturePackEditorDialog(TexturePack& texturePack);

class TexturePackEditorDialog
{
private:
    int selectedTextureMode = 0;
public:
   Panel bgPanel;
   Panel subPanel;
   Panel selectedTextureDisplayingPanel;
   Panel textureSelectingPanel;

   bool sapSpritesPanelActive = false;
   Panel sapSpritesPanel;

   Texture displayingTexture;
   int selectedTextureIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50,50,0.95f);
   glm::vec2 scale = glm::vec2(40); 
   DialogControl dialogControl;

   //Constructors
   TexturePackEditorDialog();

   //Public member functions
   void show(Timer &timer, glm::mat4 guiProjection, TexturePack& receivedTexturePack);
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
    ContextMenu(std::vector<std::string> elements);
    
    /// @brief Public member function to render the context menu
    void render(Timer &timer);
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
#define BUTTON_ACTION 4

void registerTextureAction(const std::string title, const Texture icon, Texture texture);
void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerNodeAction(const std::string title, const Texture icon);
void registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton);

class LogDialog
{
 private:
   
   AppMaterialModifiers appMaterialModifiers;
 
 public:
   Panel panel;
   Button logBtn;
   Button logBtnR;
   Button logBtnL;

   glm::vec2 pos = glm::vec2(54.531246, 4.722224);

   float panelXAxisMixVal = 0.f;
   float panelYAxisMixVal = 0.f;

   bool messagesActive = false;
   bool actionHistoryActive = false;

   DialogControl dialogControl;

   //Constructors
   LogDialog();
   LogDialog(AppMaterialModifiers& appMaterialModifiers);

   //Public member functions
   void render(Timer timer);

   bool isHovered();
};

#endif