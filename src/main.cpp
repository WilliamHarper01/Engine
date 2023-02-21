#include "graphics.h"	//a front-end for everything graphics related
#include "audio.h"		//a front-end for everything audio related

int main()
{
	createWindow();
	initVulkan();
	//startAudio();

	do
	{
	
	} while (renderFrame());

	cleanVulkan();
}