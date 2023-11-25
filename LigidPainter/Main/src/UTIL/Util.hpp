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

#ifndef UTILS_HPP
#define UTILS_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>


#include "ShaderSystem/Shader.hpp"

//forward declerations :

/// @brief forward declared Library AppTextures structure
struct AppTextures;
/// @brief forward declared Model class
class Model;
/// @brief forward declared Texture class
class Texture;
/// @brief forward declared Material class
class Material;
/// @brief forward declared Brush class
class Brush;
/// @brief forward declared Node class
class Node;
/// @brief forward declared Scene struct
struct Scene;
/// @brief forward declared Panel class
class Panel;
/// @brief forward declared Mesh class
class Mesh;
/// @brief forward declared AppMaterialModifiers struct
struct AppMaterialModifiers;
/// @brief forward declared Context struct
struct Context;
/// @brief forward declared Box class
class Box;
/// @brief forward declared Section struct
struct Section;



/// @brief Global utility functions to help to do stuff
namespace UTIL{
    
    /// @brief get the percentage of the value
    /// @param value the value
    /// @param percent % value
    /// @return value / 100 * percent
    float getPercent(float value, float percent);
    
    /// @brief get the percentage of the value in vec2
    /// @param value the value in vec2
    /// @param percent % value in vec2
    /// @return value / 100 * percent for each axis
    glm::vec2 getPercent(glm::vec2 value, glm::vec2 percent);
    
    /// @brief get the percentage of the value in vec3
    /// @param value the value in vec3
    /// @param percent % value in vec3
    /// @return value / 100 * percent for each axis
    glm::vec3 getPercent(glm::vec3 value, glm::vec3 percent);
    
    /// @brief finds the biggest index of the 'del' char in the 's' string and returns the string in the right side of the 'del' char
    /// @param s 'aaa/bbb/ccc'
    /// @param del : '/' 
    /// @return 'ccc'
    std::string getLastWordBySeparatingWithChar(std::string s, char del);
    
    /// @brief finds the biggest index of the 'del' char in the 's' string and removes the string in the right side of the 'del' char
    /// @param s 'aaa/bbb/ccc'
    /// @param del : '/' 
    /// @return 'aaa/bbb/' 
    std::string removeLastWordBySeparatingWithChar(std::string s, char del);
    
    /// @brief removes an extension from the given param
    /// @param s MyImage.png
    /// @return MyImage
    std::string removeExtension(std::string s);

    /// @brief returns the extension from the given param
    /// @param s MyImage.png
    /// @return png
    std::string getFileExtension(std::string s);

    /// @brief Transforms the uppercase characters into lowercase in the given str param.
    std::string toLowercase(const std::string& str);

    /// @brief removes the starting path from the full path
    /// @param startingPath ./AAA/BBB/
    /// @param fullPath ./AAA/BBB/CCC/Dest.tmp
    /// @return CCC/Dest.tmp
    std::string rmvPath(std::string startingPath, std::string fullPath);
    
    /// @brief opens the path param in the file explorer
    /// @param path a path to a directory
    void openInFileExplorer(const char* path);
    
    /// @brief generates an unique ID based on otherIDs vector(unique to these values) and assigns the value to the ID reference variable   
    /// @param ID 
    /// @param otherIDs 
    void giveUniqueId(int &ID ,const std::vector<int> otherIDs);

    /// @brief Converts std::vector<std::string> to char* const* (char**)
    /// @param strings 
    /// @return 
    char* const* convertStringArray(const std::vector<std::string> strings);

    /// @brief the folder distinguisher 
    /// @return '/' if unix based OS defined '\\' if the windows defined
    char folderDistinguisher();

    /// @brief deletes all the file system objects in the given folder path
    /// @param folderPath 
    bool deleteFilesInFolder(const std::string folderPath);

    /// @brief duplicates the folder in the @param src to the @param dest path
    void duplicateFolder(const std::string src, const std::string dest);

    /// @brief moves the files inside of the src folder to the dst folder
    void moveFilesToDestination(const std::string& src, const std::string& dst);

    /// @brief generates unique value into the param s according to the sArray
    /// @return if change is made 
    bool uniqueName(std::string &s, std::vector<std::string> sArray);

    /// @brief Returns the index of the param "s" in the param "array" 
    int findCorrespondingIndex(const std::string& s, const std::vector<std::string> array);

