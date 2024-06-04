/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/Wait/Wait.hpp"

#include "3D/ThreeD.hpp"

#include "GUI/GUI.hpp"

#include <vector>
#include <string>
#include <filesystem>

std::vector<Texture> __textures;
std::vector<Material> __materials;
std::vector<Brush> __brushes;
std::vector<Model> __TDModels;
std::vector<Filter> __filters;
std::vector<TexturePack> __texturePacks;

std::vector<SourceLibTexture> __sourceLibTextures; 

int __selectedElementIndex = 0;

bool __changed = true;

#define MAX_TEXTURES 40
#define MAX_MATERIALS 30
#define MAX_BRUSHES 40
#define MAX_MODELS 10
#define MAX_FILTERS 40
#define MAX_TEXTUREPACKS 10

void Library::reserve_vectors(){
    __textures.reserve(MAX_TEXTURES);
    __materials.reserve(MAX_MATERIALS);
    __brushes.reserve(MAX_BRUSHES);
    __TDModels.reserve(MAX_MODELS);
    __filters.reserve(MAX_FILTERS);
    __texturePacks.reserve(MAX_TEXTUREPACKS);
}

void Library::nameControl(){
    for (int i = __textures.size()-1; i >= 0; i--)
    {
        std::vector<std::string> texturesStr;
        for (size_t istr = 0; istr < __textures.size(); istr++)
        {
            if(i != istr)
                texturesStr.push_back(__textures[istr].title);
        }
        if(__textures[i].title == ""){
            __textures[i].title = "texture";
            __changed = true;
        }

        if(UTIL::uniqueName(__textures[i].title,texturesStr)){
            __changed = true;
        }    
    }

    for (int i = __materials.size()-1; i >= 0; i--)
    {
        std::vector<std::string> materialsStr;
        for (size_t istr = 0; istr < __materials.size(); istr++)
        {
            if(i != istr)
                materialsStr.push_back(__materials[istr].title);
        }
        if(__materials[i].title == ""){
            __materials[i].title = "material";
            __changed = true;
        }

        if(UTIL::uniqueName(__materials[i].title,materialsStr)){
            __changed = true;
        }    
    }

    for (int i = __brushes.size()-1; i >= 0; i--)
    {
        std::vector<std::string> brushesStr;
        for (size_t istr = 0; istr < __brushes.size(); istr++)
        {
            if(i != istr)
                brushesStr.push_back(__brushes[istr].title);
        }
        if(__brushes[i].title == ""){
            __brushes[i].title = "brush";
            __changed = true;
        }

        if(UTIL::uniqueName(__brushes[i].title,brushesStr)){
            __changed = true;
        }    
    }
    
    for (int i = __TDModels.size()-1; i >= 0; i--)
    {
        std::vector<std::string> TDModelsStr;
        for (size_t istr = 0; istr < __TDModels.size(); istr++)
        {
            if(i != istr)
                TDModelsStr.push_back(__TDModels[istr].title);
        }
        if(__TDModels[i].title == ""){
            __TDModels[i].title = "TDModel";
            __changed = true;
        }

        if(UTIL::uniqueName(__TDModels[i].title,TDModelsStr)){
            __changed = true;
        }    
    }
    
    for (int i = __filters.size() - 1; i >= 0; i--)
    {
        std::vector<std::string> filtersStr;
        for (size_t istr = 0; istr < __filters.size(); istr++)
        {
            if(i != istr)
                filtersStr.push_back(__filters[istr].title);
        }
        if(__filters[i].title == ""){
            __filters[i].title = "filter";
            __changed = true;
        }

        if(UTIL::uniqueName(__filters[i].title, filtersStr)){
            __changed = true;
        }    
    }

    for (int i = __texturePacks.size() - 1; i >= 0; i--)
    {
        std::vector<std::string> texturePacksStr;
        for (size_t istr = 0; istr < __texturePacks.size(); istr++)
        {
            if(i != istr)
                texturePacksStr.push_back(__texturePacks[istr].title);
        }
        if(__texturePacks[i].title == ""){
            __texturePacks[i].title = "texturePack";
            __changed = true;
        }

        if(UTIL::uniqueName(__texturePacks[i].title, texturePacksStr)){
            __changed = true;
        }    
    }
}

void Library::addTexture(Texture texture, const std::string actionTitle){
    if(__textures.size() < MAX_TEXTURES){
        __changed = true;
        
        if(actionTitle != "")
            dialog_log.registerTextureAction(actionTitle);
        
        texture.uniqueId = 0; 

        projectUpdatingThreadElements.updateTextures = true;

        __textures.push_back(texture);
        
        Library::textureGiveUniqueId(__textures.size() - 1);

        Library::nameControl();
    }

}

void Library::addMaterial(Material material, const std::string actionTitle){
    if(__materials.size() < MAX_MATERIALS){
        __changed = true;

        if(actionTitle != "")
            dialog_log.registerMaterialAction(actionTitle);
        
        //Give unique ID to the material
        __materials.push_back(material);
        
        // Update the displaying texture of the new material
        material_thread.update_material_displaying_texture(&__materials[__materials.size() - 1], getMaterialDisplayingModel(), &getMaterialDisplayingModel()->meshes[0], &getMaterialDisplayingModel()->meshes[0].material_channels, 1024);
        
        Library::nameControl();
    }
}

