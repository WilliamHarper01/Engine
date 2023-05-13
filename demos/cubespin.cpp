#if __clang__
	#include "../src/graphics.h"
	#include "../src/audio.h"
#else
	#include "graphics.h"
	#include "audio.h"
#endif

Model model;
Texture tex;
Render cubeRender;

unsigned char pixels[] = {
    255, 0, 0, 255,
    255, 0, 0, 255,
    255, 0, 0, 255,
    255, 0, 0, 255
};

void update()
{
    static double lastTime = 0;
    static double curr = 0;
    curr = currTime();

    cubeRender.rot.x += curr - lastTime;
    lastTime = curr;
}

int cubespin(int argc, char** argv) 
{
    createWindow();
    initGraphics();

    model.create("E:/VisualStudio/Engine/model.glb");
    cubeRender.create(model);
    cubeRender.scale = V3(1.0f, 1.0f, 1.0f);
    cubeRender.pos = V3(0.0f, 0.0f, 1.0f);

    startLoop(update);
    
    return 0;
}