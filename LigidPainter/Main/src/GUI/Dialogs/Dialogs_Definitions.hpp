
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "3D/ThreeD.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Settings/Settings.hpp"

//Forward declarations
class LogDialog;
class TextureField;
class MaterialSelectionDialog;

class DialogControl
{
private:
   bool loop_mode = false;

public:
   bool active = false;
   float mixVal = 0.;
   bool firstFrameActivated = false;
   bool globalFirstFrameActivated = false;

   DialogControl();
   DialogControl(bool active);
   
   void updateStart(bool loop_mode);
   void updateEnd(Timer &timer, float transitionDuration);
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
   void show(Timer& timer);

   /*! @brief Assignes the setting values to elements*/
   void setPropertiesToDialog();
   /*! @brief Assignes the elements to setting values */
   void setDialogToProperties();

private:
   void renderPanel(Timer& timer);
   void updateInfoTexts(Timer& timer);

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
   void show(Timer& timer);
 };


//!------------------------------ DISPLAYER DIALOG ------------------------------


class DisplayerDialog
{
public:
   Panel panel;
   Panel bg_txtr_selection_panel;

   Button quit_bg_txtr_selection_btn;

   bool bg_txtr_selection_mode = false;
   float bg_txtr_selection_mode_mixVal = 0.f;

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

   void generate_result(Texture* receivedTexture, Texture destTxtr);
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
   void show(Timer& timer);
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
   Material* currently_edited_materialPtr = nullptr;
      
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

   Texture displayingTexture;
   Skybox skybox;
   Texture skyboxDisplayingTexture;

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
   void show(Timer& timer);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
{
public:
   Panel panel;
   Panel subPanel;

   Panel libraryTexturesPanel;
   Panel libraryMaterialsPanel;
   Panel layersPanel;

   DialogControl dialogControl;

   //Constructors
   ExportDialog(){}
   ExportDialog(int);

   //Public member functions
   void show(Timer& timer);
 };

//!------------------------------OBJECT TEXTURING DIALOG------------------------------


class ObjectTexturingDialog
{
public:
     
   DialogControl dialogControl;

   Panel panel;

   CheckBox renderUnselectedFacesCheckBox;
   Button maskMaterialBtn;
   CheckBox displayMaskCheckBox;
   Button assignMaterialToRelatedLayerButton;
   
   CheckBox albedoChannelCheckBox;
   CheckBox roughnessChannelCheckBox;
   CheckBox metallicChannelCheckBox;
   CheckBox normalMapChannelCheckBox;
   CheckBox heightMapChannelCheckBox;
   CheckBox aoChannelCheckBox;
   
   ComboBox displayingOptionsComboBox;
   ComboBox displayBuffersComboBox;

   Camera sceneCam;
   Texture displayingTexture;

   Texture meshMask;

   Material material;

   //Constructor
   ObjectTexturingDialog(){}
   ObjectTexturingDialog(int);
    
   //Public member function
   void show(Timer& timer, Mesh& mesh, PaintingLayer* painting_layer, const unsigned int resolution);

   void updateDisplayingTexture(Mesh& mesh, PaintingLayer* painting_layer);
   void updateMeshTextures(Mesh& mesh, PaintingLayer* painting_layer, const unsigned int resolution);
   int getResolution();
 };

//!------------------------------MATERIAL DISPLAYER------------------------------

class MaterialDisplayerDialog
{
public:
   Panel panel;
   
   DialogControl dialogControl;
   
   Texture displayingTexture;
   Camera displayingCam;

   //Constructors
   MaterialDisplayerDialog(){}
   MaterialDisplayerDialog(int);

   //Public member functions
   void show(Timer& timer, Material* material);
};

//!------------------------------HEIGHT MAP DISPLAYER------------------------------

class HeightMapDisplayerDialog
{
public:
   Button display1;
   Button display2;

   Button exit_btn;

   RangeBar strength_rangebar;
   
   Camera display1_cam;
   Camera display2_cam;
   
   DialogControl dialogControl;

   //Constructors
   HeightMapDisplayerDialog(){}
   HeightMapDisplayerDialog(int);

   //Public member functions
   void show(Timer& timer, Model* model);
};

//!------------------------------TEXTURE DISPLAYER------------------------------

class DisplayTextureDialog
 {
 public:
   Panel panel;
   Button threeD_display_btn;
   Button twoD_display_btn;

   DialogControl dialogControl;
   
   Camera displayingCam;

   //Constructors
   DisplayTextureDialog(){}
   DisplayTextureDialog(int);

   //Public member functions
   void show(Timer& timer, Texture texture);
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
   
   Texture displayingTexture;
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
   Material* get_selected_material();

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


//!------------------------------RENAMING DIALOG------------------------------

class RenamingDialog
{
private:
   TextBox rename_textbox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,1.5f), "", 0.f, false);
public:
   DialogControl dialogControl;