void Library::addBrush(Brush brush, const std::string actionTitle){
    if(__brushes.size() < MAX_BRUSHES){
        __changed = true;

        if(actionTitle != "")
            dialog_log.registerBrushAction(actionTitle);
        
        __brushes.push_back(brush);
        
        Library::nameControl();
    }
}

void Library::addModel(Model model){
    if(__TDModels.size() < MAX_MODELS){
        __changed = true;

        __TDModels.push_back(model);
        
        Library::nameControl();
    }
}

void Library::addFilter(Filter filter){
    if(__filters.size() < MAX_FILTERS){
        __changed = true;
        
        __filters.push_back(filter);
        
        Library::nameControl();
    }
}

void Library::addTexturePack(TexturePack texturePack){
    if(__texturePacks.size() < MAX_TEXTUREPACKS){
        __changed = true;
        
        __texturePacks.push_back(texturePack);
        
        Library::nameControl();
    }
}

void Library::eraseTexture   (int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());
    
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        return;
    }

    __changed = true;

    dialog_log.registerTextureAction("Texture deleted");
        
    __textures.erase(__textures.begin() + index);

    projectUpdatingThreadElements.updateTextures = true;
}

void Library::eraseMaterial  (int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());
    
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the material : Requested material index is out of boundaries." << LGDLOG::end;
        return;
    }
    
    __changed = true;

    dialog_log.registerMaterialAction("Material deleted");

    //__materials[index].deleteBuffers();
    
    __materials.erase(__materials.begin() + index);
}

void Library::eraseBrush(int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        return;
    }

    __changed = true;

    dialog_log.registerBrushAction("Brush deleted");

    /*
    glDeleteTextures(1, &__brushes[index].displayingTexture.ID);
    glDeleteTextures(1, &__brushes[index].texture.ID);
    */
    __brushes.erase(__brushes.begin() + index);
}

void Library::eraseModel     (int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the model : Requested model index is out of boundaries." << LGDLOG::end;
        return;
    }
          
    __changed = true;
    
    glDeleteTextures(1, &__TDModels[index].displayingTxtr.ID);
    
    for (size_t mshI = 0; mshI < __TDModels[index].meshes.size(); mshI++)
    {
        /*
        glDeleteVertexArrays(1, &__TDModels[index].meshes[mshI].VAO);
        glDeleteBuffers(1, &__TDModels[index].meshes[mshI].VBO);
        glDeleteBuffers(1, &__TDModels[index].meshes[mshI].EBO);
        */

        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.albedo.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.roughness.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.metallic.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.normalMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.heightMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].material_channels.ambientOcclusion.ID);

        glDeleteTextures(1, &__TDModels[index].meshes[mshI].uvMask.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].displayingTxtr.ID);
    }

    __TDModels.erase(__TDModels.begin() + index);
}

void Library::eraseFilter     (int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    if(index >= __filters.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the filter : Requested filter index is out of boundaries." << LGDLOG::end;
        return;
    }

    __changed = true;
    
    glDeleteTextures(1, &__filters[index].displayingTxtr.ID);
    glDeleteProgram(__filters[index].shader.ID);

    __filters.erase(__filters.begin() + index);
}

void Library::eraseTexturePack     (int index){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    if(index >= __texturePacks.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the texture pack : Requested texture pack index is out of boundaries." << LGDLOG::end;
        return;
    }

    for (size_t i = 0; i < __texturePacks[index].textures.size(); i++)
    {
        glDeleteTextures(1, &__texturePacks[index].textures[i].ID); 
    }

    __changed = true;
    
    __texturePacks.erase(__texturePacks.begin() + index);
}

void Library::clearTextures   (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __textures.size(); i++)
    {
        glDeleteTextures(1, &__textures[i].ID);
    }

    __textures.clear();

    projectUpdatingThreadElements.updateTextures = true;
}

void Library::clearMaterials  (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __materials.size(); i++)
    {
        __materials[i].deleteBuffers();
    }

    __materials.clear();
}

void Library::clearBrushes     (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __brushes.size(); i++)
    {
        glDeleteTextures(1, &__brushes[i].displayingTexture.ID);
        glDeleteTextures(1, &__brushes[i].properties.brushTexture.ID);
    }
    
    __brushes.clear();
}

void Library::clearModels     (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __TDModels.size(); i++)
    {
        glDeleteTextures(1, &__TDModels[i].displayingTxtr.ID);

        for (size_t mshI = 0; mshI < __TDModels[i].meshes.size(); mshI++)
        {
            /*
            glDeleteVertexArrays(1, &__TDModels[i].meshes[mshI].VAO);
            glDeleteBuffers(1, &__TDModels[i].meshes[mshI].VBO);
            glDeleteBuffers(1, &__TDModels[i].meshes[mshI].EBO);
            */

            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.albedo.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.roughness.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.metallic.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.normalMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.heightMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].material_channels.ambientOcclusion.ID);
            
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].uvMask.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].displayingTxtr.ID);
        }
    }

    __TDModels.clear();
}

