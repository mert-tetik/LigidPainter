/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef UTILS_HPP
#define UTILS_HPP

//OpenGL & GLFW
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map

//forward declerations :

/// @brief forward declared Library class
struct Library;
/// @brief forward declared Library Shaders structure
struct Shaders;
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
/// @brief forward declared Mouse class
class Mouse;
/// @brief forward declared Node class
class Node;
/// @brief forward declared Scene struct
struct Scene;

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
    
    /// @brief uses void giveUniqueId(int &ID ,const std::vector<int> otherIDs); (implementation for the textures)
    /// @param ID 
    /// @param textures 
    void giveUniqueId(int &ID ,const std::vector<Texture> textures);

    /// @brief uses void giveUniqueId(int &ID ,const std::vector<int> otherIDs); (implementation for the materials)
    /// @param ID 
    /// @param materials 
    void giveUniqueId(int &ID ,const std::vector<Material> materials);
    
    /// @brief Converts std::vector<std::string> to char* const* (char**)
    /// @param strings 
    /// @return 
    char* const* convertStringArray(const std::vector<std::string> strings);


    //! NODES


    /// @brief 
    /// @param nodeScene the node scene 
    /// @param library 
    /// @param textureRes 1024 , 2048 , 512 etc. 
    ///         (the selected texture resolution) 
    ///         (used to create the textures of the material) 
    ///         (set to 0 for no texture creation (if will be called every frame))
    /// @return a vector of materials connected to the mesh node (has the size of a model.meshes)  
    std::vector<Material> getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &nodeScene,Library &library,int textureRes);
    
    /// @brief remove the connections with invalid indices 
    ///        (if the node is connected to a nonexistent node or an input)
    ///        (for example : connection node index = 5 & the nodeScene size : 4)
    /// @param nodeScene 
    void updateAllTheNodeConnections(std::vector<Node> &nodeScene);
    
    /// @brief deletes the node with the given index from nodeScene and updates all the nodes
    /// @param nodeScene 
    /// @param index remove the nodeScene[index]
    void deleteNode(std::vector<Node>& nodeScene, int index);

    /// @brief the folder distinguisher 
    /// @return '/' if unix based OS defined '\\' if the windows defined
    char folderDistinguisher();

    /// @brief deletes all the file system objects in the given folder path
    /// @param folderPath 
    void deleteFilesInFolder(const std::string folderPath);
}




class Color
{
public:
    glm::vec3 RGB; //Red, Green, Blue 
    glm::vec3 HSV; //Hue, Saturation, Value
    std::string hex; //Hex value which starts with char '#'

    //Constructors
    Color();
    Color(glm::vec3 rgbColor);
    Color(std::string hexColor);

    //Public member functions

    void loadRGB(glm::vec3 value);
    void loadHSV(glm::vec3 value);
    void loadHex(std::string value);

    glm::vec3 rgbToHsv(const glm::vec3& rgb);
    glm::vec3 hsvToRgb(const glm::vec3& hsv);
    glm::vec3 hexToRgb(const std::string& hex);
    std::string rgbToHex(const glm::vec3& rgb);
    glm::vec3 hexToHsv(const std::string& hex);
    std::string hsvToHex(const glm::vec3& hsv);
};

class Timer
{
private:
public:
    double lastTimeT = glfwGetTime();
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
    std::string generateTMPTitle();
    std::string getTMPTitleWithGreatestIndex();

public:
    unsigned int ID = 0; 
    std::string path = "";
    std::string title = "";

    int uniqueId;

    Texture();
    Texture(unsigned int ID);
    Texture(char* pixels, int w, int h);

    void load(const char* path);
    unsigned char* getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip);
    void exportTexture(std::string path,const std::string format);
    void getData(char*& pixels);
    glm::ivec2 getResolution();
    unsigned int duplicateTexture();
    void writeTMP();
    void readTMP();
};


/// @brief Manages shader programs
class Shader
{
public:
    //OpenGL shader pipeline : .vert -> .tc -> .te -> .geom -> .frag

    /// @brief Id of the shader in an OpenGL context
    unsigned int ID;

    /// @brief Default constructor (is not used) 
    Shader();

    /// @brief Create the shader program compiling the shader code in the given paths (give nullptr if the shader is not used)
    /// @param vertexPath Vertex shader file (.vert file)
    /// @param fragmentPath Fragment shader file (.frag file)
    /// @param geometryPath Geometry shader file (.geom file)
    /// @param tessControlPath Tessellation control shader file (.tc file)
    /// @param tessEvalPath Tessellation evaluation shader file (.te file)
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* tessControlPath, const char* tessEvalPath);

    /// @brief replace the #pragma LIGID_INCLUDE with the code in the given path (LIGID_INCLUDE(givenPath))
    /// @param code the code
    void processShaderCode(std::string &code);

    /// @brief use the shader program 
    void use();
    
    // Utility uniform functions
    
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, const glm::vec2 &value);
    void setVec2(const std::string &name, float x, float y);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec4(const std::string &name, const glm::vec4 &value);
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat);
    void setMat3(const std::string &name, const glm::mat3 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mat);
    