    /// @brief Returns absolute environment specific app data folder path
    ///        For instance : "C:\Users<Username>\AppData\Roaming\" for Windows OS
    ///        Always returns the path with the '/' or '\' at the end of the string
    std::string environmentSpecificAppDataFolderPath();

    /// @brief Replaces all the '/' characters with the '\' character in the Windows OS 
    void correctFolderDistinguishers(std::string& path);

    /// @brief Copies the param file into the param folder
    ///        Renames the file if already has one
    /// @param mode 0 = abort if already exists, 1 = rename the file if already exists, 2 = delete the existing file if already exists 
    void copyFileToFolder(const std::string file, const std::string folder, int mode);
}



/// @brief Manages color.
/// Definitions are in the : UTIL/Color
class Color
{
private:
    /*
     --- util color convertion functions ---
        !Inputs & Outputs are not normalized
    */
    glm::vec3 rgbToHsv(glm::vec3 rgb);
    glm::vec3 hsvToRgb(glm::vec3 hsv);
    glm::vec3 hexToRgb(std::string hex);
    std::string rgbToHex(glm::vec3 rgb);
    glm::vec3 hexToHsv(std::string hex);
    std::string hsvToHex(glm::vec3 hsv);


    /// @brief The color value in the range of 0 - 255
    glm::vec3 priv_rgb;

public:

    /// @brief @return the Red, Green, Blue value of the color in the range of 0 - 255 
    glm::vec3 getRGB();  
    
    /// @brief @return the Hue, Saturation, Value value of the color in the range of hsv values (Hue : 0 - 360, others : 0 - 100) 
    glm::vec3 getHSV(); 
    
    /// @brief @return the Red, Green, Blue value of the color in the range of 0 - 1 
    glm::vec3 getRGB_normalized();  
    
    /// @brief @return the hex value of the color (starts with '#') 
    std::string getHEX();

    /// @brief Default constructor
    Color();

    /// @brief Constructor (loads @param rgbColor in the range of 0 - 255)
    Color(glm::vec3 rgbColor);
    
    /// @brief Constructor (loads @param hexColor)
    Color(std::string hexColor);

    /// @brief loads a rgb value in the range of 0 - 255
    void loadRGB(glm::vec3 value);
    
    /// @brief loads a rgb value in the range of 0 - 1
    void loadRGB_normalized(glm::vec3 value);

    /// @brief loads a hsv value in the range of hsv values (Hue : 0 - 360, others : 0 - 100)  
    void loadHSV(glm::vec3 value);

    /// @brief loads a hex value
    void loadHex(std::string value);
};

class Timer
{
private:
public:
    double lastTimeT = 0;
    int nbFramesT = 0;
    
    size_t seconds = 0; //How many seconds has passed since the program started
    double mostHighFPSValue = 1.;
    double FPS = 1.;
    double renderingSpeed = 0.;
    
    Timer(/* args */);
    
    bool runTimer(float cycleTime);
    void transition(bool state ,float &value,float timeInSeconds);
};

struct ProceduralProperties{
    /// @brief Indicates which procedural texture shader function will be used (indexing the getProcedural function in the Shaders/Include/Procedural.frag)
    ///         -1 : Using the proceduralTextureID
    ///         1000 : Using the ID (Smart texture selected)
    int proceduralID = -1;
    float proceduralScale = 1.f;
    int proceduralnverted = 0;
    bool proceduralNormalMap = false;
    bool proceduralNormalGrayScale = false;
    float proceduralNormalStrength = 10.f;
    unsigned int proceduralTextureID = 0;
    bool proceduralUseTexCoords = false;
    bool proceduralGrayScale = false;
    float proceduralBrightness = 1.f;
    glm::vec4 smartProperties;

    float txtrPackScale = 1.f;
    float txtrPackCount = 1.f;
    float txtrPackRotation_Jitter = 1.f;
    float txtrPackSize_Jitter = 1.f;
    float txtrPackOpacity_Jitter = 1.f;
    float txtrPackScatter = 1.f;

    int textureSelectionDialog_selectedTextureIndex = 0;
    int textureSelectionDialog_selectedMode = 0;
};



class Texture
{
private:
    // --- util functions --- 

    std::string generateTMPTitle();
    std::string getTMPTitleWithGreatestIndex();

public:
    /// @brief OpenGL texture buffer object id
    unsigned int ID = 0;

