#ifndef LGDLOAD_H
#define LGDLOAD_H

struct character {
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

struct InitializedTextures;

class Load{
public:
    InitializedTextures initTextures(const char* maskTexturePath);
	unsigned int loadCubemap(std::vector<std::string> faces,unsigned int textureSlot);
	LigidCursors loadCursors();
	Icons loadIcons();
	Cubemaps loadCubemaps();
	BrushMaskTextures loadBrushMaskTextures();
	void uploadChars();
	void sendCharsToUI(std::map<char, character> characters);
};
#endif