
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
    unsigned int ID;
    Texture icon;
    Texture texture;
    Texture alteredTexture;
    int textureIndex;
    Material material;
    Brush brush;
    
    std::string element;
    std::string name;

    LibraryAction(std::string title, unsigned int ID, Texture icon, int index, std::string element, std::string name){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->textureIndex = index;
        this->element = element;
        this->name = name;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
    }

    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture, Texture alteredTexture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->alteredTexture = alteredTexture;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->textureIndex = textureIndex;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Material material, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->material = material;
        this->textureIndex = textureIndex;
    }

    LibraryAction(std::string title, unsigned int ID, Texture icon, Brush brush, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->brush = brush;
        this->textureIndex = textureIndex;
    }
};

struct PaintingAction{
    std::string title;
    Texture icon;

    bool multiChannelAction = false;

    Texture albedo; 
    bool albedoPainted;
    Texture roughness; 
    bool roughnessPainted;
    Texture metallic; 
    bool metallicPainted;
    Texture normal; 
    bool normalPainted;
    Texture height; 
    bool heightPainted;
    Texture ao; 
    bool aoPainted;

    PaintingAction(
                        std::string title, Texture icon, Texture texture 
                    )
    {
        multiChannelAction = false;

        this->title = title;
        this->icon = icon;
        this->albedo = texture;
    }

    PaintingAction(
                        std::string title, Texture icon,
                        Texture albedo, bool albedoPainted, 
                        Texture roughness, bool roughnessPainted, 
                        Texture metallic, bool metallicPainted,
                        Texture normal, bool normalPainted, 
                        Texture height, bool heightPainted, 
                        Texture ao, bool aoPainted
                    )
    {
        multiChannelAction = true;

        this->title = title;
        this->icon = icon;
        
        this->albedo = albedo; 
        this->roughness = roughness; 
        this->metallic = metallic; 
        this->normal = normal; 
        this->height = height; 
        this->ao = ao; 

        this->albedoPainted = albedoPainted; 
        this->roughnessPainted = roughnessPainted; 
        this->metallicPainted = metallicPainted; 
        this->normalPainted = normalPainted; 
        this->heightPainted = heightPainted; 
        this->aoPainted = aoPainted; 

    }
};

struct VectorsAction{
    std::string title;
    std::vector<VectorStroke> vectorStrokes;
    std::vector<VectorStroke3D> vectorStrokes3D;
    unsigned int ID;

    VectorsAction(){}

    VectorsAction(std::string title, unsigned int ID, std::vector<VectorStroke> vectorStrokes){
        this->title = title;
        this->vectorStrokes = vectorStrokes;
        this->ID = ID;
    }
    
    VectorsAction(std::string title, unsigned int ID, std::vector<VectorStroke3D> vectorStrokes){
        this->title = title;
        this->vectorStrokes3D = vectorStrokes;
        this->ID = ID;
    }
};

struct FaceSelectionAction{
    std::string title;
    std::vector<byte> primitivesArray;
    std::vector<std::vector<byte>> primitivesArray_M;
    int meshI;
    unsigned int ID;

    FaceSelectionAction(){}

    FaceSelectionAction(std::string title, unsigned int ID, std::vector<byte> primitivesArray, int meshI){
        this->title = title;
        this->ID = ID;
        this->primitivesArray = primitivesArray;
        this->meshI = meshI;
    }
    
    FaceSelectionAction(std::string title, unsigned int ID, std::vector<std::vector<byte>> primitivesArray_M){
        this->title = title;
        this->ID = ID;
        this->primitivesArray_M = primitivesArray_M;
    }
};

struct TextureFieldsAction{
    std::string title;
    std::vector<TextureField> fields;

    TextureFieldsAction(){}

    TextureFieldsAction(std::string title, std::vector<TextureField> fields){
        this->title = title;
        this->fields = fields;
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

#endif