    /// @brief OpenGL texture buffer object id for the copy context in order to use for multithreading
    ///        Generally used for the library elements
    unsigned int copyContextID = 0;

    ProceduralProperties proceduralProps;

    /// @brief Title of the texture (myTexture)
    std::string title = "";

    /// @brief unique id (unique to the other texture elements in the library.textures)
    ///        (used to generate tmp file names)
    int uniqueId;

    /// @brief default constructor (ID is generated when @ref load is called)
    Texture();
    
    /// @brief create a texture class with existing OpenGL texture buffer object id
    Texture(unsigned int ID);
    
    /// @brief Create a texture class using texture data & generate OpenGL texture buffer object id (channels is always rgba)
    /// @param pixels pixels of the texture
    /// @param w width
    /// @param h height
    Texture(char* pixels, int w, int h);
    Texture(char* pixels, int w, int h, unsigned int filterParam);
    Texture(char* pixels, int w, int h, unsigned int filterParam, int proceduralID);
    Texture(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat);

    void update(char* pixels, int w, int h);
    void update(char* pixels, int w, int h, unsigned int filterParam);
    void update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format);
    void update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned internalFormat);

    /// @brief Load a texture by importing the texture in the given path via STBI
    bool load(const char* path);

    /// @brief Load & resize a texture in the given texture resolution param by importing the texture in the given path via STBI
    bool load(const char* path, glm::ivec2 textureResolution);

    /// @brief Returns texture data in the given path & doesn't write anything to the member variables
    unsigned char* getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip);
    
    /// @brief Exports the texture to the given directory path
    /// @param path folder path (will be completed as : @param path + / + @ref title + . + format (in lowercase))
    /// @param format is PNG JPEG BMP TGA
    bool exportTexture(std::string path,const std::string format);
    
    bool exportTexture(std::string path, unsigned char* pixels, glm::ivec2 scale, const std::string format);


    /// @brief writes the texture data to the given @param pixels parameter
    /// glm::ivec2 textureRes = txtr.getResolution();
    /// char* pixels = new pixels[textureRes.x * textureRes.y * 4];
    /// getData(pixels); //Write the data to the pixels variable
    void getData(char*& pixels);
    
    /// @brief @return the resolution value of the texture 
    glm::ivec2 getResolution();

    /// @brief creates a copy of the texture and returns the OpenGL texture buffer object ID of the duplicated texture.
    ///         Use like that : Texture newTexture(duplicatedTexture.duplicateTexture());
    unsigned int duplicateTexture();
    
    void duplicateTexture(Texture &txtr);
    
    /// @brief Write the tmp file of the texture from the tmp folder
    void writeTMP(std::string tmpTitle);
    
    /// @brief Read the tmp file of the texture from the tmp folder and delete the readen file
    void readTMP(std::string tmpTitle);

    std::vector<glm::vec3> getMaterialIDPalette();

    bool writeTextureData(std::ofstream& wf);
    
    bool readTextureData(std::ifstream& rf, bool threeDMode);

    void copyDataToTheCopyContext();


    // -------- Texture Manipulation --------

    /// @brief Expands the texture's boundaries according to the mesh uv mask
    void removeSeams(Mesh& mesh, int textureResolution);

    /// @brief Expands the texture's boundaries according to the mesh uv mask
    void removeSeams(Mesh& mesh, glm::ivec2 textureResolution);
    
    /// @brief Writes the normal map version of the texture into the normalMap param (normalMap has to be initialized) 
    void generateNormalMap(unsigned int& normalMap, int textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale);
    
    /// @brief Writes the normal map version of the texture into the normalMap param (normalMap has to be initialized) 
    void generateNormalMap(unsigned int& normalMap, glm::ivec2 textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode);
    
    /// @brief Writes the normal map version of the texture into itself
    void applyNormalMap(float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode);
    
    /// @brief Generates procedural texture result using the mesh param
    unsigned int generateProceduralTexture(Mesh &mesh, int textureRes);

    /// @brief Generates 2D displaying texture using the proceduralProps & writes the texture into the this->ID
    /// @param displayMode 0 : solid 2D, 1 : Material ball PBR, 2 : Custom mesh PBR
    void generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode);

    void flipTexture(bool horizontal, bool vertical);
};

class Brush
{
public:
    
