#define GLEW_STATIC
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "vec2d.h"
#include <cstdlib>

SDL_Window *window = NULL;
SDL_GLContext context;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;

GLuint programID;


//Initializes SDL2, Creates a window an OpenGL context and initalizes GLEW
//Returns false if any of them fail to initialize
bool init()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("OpenGl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

    if(window == NULL)
    {
        printf("Failed to create window.\n");
        return false;
    }

    context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if(glewInitResult!=0)
    {
        printf("%s\n",glewGetErrorString(glewInitResult));
        return false;
    }
    return true;
}

GLuint load_shaders(const char *vertexFilePath, const char *fragmentFilePath)
{


    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderCode;
    std::fstream vertexShaderStream(vertexFilePath,std::ios::in);
    if(vertexShaderStream.is_open())
    {
        std::string line ="";
        while(std::getline(vertexShaderStream,line))
        {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    }

    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(fragmentFilePath,std::ios::in);
    if(fragmentShaderStream.is_open())
    {
        std::string line ="";
        while(std::getline(fragmentShaderStream,line))
        {
            fragmentShaderCode+= "\n" + line;
        }
        fragmentShaderStream.close();
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    printf("Compiling shader: %s\n",vertexFilePath);
    char const *vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID,1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector <char> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
    printf("%s\n", &vertexShaderErrorMessage[0]);

    printf("Compiling shader: %s\n",fragmentFilePath);
    char const *fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID,1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector <char> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
    printf("%s\n", &fragmentShaderErrorMessage[0]);

    printf("Linking program\n");
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage(std::max(infoLogLength,int(1)));
    glGetShaderInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
    printf("%s\n", &programErrorMessage[0]);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

GLuint load_texture(const char* filepath, int texture)
{
    GLuint textureID;
    glActiveTexture(texture);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height;
    unsigned char* image = SOIL_load_image(filepath,&width,&height,0,SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

void draw_square(float x, float y, float width, float height, float angle,int texture)
{
    GLuint elements[] =
    {
        0,1,2,
        0,2,3
    };

    GLint uniModel = glGetUniformLocation(programID,"model");
    GLint uniView = glGetUniformLocation(programID, "view");
    GLint uniProj = glGetUniformLocation(programID, "proj");

    glUniform1i(glGetUniformLocation(programID,"tex"),texture);

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 proj = glm::ortho(0.0f,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT,0.0f);

    glm::mat4 s = glm::scale(glm::mat4(1.0f),glm::vec3(width,height,1.0f));
    glm::mat4 p = glm::translate(glm::mat4(1.0f),glm::vec3(width/2.0f,height/2.0f,0));
    glm::mat4 r = glm::rotate(glm::mat4(1.0f),angle,glm::vec3(0.0f,0.0f,-1.0f));
    glm::mat4 t = glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0));

    model = t*r*p*s;

    glUniformMatrix4fv(uniModel,1,GL_FALSE,glm::value_ptr(model));
    glUniformMatrix4fv(uniView,1,GL_FALSE,glm::value_ptr(view));
    glUniformMatrix4fv(uniProj,1,GL_FALSE,glm::value_ptr(proj));

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,elements);

}

class Process
{
public:

    bool finished;
    Process *next;

    virtual void init()
    {

    };

    virtual void update(float dt)
    {

    };

    virtual void draw()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    };

};

class IntroAnimation : public Process
{
public:

    vec2d hatchPosition;
    vec2d hatchVelocity;
    vec2d hatchSize;
    float hatchEndHeight;
    int hatchTexture;
    vec2d mottPosition;
    vec2d mottVelocity;
    vec2d mottSize;
    float mottEndPos;
    int mottTexture;

    IntroAnimation
    (
        Process *next,
        vec2d hatchPosition,
        vec2d hatchSize,
        float hatchEndHeight,
        int hatchTexture,
        vec2d mottPosition,
        vec2d mottSize,
        float mottEndPos,
        int mottTexture
    )
    {
        this->next = next;
        finished = false;
        this->hatchPosition = hatchPosition;
        this->hatchSize = hatchSize;
        this->hatchEndHeight = hatchEndHeight;
        this->hatchTexture = hatchTexture;
        this->mottPosition = mottPosition;
        this->mottSize = mottSize;
        this->mottEndPos = mottEndPos;
        this->mottTexture = mottTexture;
    }

    void init()
    {
        glClearColor(1.0f,1.0f,1.0f,1.0f);
    }

    void update(float dt)
    {
        hatchVelocity.y = 2*(hatchEndHeight - hatchPosition.y);
        hatchPosition += dt*hatchVelocity;

        mottPosition += dt*mottVelocity;

        if(hatchPosition.y >= hatchEndHeight-1.0f)
        {
            hatchPosition.y = hatchEndHeight;
            hatchVelocity = vec2d(0.0f,0.0f);

            mottVelocity.x = 4*(mottEndPos - mottPosition.x);
        }

        if(mottPosition.x >= mottEndPos - 1.2f)
        {
            mottPosition.x = mottEndPos;

        }

    }

    void draw()
    {
        Process::draw();

        draw_square(hatchPosition.x,hatchPosition.y,hatchSize.x,hatchSize.y,0,hatchTexture);
        draw_square(mottPosition.x,mottPosition.y,mottSize.x,mottSize.y,0,mottTexture);

        draw_square(10,444,256,256,0,3);
        draw_square(266,494,350,128,0,4);
        draw_square(650,444,256,256,0,3);
        draw_square(10,444+256+10,256,256,0,3);
        draw_square(650,444+256+10,256,256,0,3);

    }

};

void intro_init()
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
}

