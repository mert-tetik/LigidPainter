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
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map

//forward declerations :

/// @brief forward declared Library AppTextures structure
struct AppTextures;
/// @brief forward declared ColorPalette class
class ColorPalette;
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
    void deleteFilesInFolder(const std::string folderPath);

    /// @brief duplicates the folder in the @param src to the @param dest path
    void duplicateFolder(const std::string src, const std::string dest);

    /// @brief generates unique value into the param s according to the sArray
    /// @return if change is made 
    bool uniqueName(std::string &s, std::vector<std::string> sArray);
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
    double FPS = 1;
    double renderingSpeed = 0;
    
    Timer(/* args */);
    
    bool runTimer(float cycleTime);
    void transition(bool state ,float &value,float timeInSeconds);
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

    /// @brief Indicates which procedural texture shader function will be used (indexing the getProcedural function in the Shaders/Include/Procedural.frag)
    ///         -1 : No procedural shader will be used. Use the @ref ID instead.
    int proceduralID = -1;
    float proceduralScale = 1.f;
    int proceduralnverted = 0;
    bool proceduralNormalMap = false;
    bool proceduralNormalGrayScale = false;
    float proceduralNormalStrength = 10.f;
    unsigned int proceduralTextureID = 0;

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

    /// @brief Load a texture by importing the texture in the given path via STBI
    void load(const char* path);
    
    /// @brief Returns texture data in the given path & doesn't write anything to the member variables
    unsigned char* getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip);
    
    /// @brief Exports the texture to the given directory path
    /// @param path folder path (will be completed as : @param path + / + @ref title + . + format (in lowercase))
    /// @param format is PNG JPEG BMP TGA
    void exportTexture(std::string path,const std::string format);
    
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
    
    /// @brief Write the tmp file of the texture from the tmp folder
    void writeTMP();
    
    /// @brief Read the tmp file of the texture from the tmp folder and delete the readen file
    void readTMP();

    std::vector<glm::vec3> getMaterialIDPalette();

    void removeSeams(Mesh& mesh, int textureResolution);
    void removeSeams(Mesh& mesh, glm::ivec2 textureResolution);
    unsigned int generateProceduralTexture(Mesh &mesh, Scene scene, int textureRes);
    void generateNormalMap(unsigned int& normalMap, int textureResolution, float proceduralNormalStrength, bool proceduralNormalGrayScale);

};

class Project
{
public:
    /// @brief Folder path of the project 
    ///        (AAA/MyProject)
    ///         Doesn't have '/' at the end
    std::string folderPath;

    //Constructor
    Project(){}

    /// @brief Create a project from scratch
    /// @param destinationPath where will be the project folder be created
    /// @param name project name
    /// @param TDModelPath 3D Model file path. Copies that file to the Project/3DModels
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    /// @return true if success
    bool createProject(std::string destinationPath,std::string name,std::string TDModelPath,int textureRes);
    
    /// @brief update the existing project (in the destination of the public member variable folderPath) (write files in the library)
    /// @param  structure (holds the textures / materials / brushes & TDModels)
    /// @param meshNodeScene The main meshNodeScene which has the mesh node
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    void updateProject(  int& textureRes);
    
    /// @brief load an existing project using ligid file path
    /// @param ligidFilePath path to the ligid file
    /// @param  structure (holds the textures / materials / brushes & TDModels)
    /// @param model The 3D models
    /// @param appTextures appTextures structure (holds all the textures used by the GUI)
    /// @param colorPalette colorPalette class (color theme of the ligidpainter)
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    /// @param meshNodeScene The main meshNodeScene which has the mesh node
    /// @return 
    bool loadProject(std::string ligidFilePath,Model &model,AppTextures appTextures,
                    ColorPalette colorPalette,int &textureRes, glm::vec2 videoScale,
                    AppMaterialModifiers& appMaterialModifiers);

    /// @brief Used to save as
    /// @param dstPath where to duplicate
    void duplicateFolder(std::string dstPath);
    
    /// @brief Copies the project path to the clipboard
    /// @param window 
    void copyTheProjectPathToTheClipboard(LigidWindow window);

    /// @brief Locates the ligid file in the folderPath 
    /// @param folderPath 
    /// @return path to the ligidpainter "" if not located
    std::string locateLigidFileInFolder(const std::string& folderPath);
    
    /// @brief Retrieve data from the ligid file
    /// @param path 
    /// @param creationDate 
    /// @param lastOpenedDate 
    /// @param meshNodeScene 
    /// @param textureRes 
    /// @return True if success
    bool readLigidFile(std::string path,time_t &creationDate,time_t &lastOpenedDate,  int& textureRes,  ColorPalette colorPalette, AppTextures appTextures, glm::vec2 videoScale);
    
    /// @brief Write ligid file to the project folder
    /// @param meshNodeScene 
    /// @param textureRes 
    void writeLigidFile(int textureRes);

        /// @brief Returns the ligid file path of the project
    ///        (AAA/MyProject/MyProject.ligid)
    std::string ligidFilePath();

    /// @brief Returns the title of the project
    ///        (MyProject)
    std::string projectName();
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
    
    /// @brief Is paintingTexture16f if smearBrush used, paintingTexture8 if not
    unsigned int paintingTexture;  