    /*  --------- Brush Properties --------- */
        float spacing = 0.f;
        float hardness = 0.f;
        float sizeJitter = 0.f;
        float scatter = 0.f;
        float fade = 0.f;
        float rotation = 0.f;
        float rotationJitter = 0.f;
        float alphaJitter = 0.f;
        bool individualTexture = false;
        bool sinWavePattern = false;
        Texture texture; 
    /*  A brush class holds these values
      Then applies these values to the GUI  
        If clicked to a use brush button
    */

    /// @brief Title of the brush (smt like myBrush_1)    
    std::string title;
    
    /// @brief The OpenGL texture buffer used to display the brush 
    ///        (a curvy stroke created with the brush)
    ///         Is initialized once and have a certain resolution value (W : 100, H : 100, C : RGBA)
    Texture displayingTexture;
    

    /// @brief Default constructor (just initializes the @ref displayingTexture)
    Brush();
    
    /// @brief Initializes the @ref displayingTexture and assings the given params
    Brush(float displayRadius, float spacing, float hardness, float sizeJitter, float scatter, float fade, float rotation, 
          float rotationJitter, float alphaJitter, bool individualTexture, bool sinWavePattern, 
          std::string title, Texture texture);
    
    void update(float displayRadius, float spacing, float hardness, float sizeJitter, float scatter, float fade, float rotation, 
          float rotationJitter, float alphaJitter, bool individualTexture, bool sinWavePattern, 
          Texture texture);

    /// @brief Updates the displaying texture using brush properties.
    void updateDisplayTexture(float radius);

    /// @brief move brush properties to the painting panel
    void useBrush(Section &brushSection);

    /// @brief move painting panel brush properties to the class's brush properties
    void applyToBrush(Section &brushSection);
};

class Renderbuffer{
public:
    unsigned int ID, internalformat, attachment;
    
    Renderbuffer();
    Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution);
    Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples);

    void update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution);
    void update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples);
};

class Framebuffer{
public:
    unsigned int ID = 0;

    Texture colorBuffer;
    Renderbuffer renderBuffer;

    Framebuffer();
    Framebuffer(Texture colorBuffer, unsigned int textureTarget);
    Framebuffer(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer);
    void update(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer);


    void generate();
    void bind();
    void setColorBuffer(Texture colorBuffer, unsigned int textureTarget);
    void deleteBuffers(bool delColorBuffer, bool delRenderBuffer);
};

/// @brief Image filter library element. 
class Filter{ 
public:
    /// @brief Filtering shader
    Shader shader;
    std::string title;
    Texture displayingTxtr;
    /// @brief Strength value of the filter
    ///        (Sending to the shader as uniform float strength)
    float strength = 1.f;

    /// @brief Default constructor
    Filter();

    /// @brief Loads lgdfilter files and generates the displaying texture
    int load(std::string path);

    /// @brief Generates the displaying texture using the this->shader. 
    ///        Updates if already generated
    void generateDisplayingTexture(glm::vec2 displayResolution);

    /// @brief Applies the @ref (*this).shader to the given texture
    void applyFilter(unsigned int txtr, Texture maskTexture, Texture maskTexture2);

    bool writeFilterData(std::ofstream& wf);

    bool readFilterData(std::ifstream& rf);

    std::string srcCode = "";
};

/// @brief Responsible of holding and managing multiple relevant textures and a library element. 
class TexturePack{
public:
    /// @brief Title of the texture pack. For instance : NewTexturePack
    std::string title = "";

    /// @brief The textures inside of the texture pack
    std::vector<Texture> textures;

    /// @brief Default constructor
    TexturePack();

    /// @brief Loads a folder that contains the textures
    int load(std::string path);

    /// @brief Applies the textures to the given texture
    void apply(Texture txtr, float scale, float count, float rotationJitter, float sizeJitter, float opacityJitter, float allignmentCorruption);

    /// @brief Locates and saperates the sprites in the given param txtr.
    ///        Uses the alpha map's red channel if the param alphaMap's OpenGL texture buffer object ID is not set to 0 
    void saperateSprites(Texture txtr, Texture alphaMap);

    /// @brief Generates a single sprite texture using the this->textures
    Texture generateSpriteTexture();
};

class Project
{
public:
    /// @brief Folder path of the project 
    ///        (AAA/MyProject)
    ///         Doesn't have '/' at the end
    std::string folderPath;
    std::string absoluteProjectPath();

