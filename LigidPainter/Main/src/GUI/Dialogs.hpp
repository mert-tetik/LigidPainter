/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include "GUI/Dialogs/Dialogs_Definitions.hpp"

extern MaterialEditorDialog dialog_materialEditor;
extern LogDialog dialog_log;
extern GreetingDialog dialog_greeting;
extern NewProjectDialog dialog_newProject;
extern ExportDialog dialog_export;
extern MaterialDisplayerDialog dialog_materialDisplayer;
extern FilterDisplayerDialog dialog_filterDisplayer;
extern DropperDialog dialog_dropper;
extern LoadProjectDialog dialog_loadProject;
extern TextureSelectionDialog dialog_textureSelection;
extern BakingDialog dialog_baking;
extern FilterSelectionDialog dialog_filterSelection;
extern NewTextureDialog dialog_newTexture;
extern SettingsDialog dialog_settings;
extern DisplayerDialog dialog_displayer;
extern ModelInfoDialog dialog_modelInfo;
extern TextureEditorDialog dialog_textureEditor;
extern TexturePackEditorDialog dialog_texturePackEditor;
extern ProjectRecoverDialog dialog_projectRecover;
extern ObjectTexturingDialog dialog_objectTexturing;
extern MaterialSelectionDialog dialog_materialSelection;
extern MeshSelectionDialog dialog_meshSelection;
extern BrushModificationDialog dialog_brushModification;

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

int show_context_menu(Timer &timer, std::vector<std::string> elements);

#endif