#if __clang__
	#include "../src/graphics.h"
	#include "../src/audio.h"
#else
	#include "graphics.h"
	#include "audio.h"
#endif

Mesh cube;
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

    cubeRender.rot.z += (curr - lastTime);
    lastTime = curr;
}

int cubespin(int argc, char** argv) 
{
    createWindow();
    initGraphics();

    cube.create("webbuild/assets/cube.obj");
    tex.create(pixels, 2, 2);
    //tex.create("tex.jpg");
    cubeRender.create(cube, tex);
    cubeRender.scale = V3(0.1f, 0.1f, 1.0f);
    cubeRender.pos = V3(0.5f, 0.0f, 1.0f);

    startLoop(update);
    
    return 0;
}