   //Constructors
   RenamingDialog(){}
   RenamingDialog(int);

   //Public member functions
   void show(Timer& timer, glm::vec2 pos, float width, std::string* title, int max_char_count);
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
   void show(Timer& timer);
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

#define HISTORY_PAINTING_MODE 0
#define HISTORY_MULTI_CHANNEL_PAINTING_MODE 1
#define HISTORY_VECTORS_MODE 2
#define HISTORY_FACESELECTION_MODE 4
#define HISTORY_TEXTUREFIELDS_MODE 5
#define HISTORY_MATERIALEDITOR_MODE 6 

#define CAT_EMOTION_DEFAULT 0
#define CAT_EMOTION_CRYING 1
#define CAT_EMOTION_ROCK 2
#define CAT_EMOTION_THINKING 3
#define CAT_EMOTION_DIZZY 4
#define CAT_EMOTION_SLEEPY 5
#define CAT_EMOTION_RANDOM 6
#define CAT_EMOTION_RELAXED 7
#define CAT_EMOTION_THINKING 8
#define CAT_EMOTION_SMILING 9

#include "./GUI/Dialogs/LogDialog/HistoryActions/HistoryActions.hpp"

class LogDialog
{
private:
   size_t lastMessagesSize = 0;

   float petPoints = 0.f; 
   bool sleepingCat = false;
   size_t sleepingCounter = 0;
   int flipCount = 0;

   /*! @brief Cat is clicked & messages, history and stuff are revealed. */
   bool menu_mode = false;
   /*! @brief Goes from 0 to 1 gradually as menu_mode flag set to true. Goes 1 to 0 as the menu_mode set to false back. (Used for smooth transitions for menu_mode)*/
   float menu_mode_mix_val = 0.f;
   /*! @brief Menu bar that displays modes such as : Messages, general history, library history, multi-threading info*/
   Panel menu_bar;

   /*! @brief Change the face of the cat. Emotion param takes anything from CAT_EMOTION_*desired emotion* */
   void assert_emotion(Timer& timer, const unsigned int emotion, const int duration);

   /*! @brief Make the cat talk whatever text says*/
   void make_cat_talk(Timer& timer, std::string text, const int duration);

   /*! @brief Returns true if cat is currently talking*/
   bool is_cat_talking();

   /*! @brief Generate texts and make the cat talk if needed*/
   void talking_interaction(Timer& timer);

   /*---------- UTIL -----------*/
   
   /*! @brief Removes the text data from LGDLOG::start and moves it to the this->messages*/
   void update_messages_array(Timer& timer);

   Panel messagesPanel;
   Panel historyPanel;
   Panel libraryHistoryPanel;
   Panel multiThreadInfoPanel;
   Button logBtn; // The cat
   /*! @brief Render the cat and panels and stuff. Anything related to graphics*/
   void render_elements(Timer& timer);

public:
   HistoryActionRecords history_action_records;
   
   /*! @brief All the messages generated in the app using LGDLOG::start is stored here.*/
   std::vector<std::string> messages;

   /*! @brief Position of the cat*/
   glm::vec2 pos = glm::vec2(54.531246, 4.722224);

   /*! @brief Indicates if the application should be closed. Controlled by getContext()::window::shouldClose()*/
   bool windowShouldClose = false;

   //Constructors
   LogDialog(){}
   LogDialog(int);

   // -- Library --
   void registerTextureAction(const std::string title);
   void registerMaterialAction(const std::string title);
   void registerBrushAction(const std::string title);
   void registerTextureModificationAction(const std::string title, Texture texture);

   // -- Painting --
   void registerPaintingAction(const std::string title, Texture painted_texture);

   void registerMultiChannelPaintingAction(const std::string title, MaterialChannels material_channels);

   // -- Vector --
   void registerVectorAction(const std::string title, VectorScene* vectorScene);

   // -- Face Selection -- 
   void registerFaceSelectionAction(const std::string title, Mesh* mesh);

   // -- Texture Field --
   void registerTextureFieldAction(const std::string title, TextureFieldScene* textureFieldScene);

   // -- Material Editor --
   void registerMaterialEditorAction(const std::string title, Material* material);


   void registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton);
   void registerNodeAction(const std::string title, const Texture icon);

   /*! @brief Render the log dialog for a single frame*/
   void render(Timer& timer);

   void undo_general_history();

   /*! @brief Returns the current active history mode related to current state of the LigidPainter. Returns -1 if no mode is selected*/
   int get_active_history_mode(); 

   /*! @brief Returns true if the log dialog is hovered*/
   bool isHovered();

   bool any_thread_in_progress();
};