    /// @brief 3D model linearized depth texture (3d model rendered with a depth shader) 
    unsigned int depthTexture;

    /// @brief Is set to true in the callbacks and is set to false after updating the depth texture
    ///  (Update the depth texture if set to true)
    bool updateTheDepthTexture = false;

    /// @brief The selected texture selected by user from the library.textures
    /// (This texture's id will be painted) 
    Texture selectedTexture;

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

    /// @brief Painter constructor (use initPainter to initialize the painter class)
    Painter();
    
    /*! 
    * @brief Init the painter class. Create textures, buffer objects & get necessary member variables
    * @param videoScale resolution value of the primary monitor
    * @param paintingShader the painting shader
    * @param buttonShader the button shader
    * @param tdModelShader the 3D model shader
    * @param depth3DShader depth shader
    * @param textureUpdatingShader texture updating shader
    */
    void initPainter(glm::vec2 videoScale);
    
    /*! 
    * @brief do painting (paint 2D). Called every frame if painting conditions are set. 
    *           painting conditions are : mouse left button pressed & cursor not hover any panel etc. 
    * @param windowOrtho orthographics projection matrix created with window size value.
    * @param textures textures in the library
    */
    void doPaint(glm::mat4 windowOrtho, Context context);
    
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
    void updateTexture(Model &model, Scene scene, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos);
    

    /*!
    * @brief updates the @ref depthTexture right after painting is done.
    *        Use this right before the @ref refreshPainting  
    * @param model the 3D model
    * @param windowScale size of the window
    */
    void updateDepthTexture( Model &model,glm::vec2 windowScale);

    void loadColor1();
    void loadColor2();
    void loadColor3();

    Color getSelectedColor();

private:
    
    /// @brief primary monitor resolution data
    glm::vec2 videoScale;

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

    glm::vec2 startCursorPos = glm::vec2(0); 
    glm::vec2 lastCursorPos = glm::vec2(0); 
    int frameCounter = 0;
    
    std::vector<glm::vec2> getCursorSubstitution(float spacing);
    void changeColor(Color &color);
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



class Brush
{
private:
    /// @brief Initializes the displaying texture
    void initDisplayingTexture();

public:
    
    /*  --------- Brush Properties --------- */
        float sizeJitter;
        float scatter;
        float fade;
        float rotation;
        float rotationJitter;
        float alphaJitter;
        bool individualTexture;
        bool sinWavePattern;
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
    unsigned int displayingTexture;
    

    /// @brief Default constructor (just initializes the @ref displayingTexture)
    Brush();
    
    /// @brief Initializes the @ref displayingTexture and assings the given params
    Brush(float sizeJitter, float scatter, float fade, float rotation, float rotationJitter, 
          float alphaJitter, bool individualTexture, bool sinWavePattern, 
          std::string title, Texture texture);

    /// @brief Updates the displaying texture using brush properties.
    void updateDisplayTexture();

    /// @brief move brush properties to the painting panel
    void useBrush(Panel &paintingPanel);

    /// @brief move painting panel brush properties to the class's brush properties
    void applyToBrush(Panel &paintingPanel);
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
    unsigned int VBO;
    unsigned int VAO;

    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    Box();

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();

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


// RENDERER UTIL STRUCTURES
struct Camera{
    float yaw = -90.f;
    float pitch = 0.f;
    glm::vec3 cameraPos = glm::vec3(0,0,-10.f);
    glm::vec3 originPos = glm::vec3(0,0,0);
    float radius = 10.f; 
};

struct Scene{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    const float fov = 45.f;
    const float aNear = 0.1f;
    const float aFar = 1000.0f;

    glm::vec2 videoScale;

    Camera camera;
};

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
};

struct Websites{
    Website ligidTools;
    Website youTube;
};

/// @brief Static textures will be used in the app
struct AppTextures{ 
    //--Icons
    Texture TDModelIcon; 
    Texture softenIcon;  
    Texture smearIcon;
    Texture modifierIcon;
    Texture environmentIcon;
    Texture dropperIcon;
    Texture colorIcon;
    Texture brushIcon;
    Texture arrowR;
    Texture arrowL;
    Texture threeDIcon;
    Texture twoDIcon;
    Texture exportIcon;
    Texture areaPaintingIcon;
    Texture gearIcon;
    Texture ligidPainterIcon;
    Texture nodeDisplayingMode;
    Texture paintingDisplayingMode;
    Texture solidPaintingDisplayingMode;
    
    //--Textures
    Texture greetingDialogImage;  
    Texture noMaterialConnectedToTheMeshWarningImage;  
};

/// @brief Fonts will be used in the app
struct Fonts{ 
    Font Arial;
};

namespace FileHandler{
    bool writeOBJFile(std::string path, Model model);
    Model readOBJFile(std::string path);
    
    Model readFBXFile(std::string path);
    
    bool readLGDMATERIALFile(std::string path, Material& material, ColorPalette colorPalette,  AppTextures appTextures, 
                             AppMaterialModifiers appMaterialModifiers);
    bool writeLGDMATERIALFile(std::string path, Material &material);
    
    bool writeLGDBRUSHFile(std::string path, Brush brush);
    bool readLGDBRUSHFile(std::string path, Brush& brush);
}


#endif