    std::string recoverSlotPath(int slot);

    //Constructor
    Project(){}

    /// @brief Create a project from scratch
    /// @param destinationPath where will be the project folder be created
    /// @param name project name
    /// @param TDModelPath 3D Model file path. Copies that file to the Project/3DModels
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    /// @return true if success
    bool createProject(std::string destinationPath, std::string name, std::vector<std::string> TDModelPaths, int textureRes);
    
    bool discardUpdateProjectFlag = false;

    /// @brief update the existing project (in the destination of the public member variable folderPath) (write files in the library)
    void updateProject(bool updateTextures, bool multithreadingMode);
    
    /// @brief load an existing project using ligid file path
    /// @param ligidFilePath path to the ligid file
    /// @return 
    bool loadProject(std::string ligidFilePath, AppMaterialModifiers& appMaterialModifiers);

    bool loadLibraryElements(std::string folderPath, AppMaterialModifiers& appMaterialModifiers, std::string ligidFilePath);

    /// @brief Used to save as
    /// @param dstPath where to duplicate
    void duplicateFolder(std::string dstPath);
    
    /// @brief Copies the project path to the clipboard
    /// @param window 
    void copyTheProjectPathToTheClipboard();

    /// @brief Locates the ligid file in the folderPath 
    /// @param folderPath 
    /// @return path to the ligidpainter "" if not located
    bool locateLigidFileInFolder(const std::string& folderPath, std::string& ligidPath);
    
    /// @brief Retrieve data from the ligid file
    /// @param path 
    /// @param creationDate 
    /// @param lastOpenedDate 
    /// @param meshNodeScene 
    /// @param textureRes 
    /// @return True if success
    bool readLigidFile(std::string path,time_t &creationDate,time_t &lastOpenedDate);
    
    /// @brief Write ligid file to the project folder
    bool writeLigidFile(std::string path);

        /// @brief Returns the ligid file path of the project
    ///        (AAA/MyProject/MyProject.ligid)
    std::string ligidFilePath();

    /// @brief Returns the title of the project
    ///        (MyProject)
    std::string projectName();

    std::atomic<bool> projectProcessing = false;

private:
    bool wrtLigidFile(std::string path);

};

/*!
* @brief Kinda holds the same values as the brush class
* Holds the brush data of the app
* Same values as the brush settings GUI elements
* Special to the painter class
*/
struct BrushProperties{
    float radius;
    float hardness;
    float opacity;
    float spacing;
    float sizeJitter;
    float scatter;
    float fade;
    float rotation;
    float rotationJitter;
    float alphaJitter;
    bool individualTexture;
    bool sinWavePattern;
    Texture brushTexture;
};

struct VectorStroke{
    glm::vec2 startPos;
    glm::vec2 endPos;
    glm::vec2 offsetPos;

    bool endPointPressed = false;
    bool startPointPressed = false;
    bool offsetPointPressed = false;
    
    bool endPointClicked = false;
    bool startPointClicked = false;
    bool offsetPointClicked = false;
    
    float endPointHoverMixVal = 0.f;
    float endPointClickedMixVal = 0.f;
    float startPointHoverMixVal = 0.f;
    float startPointClickedMixVal = 0.f;
    
    bool endPointHover = false;
    bool startPointHover = false;
    bool offsetPointHover = false;
    
    VectorStroke(){}
    VectorStroke(glm::vec2 startPos, glm::vec2 endPos, glm::vec2 offsetPos){
        this->startPos = startPos;
        this->endPos = endPos;
        this->offsetPos = offsetPos;
    }
    
    void draw(Timer& timer, float edge, bool sceneState, std::vector<VectorStroke>& strokes, int curI);
};

struct MirrorSide{
    bool active = false;

    /// @brief 3D model linearized depth texture (3d model rendered with a depth shader) 
    Texture depthTexture;

    Texture mirroredPaintingTexture;

    Texture projectedPaintingTexture;

    glm::vec3 effectAxis = glm::vec3(0.);

    glm::mat4 getViewMat(glm::vec3 offset);
};

struct FaceSelection{
public:
    /// @brief Painting the selected mesh mode activated flag 
    bool activated = false;
    /// @brief Selecting mesh mode activated flag 
    bool editMode = false;
    /// @brief 0 : circle | 1 : box  
    int selectionModeIndex = 0;
    /// @brief radius value of the circle selection mode
    int radius = 10;

