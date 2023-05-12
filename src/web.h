#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#include <emscripten/html5.h>
#include <emscripten.h>

GLbyte vShaderStr[] =
"precision mediump float; \n"
"attribute vec4 vPosition; \n"

"uniform vec2 pos; \n"
"uniform vec2 size;\n"
"uniform float rot;\n"
"uniform float fov;\n"
"uniform vec2 cameraPos;\n"
"uniform vec4 color; \n"

"varying vec4 fragColor;\n"

"void main() \n"
"{ \n"

"float hFov = fov/2.0;\n"
"\n"
"vec2 scaledObj = vPosition.xy * size;\n"
"    scaledObj = vec2(\n"
"    scaledObj.x / hFov,\n"
"    ((scaledObj.y / hFov) * 16.0) / 9.0\n"
");\n"
"\n"
"vec2 movedObj = vec2(\n"
"    (pos.x - cameraPos.x) / hFov,\n"
"    (((pos.y - cameraPos.y) / hFov) * 16.0) / -9.0\n"
");\n"
//"gl_Position = vec4(scaledObj, 0.0, 1.0);\n"
"gl_Position = vec4(movedObj + scaledObj, 0.0, 1.0);\n"

//"gl_Position = vPosition;\n"
"fragColor = color;\n"

"} \n";

GLbyte fShaderStr[] =
"precision mediump float; \n"

"varying vec4 fragColor; \n"

"void main() \n"
"{ \n"
" gl_FragColor = fragColor; \n"
"} \n";

class WebRender
{
public:
    GLint uniformBuffer;

    WebRender();
    ~WebRender();
};

class WebTexture
{
    GLint buffer;
}

class WebMesh
{
    
}