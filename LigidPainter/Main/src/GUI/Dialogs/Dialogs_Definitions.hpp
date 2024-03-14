
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

#include <string>
#include <sstream>
#include <iostream>
#include <vector>


#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"

//Forward declarations
class ContextMenu;
class LogDialog;
class TextureField;
class MaterialSelectionDialog;

class DialogControl
{
private:
public:
   bool active = false;
   float mixVal = 0.;
   bool firstFrameActivated = false;
   bool globalFirstFrameActivated = false;

   DialogControl();
   DialogControl(bool active);
   
   void updateStart();
   void updateEnd(Timer &timer, float transitionDuration);
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
   std::vector<Section> systemSettings;
   std::vector<Section> TDRendererSettings;
   Panel panel;
   Panel bgPanel;
   DialogControl dialogControl;

   int selectedSettingsMode = 0;

   //Constructor
   SettingsDialog(){}
   SettingsDialog(int);

   //Public member functions    
   void show(Timer& timer, Painter& painter);

   /*! @brief Assignes the setting values to elements*/
   void setPropertiesToDialog();
   /*! @brief Assignes the elements to setting values */
   void setDialogToProperties();

private:
   void renderPanel(Timer& timer);
   void updateInfoTexts(Painter& painter, Timer& timer);

};


//!------------------------------NEW TEXTURE DIALOG------------------------------


class NewTextureDialog
{
private:
   
public:
     
   Panel panel;
     
   DialogControl dialogControl;

   //Constructor
   NewTextureDialog(){}
   NewTextureDialog(int);
    
   //Public member function
   void show(Timer& timer);
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
   
   DialogControl dialogControl;

   //Constructors
   LoadProjectDialog(){}
   LoadProjectDialog(int);
   
   //Public member functions
   void show(Timer& timer, Project &project);
 };


//!------------------------------ DISPLAYER DIALOG ------------------------------


class DisplayerDialog
 {
 public:
    Panel panel;
    
    DialogControl dialogControl;
    std::vector<Element> skyboxes;

    //Constructors
    DisplayerDialog(){}
    DisplayerDialog(int);
    
    //Public member functions
    void show(Timer& timer, Skybox &skybox);
 };

//!------------------------------ MODEL INFO DIALOG ------------------------------

class ModelInfoDialog
{
public:
   Panel panel;
    
   DialogControl dialogControl;

   //Constructors
   ModelInfoDialog(){}
   ModelInfoDialog(int);
    
   //Public member functions
   void show(Timer& timer, Model* model);
};


//!------------------------------ BAKING DIALOG ------------------------------


class BakingDialog
{
public:

   int rendererMode = 0;
   float rendererModeMixVal = 0.f;
   Button modeImageDisplayer1;
   Button modeImage1ElementsBg;
   Button modeImageDisplayer2;
   Button modeImage2ElementsBg;
   Button renderModeLBtn;
   Button renderModeRBtn;
   CheckBox pbrResultCheckbox;
   CheckBox aoCheckbox;
   CheckBox normalMapCheckbox;
   CheckBox vertexPositionCheckbox;
   CheckBox vertexNormalsCheckbox;
   CheckBox reflectanceCheckbox;
   Button selectMeshButton;
   Button bakeButton;
   
   short selectedBakeMode = 0;  

   DialogControl dialogControl;

   //Constructors
   BakingDialog(){}
   BakingDialog(int);
   
   //Public member functions
   void show(Timer& timer, Skybox skybox);

   /*! @brief Returns true if the dialog was hovered*/   
   bool isDialogHovered();

private: 
   /*! @brief Renders each elements inside of the dialog and handles their interactions */
   void renderElements(Timer& timer, glm::vec3 pos, glm::vec2 scale, glm::vec3 resultPos, glm::vec2 resultScale);
   
   /*! @brief Returns the baked texture*/   
   Texture bake(Skybox skybox, unsigned int resolution);
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
   TextureEditorDialog(){}
   TextureEditorDialog(int);
   
