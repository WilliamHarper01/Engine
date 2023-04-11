#include "graphics.h"
#include "audio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

Render palette[256];
Render sprite[256];

short paletteValues[256];
unsigned char spriteValues[256];

short clearShort = 0;

std::string gamePath = "../SNESgame/";

#define PALETTE_COLOR_SIZE 0.015f
#define PALETTE_OFFSET_X 0.25f
#define PALETTE_OFFSET_Y -0.25f

#define SPRITE_PIXEL_SIZE 0.025f
#define SPRITE_OFFSET_X -0.35
#define SPRITE_OFFSET_Y -0.20

#define BACKGROUND_CHANGE_RATE 0.025f
#define BACKGROUND_BRIGHTNESS 0.25f

void onClickPalette(Render * clicked)
{
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", clicked->color.r, clicked->color.g, clicked->color.b, clicked->color.a);
}

void onClickSprite(Render * clicked)
{

}

void setRenderColors()
{
    for (int i=0; i<256; i++)
    {
        if (i != 0)
            palette[i].color.from5Bit(paletteValues[i]);
        sprite[i].color.from5Bit(spriteValues[i]);
    }
}

void setBackgroundColor()
{
    double t = currTime();
    clearColor.r = sinf(t * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.g = sinf(t * 1.5f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.b = sinf(t * 0.75f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
}

void snesGraphicsEditor()
{   
    Color c;
    c.from5Bit(2000);
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", c.r, c.g, c.b, c.a);
    short result = c.to5Bit();
    printf("%d\n", result);

    srand(time(NULL));

    memset(paletteValues, 0, 512);
    memset(spriteValues, 0, 256);
    
    createWindow();
        
    initVulkan();

    Texture t;
    t.create("white.png");

    //Render r;
    //r.create(t);

    palette[0].create(t);
    palette[0].color = {1.0f, 1.0f, 1.0f, 0.05f};
    palette[0].scale = {PALETTE_COLOR_SIZE, PALETTE_COLOR_SIZE, 1.0f};
    palette[0].pos.x = (0) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_X;
    palette[0].pos.y = (0) * PALETTE_COLOR_SIZE + PALETTE_OFFSET_Y;

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
        sprite[i].onClick = onClickSprite;
    }

    do
    {
        setBackgroundColor();
        setRenderColors();
    } while(renderFrame());
    cleanVulkan();
}