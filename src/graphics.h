#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <unordered_map>

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string shaders = "shaders/";

const bool enableValidationLayers = true;

struct V3 {
	float x, y, z;
	V3() : x(0.0f), y(0.0f), z(0.0f) {}
	V3(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Mesh {
	
public:
	void* handle;
	void create(std::string filename);
	~Mesh();
};

class Texture {
public:
	void* handle;
	void create(std::string filename);
	~Texture();
};

struct FontGlyph {
	int xIndex;
	int yIndex;
	int gWidth;
	int gHeight;
};

class Font {
public:
	Texture tex;
	int fontSize;
	int wGlyphs;
	std::unordered_map<int, FontGlyph> glyphs;

	~Font();
	void createFont(std::string filename, std::vector<int> extraChars);
};

class Render {
	bool isCreated = false;
public:
	bool is3d;
	V3 pos, rot, scale;
	void* handle;
	~Render();
	void create(Mesh& mesh, Texture& tex);
	void create(Texture& tex);
	void create(Font& font, std::basic_string<wchar_t>& unicodeText);
};

extern std::vector<Render*> objects;

void createWindow();
void initVulkan();
bool renderFrame();
void cleanVulkan();
double currTime();

extern V3 cameraPos;
extern V3 cameraLook;


