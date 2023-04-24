#include "graphics.h"
#include "web.h"

Color clearColor = {1.0f, 0.0f, 0.0f, 1.0f};

std::vector<Render*> objects = {};
V3 cameraPos(-0.0f, 0.0f, 0.70f);
V3 cameraLook(1.0f, 0.0f, 0.0f);

GLuint buffer;

GLfloat qvert[] = {-0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f, 
                0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f, 
                -0.5f, 0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f};

EmscriptenWebGLContextAttributes attrs;
const char* domCanvas = "nocanvas";

void (*updateLoop)();

double startTime = 0.0;

GLuint programObject;

GLuint vertexShader;
GLuint fragmentShader;
GLint linked;

int uboPosLoc;
int uboSizeLoc;
int uboRotLoc;
int uboFovLoc;
int uboCameraLoc;
int uboColorLoc;

std::vector<WebRender*> renders;

bool checkInside(double dx, double dy, double sx, double sy, double sw, double sh)
{
    sw /= 2.0;
    sh /= 2.0;

    double xmin = sx - sw;
    double xmax = sx + sw;

    double ymin = sy - sh;
    double ymax = sy + sh;

    if (dx < xmin)
        return false;
    if (dx > xmax)
        return false;
    if (dy < ymin)
        return false;
    if (dy > ymax)
        return false;

    return true;
}

void screenToWorld(double * x, double * y)
{
    int width, height;
    emscripten_get_canvas_element_size(domCanvas, &width, &height);

    *x = (*x)/((double)width)-0.5;
    *y = (*y)/((double)width)-(height/(float)width/2.0f);
}

EM_BOOL clickHandler(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    double x = (double)mouseEvent->targetX, y = (double)mouseEvent->targetY;
    screenToWorld(&x, &y);

    for (int i=0; i<objects.size(); i++)
    {
        if (checkInside(x, y, objects[i]->pos.x, objects[i]->pos.y, objects[i]->scale.x, objects[i]->scale.y))
        {
            if (objects[i]->onClick != nullptr)
                objects[i]->onClick(objects[i], mouseEvent->button);
            if (objects[i]->onClickDown != nullptr)
                objects[i]->onClickDown(objects[i], mouseEvent->button);

        }

    }
    return true;
}

void Mesh::create(std::string filename) {
    
}

Mesh::~Mesh() {
    
}

void Texture::create(std::string filename) {
    
}

Texture::~Texture() {
    
}

Font::~Font() {
    
}

void Font::createFont(std::string filename, std::vector<int> extraChars) {
    
}

WebRender::WebRender()
{
    renders.push_back(this);
}

WebRender::~WebRender()
{
    for(int i=0; i<renders.size(); i++)
    {
        if (renders[i] == this) renders.erase(i + renders.begin()); return;
    }
}

Render::~Render() {
    for(int i=0; i<objects.size(); i++)
    {
        if (objects[i] == this) objects.erase(i + objects.begin()); return;
    }
}

void Render::create(Mesh& mesh, Texture& tex)
{

}
void Render::create(Texture& tex)
{
    WebRender* r = new WebRender();
    this->handle = r;
    is3d = false;
    scale.x = 1.0;
    scale.y = 1.0;
    pos.z = 1.0;
    objects.push_back(this);
    isCreated = true;
}
void Render::create(Font& font, Goodstr & unicodeText)
{

}

void Render::updateText(Font& font, Goodstr & text) {
    
}

void createWindow() {
    domCanvas = "canvas";
}

GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);
    if(shader == 0)
        return 0;
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = (char*) malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void initGraphics() {
    attrs.alpha = true;
    attrs.depth = true;
    attrs.stencil = true;
    attrs.antialias = true;
    attrs.premultipliedAlpha = false;
    attrs.preserveDrawingBuffer = false;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    attrs.failIfMajorPerformanceCaveat = false;
    attrs.majorVersion = 1;
    attrs.minorVersion = 0;
    attrs.enableExtensionsByDefault = false;

    int ctx = emscripten_webgl_create_context(domCanvas, &attrs);
    if(!ctx)
    {
        printf("Webgl ctx could not be created!\n");
        return;
    }    

    emscripten_webgl_make_context_current(ctx);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fShaderStr);
    // Create the program object
    programObject = glCreateProgram();
    if(programObject == 0)
        return;
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");
    // Link the program
    glLinkProgram(programObject);
    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if(!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            printf("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return;
    }

    glUseProgram(programObject);

    uboPosLoc = glGetUniformLocation(programObject, "pos");
    uboSizeLoc = glGetUniformLocation(programObject, "size");
    uboRotLoc = glGetUniformLocation(programObject, "rot");
    uboFovLoc = glGetUniformLocation(programObject, "fov");
    uboCameraLoc = glGetUniformLocation(programObject, "cameraPos");
    uboColorLoc = glGetUniformLocation(programObject, "color");

    //if (uboPosLoc < 0) printf("could not find pos\n");
    //if (uboSizeLoc < 0) printf("could not find size\n");
    //if (uboRotLoc < 0) printf("could not find rot\n");
    //if (uboFovLoc < 0) printf("could not find fov\n");
    //if (uboCameraLoc < 0) printf("could not find camera\n");
    //if (uboColorLoc < 0) printf("could not find color\n");

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(qvert), qvert, GL_STATIC_DRAW);

    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    emscripten_set_mousedown_callback(domCanvas, nullptr, true, clickHandler);

    startTime = emscripten_get_now()/1000.0;
}

void renderFrame()
{
    updateLoop();

    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    for (int i=0; i<renders.size(); i++)
    {
        Render * r = objects[i];
        glUniform2f(uboPosLoc, r->pos.x, r->pos.y);
        glUniform2f(uboSizeLoc, r->scale.x, r->scale.y);
        glUniform1f(uboRotLoc, r->rot.x);
        glUniform1f(uboFovLoc, r->scale.z);
        glUniform2f(uboCameraLoc, 0.0f, 0.0f);
        glUniform4f(uboColorLoc, r->color.r, r->color.g, r->color.b, r->color.a);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glFinish();

}

bool startLoop(void (*updateCallback)()) {
    updateLoop = updateCallback;
    emscripten_set_main_loop(renderFrame, 0, 0);
    return false;
}

void cleanGraphics() {
    
}

double currTime() {
    double curr = emscripten_get_now()/1000.0 - startTime;
    return curr;   
}

void getCursorPos(double * x, double * y) {
    EmscriptenMouseEvent event;
    emscripten_get_mouse_status(&event);
    *x = (double)event.targetX; 
    *y = (double)event.targetY;
    screenToWorld(x, y);
}

bool getKeyDown(char key) {
    return false;
}