    bool hideUnselected = false;
    
    Framebuffer FBO;

    glm::vec2 boxSelectionStart = glm::vec2(0.f);
    glm::vec2 boxSelectionEnd = glm::vec2(0.f);
    bool boxSelectionInteraction(Timer &timer);

    /// @brief Contains the indices of the selected faces
    ///        Being processed in the PBR shader
    std::vector<byte> selectedPrimitiveIDs;
    std::vector<int> changedIndices;

    Texture selectedFaces;
    Texture meshMask;

    /// @brief 3D model rendered with primitive ID renderin shader
    Texture modelPrimitives;

    bool interaction(Mesh& selectedMesh, bool mouseInteraction, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 transformMatrix, glm::vec2 cursorPos, bool renderAllModel);

private:
    std::vector<byte> prevPrimArray;
    glm::vec2 lastMousePos;
};

class Painter
{
public:
    
    /// @brief 1st painting color value in the 3 others, set by the user using the GUI elements (color pickers)
    Color color1; 
    /// @brief 2nd painting color value in the 3 others, set by the user using the GUI elements (color pickers)
    Color color2; 
    /// @brief 3rd painting color value in the 3 others, set by the user using the GUI elements (color pickers)
    Color color3; 

    /// @brief which color value is selected (1 , 2 or 3)
    int selectedColorIndex = 0;

    /// @brief indicates which material channel will be painted. 0 : albedo, 1 : roughness, 2 : metallic, 3 : normal map, 4 : height map, 5 : ambient occlusion map
    int selectedPaintingChannelIndex = 0;
    
    MirrorSide oSide;
    MirrorSide oXSide;
    MirrorSide oYSide;
    MirrorSide oXYSide;
    MirrorSide oZSide;
    MirrorSide oXZSide;
    MirrorSide oYZSide;
    MirrorSide oXYZSide;

    float mirrorXOffset = 0.f;
    float mirrorYOffset = 0.f;
    float mirrorZOffset = 0.f;

    float smearTransformStrength = 1.f;
    float smearBlurStrength = 1.f;

    bool materialPainting = false;
    bool enableAlbedoChannel = false;
    bool enableRoughnessChannel = false;
    float roughnessVal = 0.f;
    bool enableMetallicChannel = false;
    float metallicVal = 0.f;
    bool enableNormalMapChannel = false;
    float normalMapStrengthVal = 0.f;
    bool enableHeightMapChannel = false;
    float heightMapVal = 0.f;
    bool enableAOChannel = false;
    float ambientOcclusionVal = 0.f;

    // Used to create the displaying texture for the brush section in the paintingPanel
    Brush displayingBrush;

    /// @brief Is paintingTexture16f if smearBrush used, paintingTexture8 if not
    unsigned int paintingTexture;  

    Texture projectedPaintingTexture;

    unsigned int paintingOverTexture;
    bool usePaintingOver = false;
    bool paintingoverTextureEditorMode = false;
    bool paintingOverGrayScale = false;
    bool paintingOverWraping = false;

    /// @brief Is set to true in the callbacks and is set to false after updating the depth texture
    ///  (Update the depth texture if set to true)
    bool updateTheDepthTexture = false;

    /// @brief The selected texture selected by user from the library.textures
    /// (This texture's id will be painted) 
    Texture selectedTexture;

    /// @brief Returns the index of the selected texture inside of the Library
    int getSelectedTextureIndexInLibrary();

    std::vector<VectorStroke> vectorStrokes;

    FaceSelection faceSelection;

    /*!
    * @brief Indicates which painting mode is selected
    * Set by the user using GUI elements (paintingModePanel)
    * 0 : Default painting    
    * 1 : Soften brush    
    * 2 : Smear brush 
    */
    int selectedPaintingModeIndex = 0;

    /*!
    *   @brief Indicates how to display the 3D model
    *   0 : Use nodes (no painting)
    *   1 : Use painting channels (painting mode)
    */
    int selectedDisplayingModeIndex = 0;
    
    /// @brief Indicates which mesh (material) (out of the model class) is selected.
    /// Set by the user using GUI elements (paintingPanel)
    int selectedMeshIndex = 0;
    
    /// @brief To avoid refreshing every frame in RendererRender.cpp
    /// Refreshing : Update the selected texture & clean the capturing framebuffer
    bool refreshable = true; 
    