private:
    /// @brief Checks if an error is occured (print the error msg to the terminal)
    /// @param shader shader id
    /// @param type indicate if compiling a .frag file, .tc file or .vert file etc. (to print to the terminal) 
    void checkCompileErrors(GLuint shader, std::string type);
};



class Project
{
public:
    /// @brief Folder path of the project 
    ///        (AAA/MyProject)
    std::string folderPath;

    /// @brief Ligid file path of the project
    ///        (AAA/MyProject/MyProject.ligid)
    std::string ligidFilePath;

    /// @brief Title of the project
    ///        (MyProject)
    std::string projectName;

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
    /// @param library library structure (holds the textures / materials / brushes & TDModels)
    /// @param nodeScene The main nodeScene which has the mesh node
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    void updateProject(Library &library, std::vector<Node> &nodeScene, int& textureRes);
    
    /// @brief load an existing project using ligid file path
    /// @param ligidFilePath path to the ligid file
    /// @param library library structure (holds the textures / materials / brushes & TDModels)
    /// @param shaders shaders structure (holds all the shader programs)
    /// @param model The 3D models
    /// @param appTextures appTextures structure (holds all the textures used by the GUI)
    /// @param colorPalette colorPalette class (color theme of the ligidpainter)
    /// @param textureRes 512 , 1024 , 2048 etc. (selected by the user & written to the .ligid file)
    /// @param nodeScene The main nodeScene which has the mesh node
    /// @param appNodes 
    /// @return 
    bool loadProject(std::string ligidFilePath,Library &library,Shaders shaders,Model &model,AppTextures appTextures,
                    ColorPalette colorPalette,int &textureRes,std::vector<Node> &nodeScene , std::vector<Node> &appNodes);

    /// @brief Used to save as
    /// @param dstPath where to duplicate
    void duplicateFolder(std::string dstPath);
    
    /// @brief Copies the project path to the clipboard
    /// @param window 
    void copyTheProjectPathToTheClipboard(GLFWwindow* window);

    /// @brief Locates the ligid file in the folderPath 
    /// @param folderPath 
    /// @return path to the ligidpainter "" if not located
    std::string locateLigidFileInFolder(const std::string& folderPath);
    
    /// @brief Retrieve data from the ligid file
    /// @param path 
    /// @param creationDate 
    /// @param lastOpenedDate 
    /// @param nodeScene 
    /// @param appNodes 
    /// @param textureRes 
    /// @return True if success
    bool readLigidFile(std::string path,time_t &creationDate,time_t &lastOpenedDate,std::vector<Node> &nodeScene, std::vector<Node> &appNodes, int& textureRes);
    
    /// @brief Write ligid file to the project folder
    /// @param nodeScene 
    /// @param textureRes 
    void writeLigidFile(const std::vector<Node> nodeScene, int textureRes);
};

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
    Color color1; 
    Color color2; 
    Color color3; 
    int selectedColorIndex = 0;
    unsigned int RBO;
    unsigned int FBO;
    unsigned int paintingTexture; //Is paintingTexture8 or paintingTexture16f 
    unsigned int paintingTexture8;
    unsigned int paintingTexture16f;//Used for smear brush
    unsigned int depthTexture;
    bool updateTheDepthTexture = false;
    Texture selectedTexture;
    int selectedPaintingModeIndex = 0; 
    int selectedMeshIndex = 0;
    glm::vec2 videoScale;
    Shader paintingShader;
    Shader buttonShader;
    Shader tdModelShader;
    Shader depth3DShader;
    Shader textureUpdatingShader;
    bool refreshable = true; //To avoid refreshing every frame in UI.hpp
    glm::mat4 projection;
    bool threeDimensionalMode = true;
    glm::mat4 windowProjection;
    glm::vec3 pos2D;
    glm::vec2 scale2D;

    BrushProperties brushProperties;

    Painter();

    void initPainter(glm::vec2 videoScale, Shader paintingShader, Shader buttonShader, Shader tdModelShader, Shader depth3DShader, Shader textureUpdatingShader);
    void doPaint(Mouse mouse,std::vector<Texture> textures);
    void refreshPainting();
    void updateTexture(std::vector<Texture> &textures, Model &model,int textureRes, Scene scene);
    void updateDepthTexture( Model &model,glm::vec2 windowScale);

    void loadColor1();
    void loadColor2();
    void loadColor3();