   //Public member functions
   void show(Timer& timer, Skybox &skybox, Texture* receivedTexture);

   void updateDisplayingTexture(Texture* receivedTexture, unsigned int destTxtr);
 };


//!------------------------------GREETING DIALOG DIALOG------------------------------


class GreetingDialog
{
public:
   Button loadProjectButton;
   Button createProjectButton;
   Button newVersionBannerButton;
   Button downloadNewVersionButton;
   Button textureDisplayerButton;
   Button textButton1;
   Panel bgPanel;
   
   DialogControl dialogControl;

   bool startScreen = true;
   
   //Constructors
   GreetingDialog(){}
   GreetingDialog(int);
   
   //Public member functions
   void show(Timer& timer, Project& project);
};

//!------------------------------MATERIAL EDITOR DIALOG------------------------------


class MaterialEditorDialog
{
private:
   //Private member functions
   void checkTextureSelectionDialog(Material &material);
   Model* getDisplayModel();
   void renderShortcutPanel(Timer& timer, bool mouseTrackingFlag, Material* material);
   void renderLayerPanel(Timer& timer, bool mouseTrackingFlag, Material* material);
   void renderModifiersPanel(Timer& timer, bool mouseTrackingFlag, Material* material);
   void renderSkyboxTxtr();
   void renderZoomIndicator(Timer& timer, bool mouseTrackingFlag);
   void renderNavPanel(Timer& timer, bool mouseTrackingFlag);
   void positioningPanels();

   bool prevUpdateTheMaterial = false;
public:

   bool updateTheMaterial = false;
   
   DialogControl dialogControl;
   Button materialDisplayer; //That buttons displays the material with a texture
   Panel bgPanel; //To cover the bg
   Panel layerPanel; //
   Panel modifiersPanel; //
   Panel shortcutPanel; //
   Panel navPanel; //
   ComboBox displayModeComboBox;
   ComboBox displayTxtrResComboBox;
   Button barButton; //
   TextBox shortcutRenamingTextbox;
   int shortcutRenamingIndex = -1;
   int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting TODO REMOVE THAT
   int selectedMaterialModifierIndex = 0;
   Panel zoomPanel;
   float zoomPanelMixVal = 0.f;

   Button twoDModelModeBtn;
   Button matDisplayerBallModeBtn;
   Button customModelModeBtn;
   int selectedModelModeIndex = 1;
   
   Button overallResultMode;
   Button selectedModifierResultMode;
   int selectedResultModeIndex = 0;

   Framebuffer displayingFBO;
   Skybox skybox;
   Framebuffer skyboxFBO;

   Camera displayerCamera;
    
   //Constructors
   MaterialEditorDialog(){}
   MaterialEditorDialog(int);

   //Public member functions
   void show(Timer &timer, Material* material);
   void activate();
   void deactivate();
   void updateSkyboxTxtr();

};


//!------------------------------NEW PROJECT DIALOG------------------------------


class NewProjectDialog
{
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
   NewProjectDialog(){}
   NewProjectDialog(int);

   //Public member functions
   void show(Timer& timer, Project &project);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
{
public:
   Panel panel;
   Panel subPanel;
   Panel propertiesPanel;

   Section libraryTexturesSection;
   Section libraryMaterialsSection;

   int activeSection = 0; 
    
   DialogControl dialogControl;

   //Constructors
   ExportDialog(){}
   ExportDialog(int);

   //Public member functions
   void show(Timer& timer, Project &project);
 };

//!------------------------------OBJECT TEXTURING DIALOG------------------------------


class ObjectTexturingDialog
{
public:
   Panel panel;
     
   DialogControl dialogControl;

   Button maskViaFaceSelection;
   Button maskViaTexture;
   Button cancelMasks;

   Button maskMaterialBtn;
   
   Button ctrlInfoBtn;
   Button shiftInfoBtn;
   
