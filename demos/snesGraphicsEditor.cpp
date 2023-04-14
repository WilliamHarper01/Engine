#include "graphics.h"
#include "audio.h"
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

void setSpriteCount(unsigned char count)
{
    if (spriteValues == nullptr)
    {
        printf("we get here %d\n", count*256);
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
            if (button == 1)
                selectedColor = i;
            else if (button == 2)
                paletteValues[i] = mixerColor;
    paletteMarker.pos = palette[selectedColor].pos;
}

void onClickSprite(Render * clicked, int button)
{
    for(int i=0; i<256; i++)
        if (clicked->handle == sprite[i].handle)
            if (button == 1)
                spriteValues[i] = selectedColor;
            else if (button == 2)
                spriteValues[i] = 0;
}

void onClickSave(Render * clicked, int button)
{
    FILE * file = fopen(paletteFile, "wb");
    fwrite(paletteValues, 2, 256, file);
    fclose(file);

    file = fopen(spriteFile, "wb");
    fwrite(&spriteCount, 1, 1, file);
    fwrite(&bpp, 1, 1, file);
    fwrite(spriteValues, 1, 256, file);
    fclose(file);
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

        if (spriteValues[i] % p != 0)
            sprite[i].color.from5Bit(paletteValues[(spriteValues[i] % p) + (p * (selectedColor/p))]);
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
        for (int i=0; i<spriteCount; i++)
            fread(spriteValues, 256, 1, file);
        fclose(file);
    }
    else
        setSpriteCount(spriteCount);


    return;
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

    srand(time(NULL));
    
    createWindow();
        
    initVulkan();

    f.fontSize = 72;
    f.createFont("Roboto-Regular.ttf", Font::getAscii());
    f.spaceLength = 0.33f;
    f.charSpace = 0.1f;
    f.lineSpace = 0.8f;

    Texture t;
    t.create("E:/VisualStudio/Engine/white.png");

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

    for (int i=0; i<256; i++)
    {
        sprite[i].create(t);
        sprite[i].scale = {SPRITE_PIXEL_SIZE, SPRITE_PIXEL_SIZE, 1.0f};
        sprite[i].pos.x = (i % 16) * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_X;
        sprite[i].pos.y = (i / 16) * SPRITE_PIXEL_SIZE + SPRITE_OFFSET_Y;
        sprite[i].onClickDown = onClickSprite;
    }

    Texture save;
    save.create("E:/VisualStudio/Engine/save.png");

    saveButton.create(save);
    saveButton.scale = {BUTTON_SIZE, BUTTON_SIZE, 1.0f};
    saveButton.pos.x = -0.35f;
    saveButton.pos.y = 0.25f;
    saveButton.onClick = onClickSave;

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

    paletteMarker.create(t);
    paletteMarker.scale = valMarker.scale;
    paletteMarker.pos = palette[selectedColor].pos;

    Render text;
    std::string rstr = "hello, world!";
    text.create(f, rstr);
    text.scale = {0.05f, 0.05f, 1.0f};

    do
    {
        std::string rstr = "woa\nwe have newlines!";
        text.updateText(f, rstr);
        
        setBackgroundColor();
        setRenderColors();
    } while(renderFrame());
    cleanVulkan();
}