
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef HISTORY_ACTIONS_HPP
#define HISTORY_ACTIONS_HPP

struct BtnAction{
    Button* button;
    Button previousBtn;
    Texture previousBtnTexture;

    BtnAction(){}
    BtnAction(Button* button, Button previousBtn, Texture previousBtnTexture){
        this->button = button; 
        this->previousBtn = previousBtn; 
        this->previousBtnTexture = previousBtnTexture; 
    }
};

struct LibraryAction{
    std::string title;
    unsigned int action_ID;

    Texture texture;    
    size_t unique_id;

    std::vector<Texture> textures;    
    std::vector<Material> materials;    
    std::vector<Brush> brushes;    
    

    LibraryAction(std::string title, std::vector<Texture> textures){
        this->title = title;
        this->textures = textures;

        this->action_ID = 0;
    }
    LibraryAction(std::string title, std::vector<Material> materials){
        this->title = title;
        this->materials = materials;

        this->action_ID = 1;
    }
    LibraryAction(std::string title, std::vector<Brush> brushes){
        this->title = title;
        this->brushes = brushes;

        this->action_ID = 2;
    }
    LibraryAction(std::string title, Texture texture){
        this->title = title;
        this->texture = texture;

        this->action_ID = 3;
    }
};

struct PaintingAction{
    std::string title;
    Texture painted_texture;

    size_t unique_ID = -1;

    PaintingAction(std::string title, Texture painted_texture);
};

struct MultiChannelPaintingAction{
    std::string title;
    MaterialChannels material_channels;

    size_t unique_ID = -1;

    MultiChannelPaintingAction(std::string title, MaterialChannels material_channels);
};

struct VectorsAction{
    std::string title;
    VectorScene vectorScene;

    VectorsAction(){}

    VectorsAction(std::string title, VectorScene vectorScene){
        this->title = title;
        this->vectorScene = vectorScene;
    }
};

struct FaceSelectionAction{
    std::string title;
    Mesh::FaceSelection face_selection_data;

    FaceSelectionAction(){}

    FaceSelectionAction(std::string title, Mesh::FaceSelection face_selection_data){
        this->title = title;
        this->face_selection_data = face_selection_data;
    }
};

struct TextureFieldsAction{
    std::string title;
    TextureFieldScene texture_field_scene;

    TextureFieldsAction(){}

    TextureFieldsAction(std::string title, TextureFieldScene texture_field_scene){
        this->title = title;
        this->texture_field_scene = texture_field_scene;
    }
};

struct MaterialEditorAction{
    std::string title;
    Material material;

    MaterialEditorAction(){}

    MaterialEditorAction(std::string title, Material material){
        this->title = title;
        this->material = material;
    }
};

struct HistoryActionRecords{
   std::map<VectorScene*, std::vector<VectorsAction>> actions_Vectors;
   void undo_vector_actions(); 
   VectorScene* get_active_vectorScene();
   
   std::map<TextureFieldScene*, std::vector<TextureFieldsAction>> actions_TextureFields;
   void undo_texture_field_actions();
   TextureFieldScene* get_active_textureFieldScene();
   
   std::map<Mesh*, std::vector<FaceSelectionAction>> actions_FaceSelection;
   void undo_face_selection_actions(); 
   Mesh* get_active_face_selected_mesh();

   std::map<unsigned int, std::vector<PaintingAction>> actions_Painting;
   void undo_painting_actions(); 
   
   std::map<unsigned int, std::vector<MultiChannelPaintingAction>>  actions_MultiChannelPainting;
   MaterialChannels get_actively_painted_material_channels();
   void undo_multi_channel_painting_actions(); 


   std::vector<LibraryAction> actions_Library;
   void undo_library_actions(); 
   

   
   std::vector<MaterialEditorAction> actions_MaterialEditor;
   void undo_material_editor_actions(); 
};

#endif