   Button assignRelatedTexturesButton;
   Button undoButton;
   
   Button materialDisplayerButton;
   Button editMaterialButton;
   Button selectMaterialButton;
   Button defaultMaterialButton;
   CheckBox albedoChannelCheckBox;
   CheckBox roughnessChannelCheckBox;
   CheckBox metallicChannelCheckBox;
   CheckBox normalMapChannelCheckBox;
   CheckBox heightMapChannelCheckBox;
   CheckBox aoChannelCheckBox;
   ComboBox mixOptionsComboBox;
   
   ComboBox displayingOptionsComboBox;

   Panel elementSelectionPanel;

   Camera sceneCam;
   Texture displayingTexture;
   Framebuffer displayingFBO;

   Texture meshMask;

   bool faceSelectionMode = false;
   std::vector<FaceSelection> faceSelection;

   std::vector<Mesh> texturesMesh;

   bool materialSelection = false;
   bool textureSelection = false;

   Material material;

   //Constructor
   ObjectTexturingDialog(){}
   ObjectTexturingDialog(int);
    
   //Public member function
   void show(Timer& timer);
   bool anyElementHover();

   void updateDisplayingTexture();
   void updateMeshTextures();
   int getResolution();
 };

//!------------------------------MATERIAL DISPLAYER------------------------------

class MaterialDisplayerDialog
 {
 public:
   Panel panel;
   
   DialogControl dialogControl;
   
   Framebuffer displayingFBO;
   Camera displayingCam;

   //Constructors
   MaterialDisplayerDialog(){}
   MaterialDisplayerDialog(int);

   //Public member functions
   void show(Timer& timer, Material material);
 };

//!------------------------------ MATERIAL SELECTION DIALOG ------------------------------

class MaterialSelectionDialog
{
public:
   Panel bgPanel;
   Panel matModePanel;
   Panel matDisplayerPanel;
   Panel selectedMatPanel;
   
   DialogControl dialogControl;
   
   Framebuffer displayingFBO;
   Camera displayingCam;

   int selectedMatMode = 0;
   int selectedMatIndex = -1;

   //Constructors
   MaterialSelectionDialog(){}
   MaterialSelectionDialog(int);

   //Public member functions
   void show(Timer& timer, Material* material);

private:
   void updateMaterialTypes();
   bool renderMatDisplayer(Timer& timer);
   void assignMaterialsToMap();
   void updateSelectedMaterialInPanel();

};


//!------------------------------FILTER DISPLAYER------------------------------

class FilterDisplayerDialog
{
public:
   Panel panel;
   
   DialogControl dialogControl;
   
   Filter filter;

   //Constructors
   FilterDisplayerDialog(){}
   FilterDisplayerDialog(int);

   //Public member functions
   void show(Timer& timer);
};

//!------------------------------DROPPER DIALOG------------------------------

class DropperDialog
{
public:
   DialogControl dialogControl;

   //Constructors
   DropperDialog(){}
   DropperDialog(int);

   //Public member functions
   void show(Timer& timer, Color* color);
};


//!------------------------------TEXTURE SELECTION DIALOG------------------------------

class TextureSelectionDialog
{
private:
   int selectedTextureMode = 1;
   void selectTheTexture(Texture& receivedTxtr, int displayingTextureRes, int displayMode, bool defaultProperties);
   void renderPanels(Timer& timer);
   void updateTextureModesPanel(bool twoDMode);
   void updateProceduralDisplayingTexturesArray(bool twoDMode);
   void initTextureSelectionDialog(Texture& receivedTexture, bool twoDMode);

   int activeSelectedTextureDisplayingMode = 0;


public:
   Panel bgPanel;
   
   Panel textureModesPanel;
   Panel subPanel;
   Panel subPanelTxtrPack;
   Panel subPanelSmartTextures;
   
   Panel selectedTextureDisplayingPanel;
   Panel textureSelectingPanel;

