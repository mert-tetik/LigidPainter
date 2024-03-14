
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */

#include "GUI/Dialogs/Dialogs.hpp"

MaterialEditorDialog dialog_materialEditor;
LogDialog dialog_log;
GreetingDialog dialog_greeting;
NewProjectDialog dialog_newProject;
ExportDialog dialog_export;
MaterialDisplayerDialog dialog_materialDisplayer;
FilterDisplayerDialog dialog_filterDisplayer;
DropperDialog dialog_dropper;
LoadProjectDialog dialog_loadProject;
TextureSelectionDialog dialog_textureSelection;
BakingDialog dialog_baking;
FilterSelectionDialog dialog_filterSelection;
NewTextureDialog dialog_newTexture;
SettingsDialog dialog_settings;
DisplayerDialog dialog_displayer;
ModelInfoDialog dialog_modelInfo;
TextureEditorDialog dialog_textureEditor;
TexturePackEditorDialog dialog_texturePackEditor;
ProjectRecoverDialog dialog_projectRecover;
ObjectTexturingDialog dialog_objectTexturing;
MaterialSelectionDialog dialog_materialSelection;
MeshSelectionDialog dialog_meshSelection;
BrushModificationDialog dialog_brushModification;

void init_dialogs(){
   dialog_materialEditor = MaterialEditorDialog(0);
   dialog_log = LogDialog(0);
   dialog_greeting = GreetingDialog(0);
   dialog_newProject = NewProjectDialog(0);
   dialog_export = ExportDialog(0);
   dialog_materialDisplayer = MaterialDisplayerDialog(0);
   dialog_filterDisplayer = FilterDisplayerDialog(0);
   dialog_dropper = DropperDialog(0);
   dialog_loadProject = LoadProjectDialog(0);
   dialog_textureSelection = TextureSelectionDialog(0);
   dialog_baking = BakingDialog(0);
   dialog_filterSelection = FilterSelectionDialog(0);
   dialog_newTexture = NewTextureDialog(0);
   dialog_settings = SettingsDialog(0);
   dialog_displayer = DisplayerDialog(0);
   dialog_modelInfo = ModelInfoDialog(0);
   dialog_textureEditor = TextureEditorDialog(0);
   dialog_texturePackEditor = TexturePackEditorDialog(0);
   dialog_projectRecover = ProjectRecoverDialog(0);
   dialog_objectTexturing = ObjectTexturingDialog(0);
   dialog_materialSelection = MaterialSelectionDialog(0);
   dialog_meshSelection = MeshSelectionDialog(0);
   dialog_brushModification = BrushModificationDialog(0);
}