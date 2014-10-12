#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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

    IMG_Init(IMG_INIT_PNG);

    TTF_Init();

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

void render_texture(SDL_Texture *texture, float x, float y, float w, float h)
{
    SDL_Rect destination;
    destination.x = x;
    destination.y = y;
    destination.w = w;
    destination.h = h;
    SDL_RenderCopy(renderer,texture,NULL,&destination);
}

SDL_Texture *render_text(const std::string &message, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font,message.c_str(),color);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,surface);

    SDL_FreeSurface(surface);

    return texture;
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
    SDL_Texture *textureOut;
    SDL_Texture *textureIn;
    float x;
    float y;
    float width;
    float height;

    ProcessButton()
    {

    };

    ProcessButton(Process *next, SDL_Texture *textureOut, SDL_Texture *textureIn, float x, float y, float width, float height)
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
            render_texture(textureIn,x,y,width,height);
        }
        else
        {
            render_texture(textureOut,x,y,width,height);
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

    SDL_Texture *hatchTexture;

    SDL_Texture *geraldHatch;

    ProcessButton aboutMe;
    ProcessButton interests;
    ProcessButton academics;
    ProcessButton exit;

    SDL_Texture *aboutMeTexture;
    SDL_Texture *interestsTexture;
    SDL_Texture *academicsTexture;
    SDL_Texture *exitTexture;

    TTF_Font *font;

    IntroAnimation
    (
        Process *next,
        Process *aboutMeProcess,
        Process *interestsProcess,
        Process *academicsProcess,
        vec2d hatchPosition,
        vec2d hatchSize,
        float hatchEndHeight,
        SDL_Texture *hatchTexture,

        SDL_Texture *buttonOut,
        SDL_Texture *buttonIn,

        TTF_Font *font
    )
    {
        this->next = next;
        finished = false;
        this->hatchPosition = hatchPosition;
        this->hatchSize = hatchSize;
        this->hatchEndHeight = hatchEndHeight;
        this->hatchTexture = hatchTexture;

        this->aboutMe = ProcessButton(aboutMeProcess,buttonOut,buttonIn,10,370,128,128);
        this->interests = ProcessButton(interestsProcess,buttonOut,buttonIn,436,370,128,128);
        this->academics = ProcessButton(academicsProcess,buttonOut,buttonIn,10,706,128,128);
        this->exit = ProcessButton(NULL,buttonOut,buttonIn,436,706,128,128);

        this->font = font;

        SDL_Color hatchBlue = {1,91,144,100};

        geraldHatch = render_text("Dr. Gerald G. Hatch Scholarship",font,hatchBlue);

        aboutMeTexture = render_text("About Me",font,hatchBlue);
        interestsTexture = render_text("Interests",font,hatchBlue);
        academicsTexture = render_text("Academics",font,hatchBlue);
        exitTexture = render_text("Exit",font,hatchBlue);

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

        render_texture(hatchTexture,hatchPosition.x,hatchPosition.y,hatchSize.x,hatchSize.y);


        int gHatchWidth;
        int gHatchHeight;
        SDL_QueryTexture(geraldHatch,NULL,NULL,&gHatchWidth,&gHatchHeight);

        render_texture(geraldHatch,SCREEN_WIDTH/2-(SCREEN_WIDTH-20)/2,200,SCREEN_WIDTH-20,128);

        aboutMe.draw();
        render_texture(aboutMeTexture,aboutMe.x+aboutMe.width+5,aboutMe.y,280,aboutMe.height);

        interests.draw();
        render_texture(interestsTexture,interests.x+interests.width+5,interests.y,280,interests.height);

        academics.draw();
        render_texture(academicsTexture,academics.x+academics.width+5,academics.y,280,academics.height);

        exit.draw();
        render_texture(exitTexture,exit.x+exit.width+5,exit.y,150,exit.height);
    }

};

class Interests : public Process
{
public:

    float scroll;

    void init()
    {
        SDL_SetRenderDrawColor(renderer,21,164,174,255);
    };

    void draw()
    {
        Process::draw();
    };

    Interests()
    {
        scroll = 0.0f;
        finished = false;
    };

};


int main(int argc, char *argv[])
{
    if(!init())
    {
        return -1;
    }

    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    SDL_Texture *hatchTexture = load_texture("assets/hatch_logo.png");
    SDL_Texture *buttonOut = load_texture("assets/button_out.png");
    SDL_Texture *buttonIn = load_texture("assets/button_in.png");

    TTF_Font *berbas = TTF_OpenFont("assets/berbas.ttf",96);


    float dt = 0;
    float prevTime = 0.0f;

    Process *interests = new Interests;

    Process *process = NULL;
    process = new IntroAnimation
    (
         NULL,
         NULL,
         interests,
         NULL,
         vec2d(44.0f,-1000.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
     );

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

        process->draw();

        SDL_RenderPresent(renderer);

    }
    SDL_DestroyTexture(buttonIn);
    SDL_DestroyTexture(buttonOut);
    SDL_DestroyTexture(hatchTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