int main(int argc, char *argv[])
{
    if(!init())
    {
        return -1;
    }

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    programID = load_shaders("source/shaders/vs.vert","source/shaders/fs.vert");
    glUseProgram(programID);

    GLfloat vertices[] =
    {
        0.5f,0.5f,0.0f, 1.0f,
        -0.5f,0.5f,1.0f,1.0f,
        -0.5f,-0.5f,1.0f,0.0f,
        0.5f,-0.5f,0.0f,0.0f
    };

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    GLuint vertexBufferID;
    glGenBuffers(1,&vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    GLuint posAttrib = glGetAttribLocation(programID,"pvPosition");
    glVertexAttribPointer(posAttrib,2, GL_FLOAT, GL_FALSE,4*sizeof(float),0);
    glEnableVertexAttribArray(posAttrib);

    GLuint texAttrib = glGetAttribLocation(programID,"pvTexCoord");
    glVertexAttribPointer(texAttrib,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    GLuint textureID[5] =
    {
        load_texture("assets/hatch_logo.png",GL_TEXTURE0),
        load_texture("assets/hmm_logo.png",GL_TEXTURE1),
        load_texture("assets/button_in.png",GL_TEXTURE2),
        load_texture("assets/button_out.png",GL_TEXTURE3),
        load_texture("assets/about_me.png",GL_TEXTURE4)
    };

    float dt = 0;
    float prevTime = 0.0f;

    Process *process = NULL;

    process = new IntroAnimation(NULL,vec2d(44.0f,-300.0f),vec2d(1024,128),10,0,
                                 vec2d(-1050.0f,148.0f),vec2d(1024,256),44,1);
    process->init();



    SDL_Event windowEvent;
    while(true)
    {
        if(process == NULL)
        {
            break;
        }

        if(SDL_PollEvent(&windowEvent))
        {
            if(windowEvent.type == SDL_QUIT)
            {
                break;
            }
            if(windowEvent.type == SDL_KEYUP)
            {
                if(windowEvent.key.keysym.sym == SDLK_ESCAPE)
                {
                    break;
                }
            }

            if(windowEvent.type == SDL_KEYDOWN)
            {

            }

        }

        float time = (float)SDL_GetTicks()/1000.0f;
        dt = time - prevTime;
        prevTime = time;

        process->update(dt);

        process->draw();

        if(process->finished)
        {
            Process *completed = process;
            process = process->next;
            delete(completed);
        }


        SDL_GL_SwapWindow(window);

    }
    glDeleteTextures(sizeof(textureID)/sizeof(GLuint), textureID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);

    glDeleteProgram(programID);

    SDL_GL_DeleteContext(context);

    SDL_Quit();
    return 0;
}