    /// @brief Indicates if the 3D painting mode is enabled 
    /// Is changed in the windowpanel(the 2D & 3D painting buttons)
    bool threeDimensionalMode = true;
    
    /// @brief Brush property values 
    /// Set by the GUI elements (painting panel (brush sections) )
    /// Is the same as the GUI elements
    BrushProperties brushProperties;

    void setBrushProperties(Section brushSection);

    /// @brief Painter constructor (use initPainter to initialize the painter class)
    Painter();
    
    /*! 
    * @brief Init the painter class. Create textures, buffer objects & get necessary member variables
    */
    void initPainter();
    
    /*! 
    * @brief do painting (paint 2D). Called every frame if painting conditions are set. 
    *           painting conditions are : mouse left button pressed & cursor not hover any panel etc. 
    * @param windowOrtho orthographic projection matrix created with window size value.
    */
    void doPaint(glm::mat4 windowOrtho, std::vector<glm::vec2> strokeLocations, int paintingMode, Panel twoDPaintingPanel, Box twoDPaintingBox);
    
    /*!
    * @brief call that function in a single frame as the painting is completed (Mouse left button released)
    *        refreshPainting clears the @ref paintingFBO's color data (clears the paintingTexture)      
    */
    void refreshPainting();

    /*!
    * @brief updates the @ref selectedTexture right after painting is done.
    *        Use this right before the @ref refreshPainting  
    * @param textures textures in the library 
    * @param model the 3D model
    * @param scene structure contains matrices related to 3D model rendering & cam pos
    * @param twoDPaintingPanel if the painting mode is 2D use this panel's transform data 
    */
    void updateTexture(Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox);
    void updateTheTexture(Texture txtr, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox, glm::vec3 paintingColor, int channelI, float channelStr);

    
    /*!
    * @brief updates the @ref depthTexture right after painting is done.
    *        Use this right before the @ref refreshPainting  
    */
    void updateDepthTexture();

    void loadColor1();
    void loadColor2();
    void loadColor3();

    Color getSelectedColor();

    void applyVectorStrokes(std::vector<VectorStroke> vectorStrokes, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox);

    /// @brief Clears & refreshes all the buffers
    void refreshBuffers();

    /// @brief renderbuffer object used to depth test (used to create the depth texture)
    unsigned int depthRBO; 

    /*!
    * @brief framebuffer object used to capture paintingTexture
    *   Always uses the paintingTexture as it's texture buffer
    */
    unsigned int paintingFBO; 

    /// @brief RGBA8
    unsigned int paintingTexture8; 
    
    /// @brief RGBA16F (Used for smear brush)
    unsigned int paintingTexture16f;

    Texture paintingBGTexture; 
    
    glm::vec2 startCursorPos = glm::vec2(0); 
    glm::vec2 lastCursorPos = glm::vec2(0); 
    int frameCounter = 0;
    
    std::vector<glm::vec2> getCursorSubstitution(float spacing);
    void changeColor(Color &color);

private:
    void Painter::projectThePaintingTexture(Texture& selectedTexture,  Texture& projectedPaintingTexture,  unsigned int paintingTexture,  unsigned int depthTexture, 
                                            int selectedPaintingModeIndex, float brushPropertiesOpacity, bool threeDimensionalMode,glm::mat4 windowOrtho,int selectedMeshIndex,
                                            Box twoDPaintingBox,glm::mat4 viewMat,bool faceSelectionActive,Texture selectedPrimitives);

    void generateMirroredProjectedPaintingTexture(   
                                                        MirrorSide& oSide, MirrorSide& oXSide, MirrorSide& oYSide, MirrorSide& oXYSide, MirrorSide& oZSide, 
                                                        MirrorSide& oXZSide, MirrorSide& oYZSide, MirrorSide& oXYZSide, float mirrorXOffset, float mirrorYOffset, 
                                                        float mirrorZOffset,Texture paintingTxtrObj, Texture& selectedTexture,  Texture& projectedPaintingTexture,  
                                                        int selectedPaintingModeIndex, float brushPropertiesOpacity,  bool threeDimensionalMode,  glm::mat4 windowOrtho,  
                                                        int selectedMeshIndex, Box twoDPaintingBox, bool faceSelectionActive, Texture selectedPrimitives
                                                    );
};

