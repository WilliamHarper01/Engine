#if __clang__
	#include "../src/graphics.h"
	#include "../src/audio.h"
#else
	#include "graphics.h"
	#include "audio.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define PALETTE_COLOR_SIZE 0.015f
#define PALETTE_OFFSET_X 0.25f
#define PALETTE_OFFSET_Y -0.25f

#define SPRITE_PIXEL_SIZE 0.025f
#define SPRITE_OFFSET_X -0.35
#define SPRITE_OFFSET_Y -0.20

#define BUTTON_SIZE 0.05f

#define BACKGROUND_CHANGE_RATE 0.025f
#define BACKGROUND_BRIGHTNESS 0.25f

#define MARKER_SIZE 0.005f

Render palette[256];
Render sprite[256];

Render saveButton;
Render mixer;

short paletteValues[256];
unsigned char * spriteValues = nullptr;

unsigned char spriteCount;
unsigned char currSprite = 0;
Render appendButton;
#define MAX_SPRITES 16

short clearShort = 0;

char * paletteFile;
char * spriteFile;

unsigned char selectedColor = 0x01;

short mixerColor = 0x7fff;
char h, s, v;
Render hue, sat, val;

Render hueMarker, satMarker, valMarker, paletteMarker;

unsigned char bpp = 4;

Font f;

Goodstr scstr;

Render text;

void setSpriteCount(unsigned char count)
{
    if (spriteValues == nullptr)
    {
        spriteValues = (unsigned char*)malloc(count*256);
        memset(spriteValues, 0, count*256);
    }
    else
    {
        unsigned char * temp = (unsigned char*)malloc(count*256);
        memset(temp, 0, count*256);
        memcpy(temp, spriteValues, spriteCount*256);
        free(spriteValues);
        spriteValues = temp;
    }
    spriteCount = count;
}

void onClickPalette(Render * clicked, int button)
{
    for(int i=0; i<256; i++)
        if (clicked->handle == palette[i].handle)
            if (button == MOUSE_LEFT)
                selectedColor = i;
            else if (button == MOUSE_RIGHT)
                paletteValues[i] = mixerColor;
    paletteMarker.pos = palette[selectedColor].pos;
}

void onClickSprite(Render * clicked, int button)
{
    for(int i=0; i<256; i++)
        if (clicked->handle == sprite[i].handle)
            if (button == MOUSE_LEFT)
                spriteValues[i+256*currSprite] = selectedColor;
            else if (button == MOUSE_RIGHT)
                spriteValues[i+256*currSprite] = 0;
}

void onClickSave(Render * clicked, int button)
{
    FILE * file = fopen(paletteFile, "wb");
    fwrite(paletteValues, 2, 256, file);
    fclose(file);

    file = fopen(spriteFile, "wb");
    fwrite(&spriteCount, 1, 1, file);
    fwrite(&bpp, 1, 1, file);
    fwrite(spriteValues, 256, spriteCount, file);
    fclose(file);
}

void onClickAppend(Render * clicked, int button)
{
    if (spriteCount < MAX_SPRITES)
        setSpriteCount(spriteCount + 1);
}

char posToColorVal(Render * clicked)
{
    double cx, cy;
    getCursorPos(&cx, &cy);

    if (clicked->handle == hue.handle)
        hueMarker.pos.x = cx;
    if (clicked->handle == sat.handle)
        satMarker.pos.x = cx;
    if (clicked->handle == val.handle)
        valMarker.pos.x = cx;

    cx -= (clicked->pos.x - clicked->scale.x);
    cx -= 0.1;
    cx *= 160.0;
    cx = std::max(std::min(cx, 31.0), 0.0);

    return (char)cx;
}

void onClickHue(Render * clicked, int button)
{
    h = posToColorVal(clicked);
}

void onClickSat(Render * clicked, int button)
{
    s = posToColorVal(clicked);
}