void Library::clearFilters     (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __filters.size(); i++)
    {
        glDeleteTextures(1, &__filters[i].displayingTxtr.ID);
        glDeleteProgram(__filters[i].shader.ID);
    }

    __filters.clear();
}

void Library::clearTexturePacks     (){
    // Wait until threads are not in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    __changed = true;
    
    for (size_t i = 0; i < __texturePacks.size(); i++)
    {
        for (size_t j = 0; j < __texturePacks[i].textures.size(); j++)
        {
            glDeleteTextures(1, &__texturePacks[i].textures[j].ID);
        }
    }

    __texturePacks.clear();
}

void Library::changeSelectedElementIndex(int newI){
    __changed = true;
    
    __selectedElementIndex = newI;
}

int Library::getSelectedElementIndex(){
    return __selectedElementIndex;
}

bool Library::isChanged(){
    return __changed;
}

void Library::setChanged(bool state){
    __changed = state;
}

Texture* Library::getTexture(int index){
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        Texture a = Texture();
        return &a;
    }
    return &__textures[index];
}

Texture Library::getTextureObj(int index){
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        Texture a = Texture();
        return a;
    }
    return __textures[index];
}

Material* Library::getMaterial(int index){
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the material : Requested material index is out of boundaries." << LGDLOG::end;
        Material a = Material();
        return &a;
    }
    return &__materials[index];
}

Material Library::getMaterialObj(int index){
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the material : Requested material index is out of boundaries." << LGDLOG::end;
        Material a = Material();
        return a;
    }
    return __materials[index];
}

Brush* Library::getBrush(int index){
    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        Brush a = Brush();
        return &a;
    }
    return &__brushes[index];
}
Brush Library::getBrushObj(int index){
    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        Brush a = Brush();
        return a;
    }
    return __brushes[index];
}

Model* Library::getModel(int index){
    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the model : Requested model index is out of boundaries." << LGDLOG::end;
        Model a = Model();
        return &a;
    }
    return &__TDModels[index];
}
Model Library::getModelObj(int index){
    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the model : Requested model index is out of boundaries." << LGDLOG::end;
        Model a = Model();
        return a;
    }
    return __TDModels[index];
}

Filter* Library::getFilter(int index){
    if(index >= __filters.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the filter : Requested filter index is out of boundaries." << LGDLOG::end;
        Filter a = Filter();
        return &a;
    }
    return &__filters[index];
}
Filter Library::getFilterObj(int index){
    if(index >= __filters.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the filter : Requested filter index is out of boundaries." << LGDLOG::end;
        Filter a = Filter();
        return a;
    }
    return __filters[index];
}

TexturePack* Library::getTexturePack(int index){
    if(index >= __texturePacks.size()){
        LGDLOG::start << "ERROR! : Couldn't get the texture pack : Requested texture pack index is out of boundaries." << LGDLOG::end;
        TexturePack a = TexturePack();
        return &a;
    }
    return &__texturePacks[index];
}
TexturePack Library::getTexturePackObj(int index){
    if(index >= __texturePacks.size()){
        LGDLOG::start << "ERROR! : Couldn't get the texture pack : Requested texture pack index is out of boundaries." << LGDLOG::end;
        TexturePack a = TexturePack();
        return a;
    }
    return __texturePacks[index];
}

int Library::getTextureArraySize(){
    return __textures.size();
}
int Library::getMaterialArraySize(){
    return __materials.size();
}
int Library::getBrushArraySize(){
    return __brushes.size();
}
int Library::getModelArraySize(){
    return __TDModels.size();
}
int Library::getFilterArraySize(){
    return __filters.size();
}
int Library::getTexturePackArraySize(){
    return __texturePacks.size();
}

void Library::textureGiveUniqueId(int index){
	std::vector<int> IDArray;

	for (size_t i = 0; i < __textures.size(); i++)
	{
		IDArray.push_back(__textures[i].uniqueId);
	}
	
	UTIL::giveUniqueId(__textures[index].uniqueId, IDArray);
}

std::vector<Texture>* Library::getTextureVectorPointer(){
    return &__textures;
}
std::vector<Material>* Library::getMaterialVectorPointer(){
    return &__materials;
}
std::vector<Brush>* Library::getBrushVectorPointer(){
    return &__brushes;
}

void Library::loadSourceLibTextures(){
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Texture Library/Low_Res_Display_Textures")) {
            SourceLibTexture srcLibTxtr;
            srcLibTxtr.load(entry.path().string());
            __sourceLibTextures.push_back(srcLibTxtr);
        }            
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 788632" << ex.what() << LGDLOG::end;
    }
}

SourceLibTexture Library::getSrcLibTxtr(int index){
    return __sourceLibTextures[index];
}

int Library::getgetSrcLibTxtrsArraySize(){
    return __sourceLibTextures.size();
}