/// @brief A single char
struct character { 
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

/// @brief Manages font related stuff & contains all the characters of a font
class Font
{
public:
    /// @brief Name of the font (title)
    std::string name = "";

	/// @brief Characters of the font 
	std::map<char, character> characters;

    /// @brief Default constructor (does nothing)
    Font(/* args */);

    /// @brief Loads the font into the characters map in the given @param path
    void loadFont(const char* path);
};





/// @brief handles 2D square vertex objects.
/*
    int main(){
        Box box;
        box.init();

        while(true){
            box.bindBuffers();

            Shader related stuff
            ...

            glDrawArrays(GL_TRIANGLES, 0 , 6); 
            
            Shader related stuff
            ...
            
            glDrawArrays(GL_TRIANGLES, 0 , 6); 

            box.unbindBuffers();
        }
    }

*/
class Box
{
public:
    unsigned int VBO = 0;
    unsigned int VAO = 0;

    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    Box();

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();

    void customMeshInit(glm::vec3 pos, glm::vec2 scale);
    
    void customMeshUpdate(glm::vec3 pos, glm::vec2 scale);

    /// @brief Binds the 2D square vertex objects
    void bindBuffers();
    
    /// @brief Unbinds the 2D square vertex objects
    void unbindBuffers();
    
    /// @brief Draw the square (is not used) 
    void draw(glm::vec3 pos, glm::vec2 scale);
};


/// @brief handles websites
class Website
{
private:

    /// @brief a website url (loaded with the constructor)
    std::string url;

public:
    /// @brief default constructor
    Website();

    /// @brief constructor with url value
    Website(std::string url);
    
    /// @brief open the website in the primary search engine
    void open();
    
    /// @brief @return the url 
    std::string getURL();
};

namespace ZIPPER{
    std::vector<char> decompressZlibChar(const std::vector<char>& compressedData, size_t uncompressedSize);
    std::vector<float> decompressZlibFloat(const std::vector<char>& compressedData, size_t numFloats);
    std::vector<double> decompressZlibDouble(const std::vector<char>& compressedData, size_t numDoubles);
    std::vector<long long> decompressZlibLongLong(const std::vector<char>& compressedData, size_t numLongLongs);
    std::vector<int> decompressZlibInt(const std::vector<char>& compressedData, size_t numInts);
};

struct Websites{
    Website ligidTools;
    Website youTube;
};

/// @brief Fonts will be used in the app
struct Fonts{ 
    Font Arial;
};

namespace FileHandler{
    bool writeOBJFile(std::string path, Model model);
    Model readOBJFile(std::string path);
    
    Model readFBXFile(std::string path);
    
    bool writeLGDMATERIALFile(std::string path, Material material);
    bool readLGDMATERIALFile(std::string path, Material& material, AppMaterialModifiers appMaterialModifiers);
    
    bool writeLGDBRUSHFile(std::string path, Brush brush);
    bool readLGDBRUSHFile(std::string path, Brush& brush);
}

class SourceLibTexture{
public:
    /// @brief Low resolution version of the texture to not occupy much space in the RAM 
    Texture displayingTexture;

    Texture fullTexture;

    /// @brief Unique value smt like 52_A to access later
    ///        52 : Texture ID
    ///        A : Albedo version
    ///        H : Height Map version
    std::string ID;
    
    /// @brief Loads and returns the texture corresponding with the @ref ID
    Texture getTexture();
    
    /// @brief Load the textures inside of the "Resources/Texture Library"
    void load(const std::string path);
};

namespace Debugger{
    void block(std::string name);
};

namespace Shortcuts{
    bool CTRL_Z();
    bool CTRL_X();
    bool CTRL_S();
    bool CTRL_N();
    bool CTRL_O();
    bool CTRL_P();
    bool CTRL_Y();
    bool CTRL_C();
    bool CTRL_V();
    bool CTRL_A();
    bool CTRL_F();
    bool CTRL_G();
    bool CTRL_W();
    bool CTRL_SHIFT_S();
    bool F11();
    bool anyShortCut();
}

#include <thread>
#include <mutex>

struct ThreadElements{
    std::atomic<bool> updateTextures = false;
    
    std::atomic<bool> isRunning = true;
    std::mutex mutex;
    std::condition_variable exportCV;
};

extern ThreadElements projectUpdatingThreadElements; 
void projectUpdatingThread(Project &project);

#endif