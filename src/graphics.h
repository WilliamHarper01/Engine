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

#include "goodstr.h"

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 2

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

struct V3 {
	float x, y, z;
	V3() : x(0.0f), y(0.0f), z(0.0f) {}
	V3(float x, float y, float z) : x(x), y(y), z(z) {}
	void print();
};

struct Color{
	float r, g, b, a;
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	short to5Bit();
	void from5Bit(short color);
	void from5BitHSV(short hsv);
	short to5BitHSV(); //in format 0b0vvvvvssssshhhhh
	void print();
};

#define BLACK {0.0f, 0.0f, 0.0f, 1.0f}
#define WHITE {1.0f, 1.0f, 1.0f, 1.0f}

extern Color clearColor;

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
	int fontSize = 12;
	float spaceLength = 1.0f;
	float charSpace = 0.2f;
	float lineSpace = 1.0f;
	bool centerText = true;
	int wGlyphs;
	std::unordered_map<int, FontGlyph> glyphs;

	~Font();
	void createFont(std::string filename, std::vector<int> extraChars);
	static std::vector<int> getAscii();
};

class Render {
	bool isCreated = false;
public:
	bool is3d;
	V3 pos, rot, scale;
	Color color;
	void* handle;
	void (*onClick)(Render *, int) = nullptr;
	void (*onClickDown)(Render *, int) = nullptr;
	~Render();
	void create(Mesh& mesh, Texture& tex);
	void create(Texture& tex);
	void create(Font& font, Goodstr & unicodeText);
	void updateText(Font& font, Goodstr & text);
};

extern std::vector<Render*> objects;

void createWindow();
void initGraphics();
bool startLoop(void (*updateCallback)());
void cleanGraphics();
double currTime();
void getCursorPos(double * x, double * y);
bool getKeyDown(char key);

extern V3 cameraPos;
extern V3 cameraLook;


