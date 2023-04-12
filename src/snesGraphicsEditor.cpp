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

Render saveButton;
Render mixer;

short paletteValues[256];
unsigned char spriteValues[256];

short clearShort = 0;

char * paletteFile;
char * spriteFile;

unsigned char selectedColor = 0x01;
short mixerColor = 0x7fff;

#define PALETTE_COLOR_SIZE 0.015f
#define PALETTE_OFFSET_X 0.25f
#define PALETTE_OFFSET_Y -0.25f

#define SPRITE_PIXEL_SIZE 0.025f
#define SPRITE_OFFSET_X -0.35
#define SPRITE_OFFSET_Y -0.20

#define BUTTON_SIZE 0.05f

#define BACKGROUND_CHANGE_RATE 0.025f
#define BACKGROUND_BRIGHTNESS 0.25f

void onClickPalette(Render * clicked, int button)
{
    for(int i=0; i<256; i++)
        if (clicked->handle == palette[i].handle)
            if (button == 1)
                selectedColor = i;
            else if (button == 2)
                paletteValues[i] = mixerColor;
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
    FILE * file = fopen(paletteFile, "w");
    fwrite(paletteValues, 2, 256, file);
    fclose(file);

    file = fopen(spriteFile, "w");
    fwrite(spriteValues, 1, 256, file);
    fclose(file);
}

void setRenderColors()
{
    for (int i=0; i<256; i++)
    {
        if (i != 0)
            palette[i].color.from5Bit(paletteValues[i]);
        if (spriteValues[i] != 0)
            sprite[i].color.from5Bit(paletteValues[spriteValues[i]]);
        else
            sprite[i].color = {1.0f, 1.0f, 1.0f, 0.05f};
    }
    mixer.color.from5Bit(mixerColor);
}

void setBackgroundColor()
{
    double t = currTime();
    clearColor.r = sinf(t * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.g = sinf(t * 1.5f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
    clearColor.b = sinf(t * 0.75f * BACKGROUND_CHANGE_RATE) * BACKGROUND_BRIGHTNESS;
}

void loadFiles()
{
    memset(paletteValues, 0, 512);
    memset(spriteValues, 0, 256);
    
    FILE * file = fopen(paletteFile, "r");
    if (file == NULL)
        file = fopen(paletteFile, "w+");
    fread(paletteValues, 256*2, 1, file);
    fclose(file);

    file = fopen(spriteFile, "r");
    if (file == NULL)
        file = fopen(spriteFile, "w+");
    fread(spriteValues, 256, 1, file);
    fclose(file);
}

void snesGraphicsEditor(int argc, char** argv)
{   
    paletteFile = "palette.dmp";
    if (argc > 1)
        paletteFile = argv[1];

    spriteFile = "sprite.dmp";
    if (argc > 2)
        spriteFile = argv[2];

    loadFiles();

    
    Color c;
    c.from5BitHSV(0b0111110000011111);
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", c.r, c.g, c.b, c.a);
    short result = c.to5BitHSV();
    printf("%d\n", result);

    srand(time(NULL));
    
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

    Texture save;
    save.create("save.png");

    saveButton.create(save);
    saveButton.scale = {BUTTON_SIZE, BUTTON_SIZE, 1.0f};
    saveButton.pos.x = -0.35f;
    saveButton.pos.y = 0.25f;
    saveButton.onClick = onClickSave;

    mixer.create(t);
    mixer.scale = saveButton.scale;
    mixer.pos.x = 0.35f;
    mixer.pos.y = 0.05f;

    do
    {
        setBackgroundColor();
        setRenderColors();
    } while(renderFrame());
    cleanVulkan();
}