   Button selectedTextureSolidDisplayingModeBtn;
   Button selectedTextureMaterialBallDisplayingMode;
   Button selectedTextureCustomMeshDisplayingMode;

   Button zoomInBtn;
   Button zoomOutBtn;
   Button zoomValDisplayer;
   float zoomVal = 1;
   RangeBar rotationRangeBar;

   Panel smartPositionTexturePanel;
   bool smartPositionTexturePanelActive = false;
   Panel smartStripesTexturePanel;
   bool smartStripesTexturePanelActive = false;
   Panel edgeWearTexturePanel;
   bool edgeWearTexturePanelActive = false;
   Panel smartDistanceTexturePanel;
   bool smartDistanceTexturePanelActive = false;

   std::vector<Texture> proceduralDisplayingTextures;

   Texture displayingTexture;
   int selectedTextureIndex = 0; 
   bool clicked = false; 
   glm::vec3 pos = glm::vec3(50,50,0.95f);
   glm::vec2 scale = glm::vec2(40); 
   DialogControl dialogControl;

   //Constructors
   TextureSelectionDialog(){}
   TextureSelectionDialog(int);

   //Public member functions
   void show(Timer &timer, Texture& receivedTexture, int displayingTextureRes, bool twoDMode);
};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

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
   FilterSelectionDialog(){}
   FilterSelectionDialog(int);

   //Public member functions
   void show(Timer &timer, Filter& receivedFilter, int displayingTextureRes);
};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

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
   MeshSelectionDialog(){}
   MeshSelectionDialog(int);

   //Public member functions
   void show(Timer &timer, int& selectedMeshI);
};

//!------------------------------BRUSH MODIFICATION DIALOG------------------------------

class BrushModificationDialog
{
private:
    int selectedTextureMode = 0;
public:
   Panel bgPanel;
   Panel libraryBrushSelectionPanel;
   Button libraryBrushSelectionButton;
   Button brushDisplayBtn;

   DialogControl dialogControl;

   //Constructors
   BrushModificationDialog(){}
   BrushModificationDialog(int);

   //Public member functions
   void show(Timer &timer, BrushProperties* brushProperties);

   void updateDisplayingTexture(BrushProperties* brushProperties);
   void inToOut(BrushProperties* brushProperties);
   void outToIn(BrushProperties* brushProperties);
   void applyDefaultBrushProperties(BrushProperties* brushProperties);
   void cancelChanges(BrushProperties* brushProperties);
};

//!------------------------------TEXTURE SELECTION DIALOG------------------------------

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
   TexturePackEditorDialog(){}
   TexturePackEditorDialog(int);

   //Public member functions
   void show(Timer &timer, TexturePack& receivedTexturePack);
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

//!------------------------------ PROJECT RECOVER ------------------------------

class ProjectRecoverDialog
{
public:
   Panel panel;

   Panel projectPanel;
   Panel project_texturesPanel;
   Panel project_materialsPanel;
   Panel project_brushesPanel;
   Button projectPanelSelectBtn;
   Button projectPanelExitBtn;
   Button projectPanelFileExplorerBtn;

   bool projectSelectionMode = false;

   Button lpIconBtn;
   Button recoverTitleBtn;
   Button recoverInfoBtn;
   Button recover1Btn;
   Button recover2Btn;
   Button recover3Btn;

   DialogControl dialogControl;

   //Constructors
   ProjectRecoverDialog(){}
   ProjectRecoverDialog(int);

   //Public member functions
   void show(Timer& timer, Project &project);
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