void onClickVal(Render * clicked, int button)
{
    v = posToColorVal(clicked);
}

void setRenderColors()
{
    for (int i=0; i<256; i++)
    {
        if (i != 0)
            palette[i].color.from5Bit(paletteValues[i]);
        
        int p = (1 << bpp);

        if (spriteValues[i+256*currSprite] % p != 0)
            sprite[i].color.from5Bit(paletteValues[(spriteValues[i+256*currSprite] % p) + (p * (selectedColor/p))]);
        else
            sprite[i].color = {1.0f, 1.0f, 1.0f, 0.05f};
        
    }

    mixerColor = (v << 10) + (s << 5) + h;

    mixer.color.from5BitHSV(mixerColor);
    mixerColor = mixer.color.to5Bit();
}

void setBackgroundColor()
{
    double t = currTime();
    clearColor.r = sinf(t * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.g = sinf(t * 1.5f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.b = sinf(t * 0.75f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;

    hueMarker.color = {clearColor.g, clearColor.b, clearColor.r, 1.0f};
    satMarker.color = hueMarker.color;
    valMarker.color = hueMarker.color;
    paletteMarker.color = hueMarker.color;
}

void loadFiles()
{
    memset(paletteValues, 0, 512);
    spriteCount = 1;
    
    FILE * file = fopen(paletteFile, "rb");
    if (file)
    {
        fread(paletteValues, 256*2, 1, file);
        fclose(file);
    }

    file = fopen(spriteFile, "rb");
    if (file)
    {
        fread(&spriteCount, 1, 1, file);
        setSpriteCount(spriteCount);
        fread(&bpp, 1, 1, file);
        fread(spriteValues, 256, spriteCount, file);
        fclose(file);
    }
    else
        setSpriteCount(spriteCount);


    return;
}

void switchSprites()
{
    static bool isAlreadyDown = false;
    
    if (getKeyDown('a') && !isAlreadyDown) { currSprite--; isAlreadyDown = true;}
    else if (getKeyDown('d') && !isAlreadyDown) {currSprite++; isAlreadyDown = true;}
    else if (!getKeyDown('a') && !getKeyDown('d')) {isAlreadyDown = false;}

    currSprite = currSprite % spriteCount;

    if (currSprite < 0)
        currSprite = spriteCount - 1;
}

void update()
{
    switchSprites();
    
    scstr.format("Sprite: %d/%d", currSprite+1, spriteCount);

    text.updateText(f, scstr);
    
    setBackgroundColor();
    setRenderColors();
}

void snesGraphicsEditor(int argc, char** argv)
{   
    spriteFile = "sprite.spr";
    if (argc > 1)
        spriteFile = argv[1];

    paletteFile = "palette.pal";
    if (argc > 2)
        paletteFile = argv[2];

    loadFiles();
    
    createWindow();
        
    initGraphics();

    f.fontSize = 72;
    f.createFont("Roboto-Regular.ttf", Font::getAscii());
    f.spaceLength = 0.33f;
    f.charSpace = 0.1f;
    f.lineSpace = 0.8f;

    Texture t;
    t.create("default");

    //Render r;
    //r.create(t);

    palette[0].create(t);
    palette[0].color = {1.0f, 1.0f, 1.0f, 0.05f};
    palette[0].scale = {PALETTE_COLOR_SIZE, PALETTE_COLOR_SIZE, 1.0f};
    palette[0].pos.x = (0) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_X;
    palette[0].pos.y = (0) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_Y;
    palette[0].onClick = onClickPalette;

    for (int i=1; i<256; i++)
    {
        palette[i].create(t);
        palette[i].scale = {PALETTE_COLOR_SIZE, PALETTE_COLOR_SIZE, 1.0f};
        palette[i].pos.x = (i % 16) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_X;
        palette[i].pos.y = (i / 16) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_Y;
        palette[i].onClick = onClickPalette;
    }

    Render ref;
    Texture refImage;

    if (argc > 3)
    {
        refImage.create(std::string(argv[3]));
        
        ref.create(refImage);
        ref.scale = {SPRITE_PIXEL_SIZE*16, SPRITE_PIXEL_SIZE*16, 1.0f};
        ref.pos.x = 7.5f * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_X;
        ref.pos.y = 7.5f * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_Y;
    }

    for (int i=0; i<256; i++)
    {
        sprite[i].create(t);
        sprite[i].scale = {SPRITE_PIXEL_SIZE, SPRITE_PIXEL_SIZE, 1.0f};
        sprite[i].pos.x = (i % 16) * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_X;
        sprite[i].pos.y = (i / 16) * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_Y;
        sprite[i].onClickDown = onClickSprite;
    }

    saveButton.create(t);
    saveButton.scale = {BUTTON_SIZE, BUTTON_SIZE, 1.0f};
    saveButton.pos.x = -0.35f;
    saveButton.pos.y = 0.25f;
    saveButton.onClick = onClickSave;

    Render saveText;
    Goodstr saveStr;
    saveStr.format("save");
    saveText.create(f, saveStr);
    saveText.scale = {0.01f, 0.01f, 1.0f};
    saveText.pos = saveButton.pos;
    saveText.color = BLACK;

    mixer.create(t);
    mixer.scale = saveButton.scale;
    mixer.pos.x = 0.35f;
    mixer.pos.y = 0.05f;

    hue.create(t);
    hue.scale.x = mixer.scale.x*4.0f;
    hue.scale.y = mixer.scale.y*0.5f;
    hue.pos.x = mixer.pos.x;
    hue.pos.y = mixer.pos.y + 0.06f;
    hue.onClickDown = onClickHue;

    sat.create(t);
    sat.scale.x = mixer.scale.x*4.0f;
    sat.scale.y = mixer.scale.y*0.5f;
    sat.pos.x = mixer.pos.x;
    sat.pos.y = mixer.pos.y + 0.1f;
    sat.onClickDown = onClickSat;

    val.create(t);
    val.scale.x = mixer.scale.x*4.0f;
    val.scale.y = mixer.scale.y*0.5f;
    val.pos.x = mixer.pos.x;
    val.pos.y = mixer.pos.y + 0.14f;
    val.onClickDown = onClickVal;

    h = 0b10000;
    s = 0b10000;
    v = 0b10000;

    hueMarker.create(t);
    hueMarker.scale = {MARKER_SIZE, MARKER_SIZE, 1.0f};
    hueMarker.pos = hue.pos;

    satMarker.create(t);
    satMarker.scale = {MARKER_SIZE, MARKER_SIZE, 1.0f};
    satMarker.pos = sat.pos;

    valMarker.create(t);
    valMarker.scale = {MARKER_SIZE, MARKER_SIZE, 1.0f};
    valMarker.pos = val.pos;

    scstr.format("Sprite: %d/%d", currSprite, spriteCount);

    text.create(f, scstr);
    text.scale = {0.02f, 0.02f, 1.0f};
    text.pos = saveButton.pos;
    text.pos.x += 0.2f;
    text.color = {0.0f, 0.0f, 0.0f, 1.0f};

    appendButton.create(t);
    appendButton.scale = {BUTTON_SIZE, BUTTON_SIZE, 1.0f};
    appendButton.pos.x = -0.25f;
    appendButton.pos.y = 0.25f;
    appendButton.onClick = onClickAppend;

    Render appendText;
    Goodstr appendStr;
    appendStr.format("append");
    appendText.create(f, appendStr);
    appendText.scale = {0.01f, 0.01f, 1.0f};
    appendText.pos = appendButton.pos;
    appendText.color = BLACK;

    paletteMarker.create(t);
    paletteMarker.scale = valMarker.scale;
    paletteMarker.pos = palette[selectedColor].pos;

    startLoop(update);
}