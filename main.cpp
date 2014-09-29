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

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

GLuint programID;


//Initializes SDL2, Creates a window an OpenGL context and initalizes GLEW
//Returns false if any of them fail to initialize
bool init()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("OpenGl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

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

void draw_square(float x, float y, float width, float height, float angle)
{
    GLuint elements[] =
    {
        0,1,2,
        0,2,3
    };

    GLint uniModel = glGetUniformLocation(programID,"model");
    GLint uniView = glGetUniformLocation(programID, "view");
    GLint uniProj = glGetUniformLocation(programID, "proj");

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 proj = glm::ortho(0.0f,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT,0.0f);

    glm::mat4 s = glm::scale(glm::mat4(1.0f),glm::vec3(width,height,1.0f));
    glm::mat4 p = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,height/2.0f,0));
    glm::mat4 r = glm::rotate(glm::mat4(1.0f),angle,glm::vec3(0.0f,0.0f,-1.0f));
    glm::mat4 t = glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0));

    model = t*r*p*s;

    glUniformMatrix4fv(uniModel,1,GL_FALSE,glm::value_ptr(model));
    glUniformMatrix4fv(uniView,1,GL_FALSE,glm::value_ptr(view));
    glUniformMatrix4fv(uniProj,1,GL_FALSE,glm::value_ptr(proj));

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,elements);

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

    float mass = 1.0f;
    vec2d position(25.0f,5.0f);
    vec2d springs[] =
    {
        vec2d(29.0f,10.0f),
        vec2d(25.0f,45.0f),
        vec2d(40.0f,25.0f),
        vec2d(5.0f,30.0f),
    };
    vec2d velocity(0.0f,0.0f);
    float k = 4.0f;
    float c = 0.3;
    float cd = 1.05f;
    float rho = 1.0f;
    float e = 0.7;
    vec2d fg(0.0f,9.8*mass);
    vec2d fs(0.0f,0.0f);
    vec2d fv(0.0f,0.0f);
    vec2d fd(0.0f,0.0f);
    vec2d fa(0.0f,0.0f);

    float dt = 0;
    float prevTime = 0.0f;

    SDL_Event windowEvent;
    while(true)
    {
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
                if(windowEvent.key.keysym.sym == SDLK_SPACE)
                {
                    fa = vec2d(0.0f,0.0f);
                }
            }

            if(windowEvent.type == SDL_KEYDOWN)
            {
                if(windowEvent.key.keysym.sym == SDLK_SPACE)
                {
                    fa = vec2d(70.0f,0.0f);
                }
            }

        }

        float time = (float)SDL_GetTicks()/1000.0f;
        dt = time - prevTime;
        prevTime = time;

        fs = vec2d(0.0f,0.0f);

        for(int i = 0; i < sizeof(springs)/sizeof(vec2d); ++i)
        {
            fs+= -k*(position-springs[i]);
        }

        fv = -c*velocity;

        vec2d force = fg + fs + fv + fa;
        vec2d acceleration = force/mass;
        position += velocity*dt + 0.5*acceleration*dt*dt;
        velocity += acceleration*dt;

        if(position.y*10 + 20 >= SCREEN_HEIGHT && velocity.y > 0)
        {
            velocity.y *= -e;
        }

        glClear( GL_COLOR_BUFFER_BIT);
        for(int i = 0; i < sizeof(springs)/sizeof(vec2d); ++i)
        {
            draw_square(springs[i].x*10,springs[i].y*10,1,sqrtf((position.x-springs[i].x)*(position.x-springs[i].x)+(position.y-springs[i].y)*(position.y-springs[i].y))*10,atan2f(position.x-springs[i].x,position.y-springs[i].y));
        }
        draw_square(position.x*10,position.y*10,20,20,0);

        SDL_GL_SwapWindow(window);

    }
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);

    glDeleteProgram(programID);

    SDL_GL_DeleteContext(context);

    SDL_Quit();
    return 0;
}
