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

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <map>

struct Library;
struct Shaders;
struct AppTextures;
class ColorPalette;
class Model;
class Texture;
class Material;
class Brush;
class Mouse;
class Node;


class Util
{
public:
    Util(/* args */);

    float getPercent(float value, float percent);
    glm::vec2 getPercent(glm::vec2 value, glm::vec2 percent);
    glm::vec3 getPercent(glm::vec3 value, glm::vec3 percent);

    std::string getLastWordBySeparatingWithChar(std::string s, char del);
    std::string removeLastWordBySeparatingWithChar(std::string s, char del);
    std::string removeExtension(std::string s);
    std::string rmvPath(std::string startingPath, std::string fullPath);
    void openInFileExplorer(const char* path);
    void giveUniqueId(int &ID ,const std::vector<int> otherIDs);
    void giveUniqueId(int &ID ,const std::vector<Texture> textures);
    void giveUniqueId(int &ID ,const std::vector<Material> materials);

    std::vector<Material> getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &nodeScene,Library &library,int textureRes);

};

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
    #if defined(_WIN32) || defined(_WIN64)
		char folderDistinguisher = '\\';
	#else
		char folderDistinguisher = '/'; 
	#endif
public:
    unsigned int ID = 0; 
    std::string path = "";
    std::string title = "";
    std::string tmpPath;
    bool isTexture = true; //Otherwise is a folder

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
};



class Shader
{
public:
    unsigned int ID;

    //Constructor
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

    void use();
    
    // tility uniform functions
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
    void checkCompileErrors(GLuint shader, std::string type);
};



class Project
{
private:

    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif
    
    void deleteFilesInFolder(const std::string folderPath);

public:
    std::string folderPath;
    std::string ligidFilePath;
    std::string projectName;

    //Constructor
    Project(){}

    //Public member functions
    bool createProject(std::string destinationPath,std::string name,std::string TDModelPath,int textureRes);
    void updateProject(Library &library, std::vector<Node> &nodeScene, int& textureRes);
    bool loadProject(std::string ligidFilePath,Library &library,Shaders shaders,Model &model,AppTextures appTextures,
                    ColorPalette colorPalette,int &textureRes,std::vector<Node> &nodeScene , std::vector<Node> &appNodes);
    void duplicateFolder(std::string dstPath);
    void copyTheProjectPathToTheClipboard(GLFWwindow* window);

    std::string locateLigidFileInFolder(const std::string& folderPath);
    
    bool readLigidFile(std::string path,time_t &creationDate,time_t &lastOpenedDate,std::vector<Node> &nodeScene, std::vector<Node> &appNodes, int& textureRes);
    void writeLigidFile(const std::vector<Node> nodeScene, int textureRes);
};


class Painter
{
public:
    Color color1; 
    Color color2; 
    Color color3; 
    int selectedColorIndex = 0;
    Texture brushTexture;
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
    bool refreshable = true; //To avoid refreshing every frame in UI.hpp
    glm::mat4 projection;
    bool threeDimensionalMode = true;
    glm::mat4 windowProjection;
    glm::vec3 pos2D;
    glm::vec2 scale2D;

    Painter();

    void initPainter(glm::vec2 videoScale, Shader paintingShader, Shader buttonShader, Shader tdModelShader,Shader depth3DShader);
    void doPaint(Mouse mouse,float radius,float hardness,float opacity,float spacing,float sizeJitter,float scatter,float fade,float rotation,
                 float rotationJitter,float alphaJitter,bool individualTexture,bool sinWavePattern,std::vector<Texture> textures);
    void refreshPainting();
    void updateTexture(std::vector<Texture> &textures, Model &model,int textureRes);
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
private:
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif
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
struct Library{
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Brush> brushes;
    std::vector<Model> TDModels;

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