      void clear(){
         buffer = std::ostringstream(""); 
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
#define VECTOR_ACTION 5
#define FACE_SELECTION_PAINTER_ACTION 6
#define FACE_SELECTION_OBJECTTEXTURING_ACTION 7
#define TEXTURE_IMAGE_EDITOR_ACTION 8
#define LIBRARY_ELEMENT_RENAMING 9
#define MATERIAL_DELETION_ACTION 10
#define MATERIAL_ADDITION_ACTION 11
#define BRUSH_DELETION_ACTION 12
#define BRUSH_ADDITION_ACTION 13
#define BRUSH_CHANGED_ACTION 14
#define VECTOR_ACTION_3D 15

#define HISTORY_LIBRARY_MODE 0
#define HISTORY_PAINTING_MODE 1
#define HISTORY_VECTORS_MODE 2
#define HISTORY_OBJECTSELECTION_MODE 3
#define HISTORY_FACESELECTION_MODE 4
#define HISTORY_TEXTUREFIELDS_MODE 5
#define HISTORY_MATERIALEDITOR_MODE 6 

// -- Library --
void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index);
void registerMaterialDeletionAction(const std::string title, const Texture icon, Material material, const int index);
void registerMaterialAdditionAction(const std::string title, const Texture icon, Material material, const int index);
void registerBrushDeletionAction(const std::string title, const Texture icon, Brush brush, const int index);
void registerBrushAdditionAction(const std::string title, const Texture icon, Brush brush, const int index);
void registerBrushChangedAction(const std::string title, const Texture icon, Brush brush, const int index);
void registerImageEditorAction(const std::string title, const Texture icon, Texture texture);
void registerLibraryElementRenamingAction(const std::string title, const Texture icon, const int index, const std::string element, const std::string name);

// -- Painting --
void registerPaintingAction(const std::string title, const Texture icon, Texture albedo, bool albedoPainted, 
                            Texture roughness, bool roughnessPainted, Texture metallic, bool metallicPainted,
                            Texture normal, bool normalPainted, Texture height, bool heightPainted, Texture ao,
                            bool aoPainted);

// -- Vector --
void registerVectorAction(const std::string title, std::vector<VectorStroke> vectorStrokes);
void registerVectorAction(const std::string title, std::vector<VectorStroke3D> vectorStrokes);

// -- Object Selection --
void registerObjectSelectionAction(const std::string title, int meshI, std::vector<int> selectedObjectIndices);

// -- Face Selection -- 
void registerFaceSelectionAction(const std::string title, std::vector<byte> primitivesArray, std::vector<byte> prevPrimArray, int meshI);
void registerFaceSelectionActionObjectTexturingDialog(const std::string title, std::vector<std::vector<byte>> primitivesArray, std::vector<std::vector<byte>> prevPrimArray);

// -- Texture Field --
void registerTextureFieldAction(const std::string title, std::vector<TextureField> fields);

// -- Material Editor --
void registerMaterialAction(const std::string title, Material material);


void registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton);
void registerNodeAction(const std::string title, const Texture icon);

class LogDialog
{
public:
   Panel messagesPanel;
   Panel historyPanel;
   
   Button logBtn;
   Button logBtnR;
   Button logBtnL;

   bool libraryHistoryMode = true;
   Button libraryHistoryBtn;
   Button otherHistoryBtn;

   Button yesBtn;
   Button noBtn;
   
   int activeHistoryMode = 0;

   Button messageInfoBtn;
   bool messageInfoActive = false;
   float messageInfoBtnMixVal = 0.f;
   size_t messageInfoBtnStartTime = 0.f;

   int cryCounter = 0;

   glm::vec2 pos = glm::vec2(54.531246, 4.722224);

   float messagesPanelXAxisMixVal = 0.f;
   float messagesPanelYAxisMixVal = 0.f;
   float historyPanelXAxisMixVal = 0.f;
   float historyPanelYAxisMixVal = 0.f;

   bool messagesActive = false;
   bool actionHistoryActive = false;

   bool windowShouldClose = false;

   DialogControl dialogControl;

   //Constructors
   LogDialog(){}
   LogDialog(int);

   //Public member functions
   void render(
               Timer timer, Painter& painter, std::vector<TextureField>& paintingOverTextureFields, Project &project
            );

   bool unded = false;
   void undo(Painter& painter, std::vector<TextureField>& paintingOverTextureFields);

   bool isHovered();
};