private:
    glm::vec2 startCursorPos = glm::vec2(0); 
    glm::vec2 lastCursorPos = glm::vec2(0); 
    int frameCounter = 0;
    
    std::vector<glm::vec2> getCursorSubstitution(Mouse &mouse,float spacing);
    void changeColor(Color &color);
    Color getSelectedColor();
};



class Mouse
{
public:
    glm::vec2 cursorPos = glm::vec2(0);
    glm::vec2 mouseOffset;
	int mods = 0;
	bool LClick = false; //Left click - button 0
	bool RClick = false; //Right click - button 1
	bool MClick = false; //Middle click - button 2
	bool LDoubleClick = false; //Left click - button 0
	bool LPressed = false;
	bool RPressed = false;
	bool MPressed = false;
	double mouseScroll = 0.;
	GLFWcursor* defaultCursor;
	GLFWcursor* pointerCursor;
	GLFWcursor* hSlideCursor;
	GLFWcursor* vSlideCursor;
	GLFWcursor* activeCursor; //updateCursor sets that cursor as the cursor
    GLFWwindow* window;

    Mouse();
    Mouse(GLFWwindow* window);

	void loadCursors();
	void setCursor(GLFWcursor* cursor);
	void updateCursor();
    bool isMouseHover(glm::vec2 scale, glm::vec2 position);
};


struct character { //A single char
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};


class Font
{
public:
    std::string name = "";
    std::string path = "";
	std::map<char, character> characters;

    Font(/* args */);
    void loadFont(const char* path);
};



class Brush
{
public:
    float sizeJitter;
    float scatter;
    float fade;
    float rotation;
    float rotationJitter;
    float alphaJitter;
    bool individualTexture;
    bool sinWavePattern;
    std::string title;
    unsigned int displayingTexture;
    Texture texture; //Brush texture

    //Constructors
    Brush();
    Brush(float sizeJitter,float scatter,float fade,float rotation,float rotationJitter,float alphaJitter,bool individualTexture,bool sinWavePattern,
          std::string title,Texture texture);

    void initDisplayingTexture();
    void saveFile(std::string path);
    void readFile(std::string path);
    void updateDisplayTexture(Shader paintingShader, Shader buttonShader);
};



class Box
{
public:
    unsigned int VBO;
    unsigned int VAO;

    Box();

    void init();
    void bindBuffers();
    void unbindBuffers();
    void draw(Shader shader,glm::vec3 pos,glm::vec2 scale);
};


class Website
{
private:
    std::string url;
public:
    //Constructor
    Website();
    Website(std::string url);
    
    //Public member functions
    void open();
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
    const float near = 0.1f;
    const float far = 1000.0f;

    glm::vec2 videoScale;

    Camera camera;
};

/// @brief Don't directly manipulate vectors! Instead use public member functions. Same goes for the selectedElementIndex.
struct Library{
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Brush> brushes;
    std::vector<Model> TDModels;

    int selectedElementIndex = 0;

    bool changed = true;

    void uniqueName(std::string &s,std::vector<std::string> sArray);
    void uniqueNameControl();

    //Add elements
    void addTexture     (Texture& texture);
    void addMaterial    (Material& material);
    void addBrush       (Brush& brush);
    void addModel       (Model& model);

    //Erase elements
    void eraseTexture   (int index);
    void eraseMaterial  (int index);
    void eraseBrush     (int index);
    void eraseModel     (int index);
    
    //Clear vectors
    void clearTextures   ();
    void clearMaterials  ();
    void clearBrushes    ();
    void clearModels     ();

    void changeSelectedElementIndex(int newI);

};

struct Context{
    GLFWwindow* window;
    glm::ivec2 windowScale;
};
struct Shaders{
    Shader tdModelShader;
    Shader skyboxShader;
    Shader skyboxBall;
    Shader buttonShader;
    Shader prefilteringShader;
    Shader singleCurve;
    Shader colorPicker;
    Shader twoDPainting;
    Shader depth3D;
    Shader textureUpdatingShader;
};

struct Websites{
    Website ligidTools;
    Website youTube;
};

struct AppTextures{ //Textures those will be used in UI of the app
    //--Icons
    Texture TDModelIcon; 
    Texture softenIcon;  
    Texture smearIcon;
    Texture modifierIcon;
    Texture environmentIcon;
    Texture dropperIcon;
    Texture colorIcon;
    Texture brushIcon;
    Texture threeDIcon;
    Texture twoDIcon;
    Texture exportIcon;
    Texture areaPaintingIcon;
    Texture gearIcon;
    Texture ligidPainterIcon;
    
    //--Textures
    Texture greetingDialogImage;  
};
struct Fonts{ //Fonts those will be used in the app
    Font Arial;
};


#endif