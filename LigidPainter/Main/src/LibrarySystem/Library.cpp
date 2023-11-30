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

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"

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

void Library::addTexture(Texture texture){
    __changed = true;
    
    registerTextureAdditionAction("Texture added", Texture(), texture, __textures.size());

    texture.uniqueId = 0; 

    projectUpdatingThreadElements.updateTextures = true;

    texture.copyDataToTheCopyContext();

    __textures.push_back(texture);
    
    Library::textureGiveUniqueId(__textures.size() - 1);

    Library::nameControl();
}

void Library::addMaterial(Material material){
    __changed = true;
    
    material.uniqueID = 0; 

    //Give unique ID to the material

    __materials.push_back(material);
    Library::materialGiveUniqueId(__materials.size() - 1);
    
    Library::nameControl();
}

void Library::addBrush(Brush brush){
    __changed = true;
    
    __brushes.push_back(brush);
    
    Library::nameControl();
}

void Library::addModel(Model model){
    __changed = true;

    __TDModels.push_back(model);
    
    Library::nameControl();
}

void Library::addFilter(Filter filter){
    __changed = true;
    
    __filters.push_back(filter);
    
    Library::nameControl();
}

void Library::addTexturePack(TexturePack texturePack){
    __changed = true;
    
    __texturePacks.push_back(texturePack);
    
    Library::nameControl();
}

void Library::eraseTexture   (int index){
    
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        return;
    }

    registerTextureDeletionAction("Texture Deletion", Texture(), __textures[index], index);

    __changed = true;
    
    /*
    mainThreadUsingCopyContext = true;
    getCopyContext()->window.makeContextCurrent();
    glDeleteTextures(1, &__textures[index].copyContextID);

    getContext()->window.makeContextCurrent();
    glDeleteTextures(1, &__textures[index].ID);
    mainThreadUsingCopyContext = false;
    */

    __textures.erase(__textures.begin() + index);

    projectUpdatingThreadElements.updateTextures = true;
}

void Library::eraseMaterial  (int index){
    
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the material : Requested material index is out of boundaries." << LGDLOG::end;
        return;
    }
    
    __changed = true;

    __materials[index].deleteBuffers();
    
    __materials.erase(__materials.begin() + index);
}

void Library::eraseBrush     (int index){

    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        return;
    }
    __changed = true;

    glDeleteTextures(1, &__brushes[index].displayingTexture.ID);
    glDeleteTextures(1, &__brushes[index].texture.ID);
    
    __brushes.erase(__brushes.begin() + index);
}

void Library::eraseModel     (int index){
    
    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the model : Requested model index is out of boundaries." << LGDLOG::end;
        return;
    }
          
    __changed = true;
    
    glDeleteTextures(1, &__TDModels[index].displayingTxtr.ID);
    
    for (size_t mshI = 0; mshI < __TDModels[index].meshes.size(); mshI++)
    {
        glDeleteVertexArrays(1, &__TDModels[index].meshes[mshI].VAO);
        glDeleteBuffers(1, &__TDModels[index].meshes[mshI].VBO);
        glDeleteBuffers(1, &__TDModels[index].meshes[mshI].EBO);
        
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].albedo.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].roughness.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].metallic.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].normalMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].heightMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].ambientOcclusion.ID);

        glDeleteTextures(1, &__TDModels[index].meshes[mshI].uvMask.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].displayingTxtr.ID);

        glDeleteTextures(1, &__TDModels[index].meshes[mshI].selectedObjectPrimitivesTxtr.ID);
    }

    __TDModels.erase(__TDModels.begin() + index);
}

void Library::eraseFilter     (int index){
    
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
    __changed = true;
    
    mainThreadUsingCopyContext = true;
    getCopyContext()->window.makeContextCurrent();
    for (size_t i = 0; i < __textures.size(); i++)
    {
        glDeleteTextures(1, &__textures[i].copyContextID);
    }
    getContext()->window.makeContextCurrent();
    for (size_t i = 0; i < __textures.size(); i++)
    {
        glDeleteTextures(1, &__textures[i].ID);
    }
    mainThreadUsingCopyContext = false;
    
    __textures.clear();

    projectUpdatingThreadElements.updateTextures = true;
}

void Library::clearMaterials  (){
    __changed = true;
    
    for (size_t i = 0; i < __materials.size(); i++)
    {
        __materials[i].deleteBuffers();
    }

    __materials.clear();
}

void Library::clearBrushes     (){
    __changed = true;
    
    for (size_t i = 0; i < __brushes.size(); i++)
    {
        glDeleteTextures(1, &__brushes[i].displayingTexture.ID);
        glDeleteTextures(1, &__brushes[i].texture.ID);
    }
    
    __brushes.clear();
}

void Library::clearModels     (){
    __changed = true;
    
    for (size_t i = 0; i < __TDModels.size(); i++)
    {
        glDeleteTextures(1, &__TDModels[i].displayingTxtr.ID);

        for (size_t mshI = 0; mshI < __TDModels[i].meshes.size(); mshI++)
        {
            glDeleteVertexArrays(1, &__TDModels[i].meshes[mshI].VAO);
            glDeleteBuffers(1, &__TDModels[i].meshes[mshI].VBO);
            glDeleteBuffers(1, &__TDModels[i].meshes[mshI].EBO);

            glDeleteTextures(1, &__TDModels[i].meshes[mshI].albedo.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].roughness.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].metallic.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].normalMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].heightMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].ambientOcclusion.ID);
            
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].uvMask.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].displayingTxtr.ID);
            
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].selectedObjectPrimitivesTxtr.ID);
        }
    }

    __TDModels.clear();
}

void Library::clearFilters     (){
    __changed = true;
    
    for (size_t i = 0; i < __filters.size(); i++)
    {
        glDeleteTextures(1, &__filters[i].displayingTxtr.ID);
        glDeleteProgram(__filters[i].shader.ID);
    }

    __filters.clear();
}

void Library::clearTexturePacks     (){
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

void Library::materialGiveUniqueId(int index){
	std::vector<int> IDArray;

	for (size_t i = 0; i < __materials.size(); i++)
	{
		IDArray.push_back(__materials[i].uniqueID);
	}

	UTIL::giveUniqueId(__materials[index].uniqueID, IDArray);
}

std::vector<Texture>* Library::getTextureVectorPointer(){
    return &__textures;
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

