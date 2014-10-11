#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include "vec2d.h"
#include <cstdlib>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;




//Initializes SDL2, Creates a window
bool init()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("OpenGl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if(window == NULL)
    {
        printf("Failed to create window.\n");
        return false;
    }
}

SDL_Texture *load_texture(const char* fileName)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer,fileName);
    if(texture == NULL)
    {
        printf("Could not load %s\n",fileName);
    }

    return texture;
}

void draw_square(float x, float y, float width, float height, float angle,int texture)
{

}

class Process
{
public:

    bool finished;
    Process *next;

    virtual void init()
    {

    };

    virtual void handle_events(SDL_Event *event)
    {

    };

    virtual void update(float dt)
    {

    };

    virtual void draw()
    {
    };

};

class ProcessButton
{
public:
    Process *next;
    bool pressed;
    int textureOut;
    int textureIn;
    float x;
    float y;
    float width;
    float height;

    ProcessButton()
    {

    };

    ProcessButton(Process *next, int textureOut, int textureIn, float x, float y, float width, float height)
    {
        this->next = next;
        this->pressed = false;
        this->textureIn = textureIn;
        this->textureOut = textureOut;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    };

    bool handle_events(SDL_Event *event, Process **processNext)
    {

        int mouseX = 0,mouseY = 0;
        SDL_GetMouseState(&mouseX,&mouseY);

        if((mouseX-(x+width/2))*(mouseX-(x+width/2))+(mouseY-(y+height/2))*(mouseY-(y+height/2))<(width/2)*(width/2))
        {
            if(SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                pressed = true;
            }
            if(event->type == SDL_MOUSEBUTTONUP)
            {
                if(event->button.button == SDL_BUTTON_LEFT)
                {
                    *processNext = next;
                    return true;
                }
            }
        }
        else
        {
            pressed = false;
        }
        return false;
    }

    void draw()
    {
        if(pressed)
        {
            draw_square(x,y,width,height,0,textureIn);
        }
        else
        {
            draw_square(x,y,width,height,0,textureOut);
        }
    }

};

class IntroAnimation : public Process
{
public:

    vec2d hatchPosition;
    vec2d hatchVelocity;
    vec2d hatchSize;
    float hatchEndHeight;
    int hatchTexture;

    ProcessButton aboutMe;
    ProcessButton interests;
    ProcessButton academics;
    ProcessButton exit;

    IntroAnimation
    (
        Process *next,
        Process *aboutMeProcess,
        Process *interestsProcess,
        Process *academicsProcess,
        vec2d hatchPosition,
        vec2d hatchSize,
        float hatchEndHeight,
        int hatchTexture
    )
    {
        this->next = next;
        finished = false;
        this->hatchPosition = hatchPosition;
        this->hatchSize = hatchSize;
        this->hatchEndHeight = hatchEndHeight;
        this->hatchTexture = hatchTexture;

        this->aboutMe = ProcessButton(aboutMeProcess,3,2,10,370,128,128);
        this->interests = ProcessButton(interestsProcess,3,2,436,370,128,128);
        this->academics = ProcessButton(academicsProcess,3,2,10,706,128,128);
        this->exit = ProcessButton(NULL,3,2,436,706,128,128);

    }

    void init()
    {
    }

    void handle_events(SDL_Event *event)
    {
        finished = aboutMe.handle_events(event,&next)
        || interests.handle_events(event,&next)
        || academics.handle_events(event,&next)
        || exit.handle_events(event,&next);
    }

    void update(float dt)
    {
        hatchVelocity.y = 1.7*(hatchEndHeight - hatchPosition.y);
        hatchPosition += dt*hatchVelocity;

        if(hatchPosition.y >= hatchEndHeight-1.0f)
        {
            hatchPosition.y = hatchEndHeight;
            hatchVelocity = vec2d(0.0f,0.0f);

        }
    }

    void draw()
    {
        Process::draw();

        draw_square(hatchPosition.x,hatchPosition.y,hatchSize.x,hatchSize.y,0,hatchTexture);

        aboutMe.draw();
        interests.draw();
        academics.draw();
        exit.draw();
    }

};

class Interests : public Process
{
public:
    void init()
    {
        SDL_SetRenderDrawColor(renderer,120,50,50,255);
    };

    void draw()
    {
        Process::draw();
    };

    Interests()
    {
        finished = false;
    };

};

void intro_init()
{
}

int main(int argc, char *argv[])
{
    if(!init())
    {
        return -1;
    }

    SDL_SetRenderDrawColor(renderer,255,255,255,255);


    float dt = 0;
    float prevTime = 0.0f;

    Process *interests = new Interests;

    Process *process = NULL;
    process = new IntroAnimation(NULL,NULL,interests,NULL,vec2d(44.0f,-1000.0f),vec2d(1024,128),10,0);
    process->init();



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
            }

            process->handle_events(&windowEvent);

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
            if(process == NULL)
                break;
            else
                process->init();

        }

        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
