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

SDL_Texture *hatchTexture = NULL;
SDL_Texture *buttonOut = NULL;
SDL_Texture *buttonIn = NULL;
SDL_Texture *kaiTexture = NULL;
SDL_Texture *alexTexture = NULL;
SDL_Texture *napoleonTexture = NULL;
SDL_Texture *codeTexture = NULL;
SDL_Texture *aluTexture = NULL;
SDL_Texture *cpuTexture = NULL;
SDL_Texture *higgsTexture = NULL;
SDL_Texture *nuclearTexture = NULL;
SDL_Texture *waterlooTexture = NULL;
SDL_Texture *queenTexture = NULL;
SDL_Texture *awardsTexture = NULL;
SDL_Texture *sdlTexture;
SDL_Texture *gccTexture;
SDL_Texture *cbTexture;
SDL_Texture *mingwTexture;
SDL_Texture *cppTexture;

TTF_Font *berbas = NULL;


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
    if(texture == NULL)
    {
        printf("Texture is NULL\n");
        return;
    }
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

    SDL_Texture *geraldHatch;

    ProcessButton aboutMe;
    ProcessButton interests;
    ProcessButton academics;
    ProcessButton about;
    ProcessButton mandelbrot;
    ProcessButton exit;

    SDL_Texture *aboutMeTexture;
    SDL_Texture *interestsTexture;
    SDL_Texture *academicsTexture;
    SDL_Texture *aboutTexture;
    SDL_Texture *mandelbrotTexture;
    SDL_Texture *exitTexture;

    Process *aboutMeProcess;
    Process *interestsProcess;
    Process *academicsProcess;
    Process *mandelbrotProcess;
    Process *aboutProcess;

    TTF_Font *font;

    IntroAnimation
    (
        vec2d hatchPosition,
        vec2d hatchSize,
        float hatchEndHeight,
        SDL_Texture *hatchTexture,

        SDL_Texture *buttonOut,
        SDL_Texture *buttonIn,

        TTF_Font *font
    );


    void init();

    void handle_events(SDL_Event *event)
    {
        finished = aboutMe.handle_events(event,&next)
        || interests.handle_events(event,&next)
        || academics.handle_events(event,&next)
        || exit.handle_events(event,&next)
        || about.handle_events(event,&next)
        || mandelbrot.handle_events(event,&next);
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

        about.draw();
        render_texture(aboutTexture,about.x+about.width+5,about.y,200,about.height);

        mandelbrot.draw();
        render_texture(mandelbrotTexture,mandelbrot.x+mandelbrot.width+5,mandelbrot.y,280,mandelbrot.height);

        exit.draw();
        render_texture(exitTexture,exit.x+exit.width+5,exit.y,150,exit.height);
    }

};

class Interests : public Process
{
public:

    float scroll;

    float velocity;

    ProcessButton goBack;

    Process *goBackProcess;
    SDL_Texture *goBackTexture;

    SDL_Texture *pText;
    SDL_Texture *cText;
    SDL_Texture *hText;
    SDL_Texture *sText;

    float length;

    void init()
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        this->goBackProcess = new IntroAnimation
        (
         vec2d(44.0f,-200.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
        );

        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);
    };

    void handle_events(SDL_Event *event)
    {
        if(!finished)
            finished = goBack.handle_events(event,&next);
    };

    void update(float dt)
    {
        scroll += velocity*dt;
        if(scroll >= length)
        {
            next = goBackProcess;
            finished = true;
        }

    };

    void draw()
    {
        render_texture(alexTexture,1550.0f-scroll,300,300,300);
        render_texture(napoleonTexture,1900.0-scroll,50,400,473);
        render_texture(pText,200-scroll,10,700,100);
        render_texture(codeTexture,200-scroll,160,494,640);
        render_texture(cText,800-scroll,500,500,100);
        render_texture(aluTexture,900-scroll,10,500,400);
        render_texture(cpuTexture,900-scroll,700,300,300);
        render_texture(hText,1500-scroll,100,300,100);
        render_texture(sText,2200-scroll,700,300,100);
        render_texture(higgsTexture,2600-scroll,500,500,500);
        render_texture(nuclearTexture,2500-scroll,10,470,400);
        goBack.draw();
        render_texture(goBackTexture,goBack.x+goBack.width + 5,goBack.y,200,goBack.height);


    };

    ~Interests()
    {
    SDL_DestroyTexture(goBackTexture);

    SDL_DestroyTexture(pText);
    SDL_DestroyTexture(cText);
    SDL_DestroyTexture(hText);
    SDL_DestroyTexture(sText);
    }

    Interests()
    {
        scroll = 0.0f;
        finished = false;
        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);

        SDL_Color hatchBlue = {1,91,144,100};
        goBackTexture = render_text("Back",berbas,hatchBlue);

        pText = render_text("Programming... Obviously",berbas,hatchBlue);
        cText = render_text("Anything Computers",berbas,hatchBlue);
        hText = render_text("History",berbas,hatchBlue);
        sText = render_text("Science",berbas,hatchBlue);


        velocity = 70.0f;

        length = 2000.0f;

    };

};

class AboutMe : public Process
{
public:

    vec2d kaiPosition;
    vec2d kaiDimensions;

    ProcessButton goBack;
    Process *goBackProcess;

    SDL_Texture *goBackTexture;

    SDL_Texture *nameTexture;
    SDL_Texture *ageTexture;
    SDL_Texture *schoolTexture;
    SDL_Texture *languagesTexture;

    TTF_Font *font;

    void init()
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        this->goBackProcess = new IntroAnimation
        (
         vec2d(44.0f,-200.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
        );

        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);

    };

    void draw()
    {
        goBack.draw();
        render_texture(goBackTexture,goBack.x+goBack.width + 5,goBack.y,200,goBack.height);
        render_texture(kaiTexture,kaiPosition.x,kaiPosition.y,kaiDimensions.x,kaiDimensions.y);
        render_texture(nameTexture,10,340,500,100);
        render_texture(ageTexture,10,450,200,100);
        render_texture(schoolTexture,10,560,600,100);
        render_texture(languagesTexture,10,670,700,100);

    };

    void handle_events(SDL_Event *event)
    {
        finished = goBack.handle_events(event,&next);
    }

    ~AboutMe()
    {
    SDL_DestroyTexture(goBackTexture);

    SDL_DestroyTexture(nameTexture);
    SDL_DestroyTexture(ageTexture);
    SDL_DestroyTexture(schoolTexture);
    SDL_DestroyTexture(languagesTexture);
    }

    AboutMe
    (
        SDL_Texture *kaiTexture,
        vec2d kaiPosition,
        vec2d kaiDimensions,
        SDL_Texture *buttonOut,
        SDL_Texture *buttonIn,
        TTF_Font *font
    )
    {
        this->kaiPosition = kaiPosition;
        this->kaiDimensions = kaiDimensions;

        SDL_Color hatchBlue = {1,91,144,100};
        goBackTexture = render_text("Back",font,hatchBlue);

        nameTexture = render_text("Name:  Kai  Rusch",font,hatchBlue);
        ageTexture = render_text("Age:  17",font,hatchBlue);
        schoolTexture = render_text("School:  Aurora  High  School",font,hatchBlue);
        languagesTexture = render_text("Speaks:  German,  English,  French",font,hatchBlue);

        finished = false;
    };

};

class Academics : public Process
{
public:
 float scroll;

    ProcessButton goBack;

    Process *goBackProcess;
    SDL_Texture *goBackTexture;

    SDL_Texture *sText;
    SDL_Texture *eText;

    SDL_Texture *aText;

    float height;

    float velocity;

    void init()
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        this->goBackProcess = new IntroAnimation
        (
         vec2d(44.0f,-200.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
        );

        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);
    };

    void handle_events(SDL_Event *event)
    {
        if(!finished)
            finished = goBack.handle_events(event,&next);
    };

    void update(float dt)
    {
      scroll += velocity *dt;
      if(scroll >= height)
      {
            next = goBackProcess;
            finished = true;
      }
    };

    void draw()
    {

        render_texture(waterlooTexture,200,800-scroll,450,250);
        render_texture(queenTexture,670,800-scroll,450,250);
        render_texture(sText,390,600-scroll,400,100);
        render_texture(eText,270,1100-scroll,700,100);
        render_texture(aText,320,1400-scroll,600,100);
        render_texture(awardsTexture,110,1700-scroll,1150,2050);

        goBack.draw();
        render_texture(goBackTexture,goBack.x+goBack.width + 5,goBack.y,200,goBack.height);
    };

    ~Academics()
    {
        SDL_DestroyTexture(sText);
        SDL_DestroyTexture(eText);
        SDL_DestroyTexture(aText);
    }

    Academics()
    {
        scroll = 0.0f;
        finished = false;
        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);

        SDL_Color hatchBlue = {1,91,144,100};
        goBackTexture = render_text("Back",berbas,hatchBlue);

        sText = render_text("Plan  to  study  at:",berbas,hatchBlue);
        eText = render_text("Program:  Software  Engineering",berbas,hatchBlue);
        aText = render_text("Academic  Awards:",berbas,hatchBlue);

        velocity = 150.0f;
        height = 3000.0f;

    };
};

class About : public Process
{
public:
    ProcessButton goBack;

    Process *goBackProcess;
    SDL_Texture *goBackTexture;

    SDL_Texture *aTexture;

    void init()
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        this->goBackProcess = new IntroAnimation
        (
         vec2d(44.0f,-200.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
        );

        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);
    };

    void handle_events(SDL_Event *event)
    {
        if(!finished)
            finished = goBack.handle_events(event,&next);
    };

    void draw()
    {

        render_texture(sdlTexture,300,300,179,99);
        render_texture(gccTexture,500,280,109,130);
        render_texture(mingwTexture,620,320,229,60);
        render_texture(cbTexture,890,300,128,128);
        render_texture(cppTexture,100,300,180,97);
        render_texture(aTexture,130,80,900,150);

        goBack.draw();
        render_texture(goBackTexture,goBack.x+goBack.width + 5,goBack.y,200,goBack.height);
    };

    ~About()
    {
        SDL_DestroyTexture(aTexture);
    }

    About()
    {
        finished = false;
        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);

        SDL_Color hatchBlue = {1,91,144,100};
        goBackTexture = render_text("Back",berbas,hatchBlue);
        aTexture = render_text("This  program  was  made  with:",berbas,hatchBlue);

    };

};

class Mandelbrot : public Process
{
public:

    ProcessButton goBack;

    Process *goBackProcess;
    SDL_Texture *goBackTexture;

    SDL_Texture *mandelTexture;

    SDL_Surface *surface;

    int frames;
    int maxIt;

    void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
    {
        int bpp = surface->format->BytesPerPixel;
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
        *(Uint32 *)p = pixel;
    }

    void init()
    {
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        this->goBackProcess = new IntroAnimation
        (
         vec2d(44.0f,-200.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
        );

        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);
    };

    void handle_events(SDL_Event *event)
    {
        if(!finished)
            finished = goBack.handle_events(event,&next);
    };

    void update(float dt)
    {
        if(frames >= 3000 && maxIt <= 20)
        {
            maxIt++;

            for(int i = 0; i < 1024; ++i)
            {
                for(int j = 0; j < 1024; ++j)
                {
                    float x0 = ((float)i-512)/((float)1024/4.0f);
                    float y0 = ((float)j-512)/((float)1024/4.0f);

                    float x = 0;
                    float y = 0;

                    int it = -1;
                    while(it <= maxIt && x*x + y*y <= 4.0f)
                    {
                        it++;
                        float xTemp = x*x - y*y + x0;
                        y = 2*x*y + y0;
                        x = xTemp;
                    }

                    put_pixel(surface,i,j,SDL_MapRGB(surface->format,it*200,it*100,it*50));

                }
            }

            SDL_DestroyTexture(mandelTexture);
            mandelTexture = SDL_CreateTextureFromSurface(renderer,surface);

            frames = 0;
        }
    };

    void draw()
    {
        frames++;
        render_texture(mandelTexture,128,0,1024,1024);
        goBack.draw();
        render_texture(goBackTexture,goBack.x+goBack.width + 5,goBack.y,200,goBack.height);
    };

    ~Mandelbrot()
    {
        SDL_DestroyTexture(mandelTexture);
        SDL_FreeSurface(surface);

    }

    Mandelbrot()
    {
        finished = false;
        this->goBack = ProcessButton(goBackProcess,buttonOut,buttonIn,SCREEN_WIDTH-400,SCREEN_HEIGHT-150,128,128);

        SDL_Color hatchBlue = {1,91,144,100};
        goBackTexture = render_text("Back",berbas,hatchBlue);

        surface = SDL_CreateRGBSurface(0,1024,1024,32,0,0,0,0);

        mandelTexture = SDL_CreateTextureFromSurface(renderer,surface);

        frames = 4000;
        maxIt = -1;

    };

};

//------------------ Constructors  -------------------------

IntroAnimation::IntroAnimation
(
        vec2d hatchPosition,
        vec2d hatchSize,
        float hatchEndHeight,
        SDL_Texture *hatchTexture,

        SDL_Texture *buttonOut,
        SDL_Texture *buttonIn,

        TTF_Font *font
)
    {
        this->next = NULL;
        finished = false;
        this->hatchPosition = hatchPosition;
        this->hatchSize = hatchSize;
        this->hatchEndHeight = hatchEndHeight;

        this->font = font;

        SDL_Color hatchBlue = {1,91,144,100};

        geraldHatch = render_text("Dr. Gerald G. Hatch Scholarship",font,hatchBlue);

        aboutMeTexture = render_text("About Me",berbas,hatchBlue);
        interestsTexture = render_text("Interests",berbas,hatchBlue);
        academicsTexture = render_text("Academics",berbas,hatchBlue);
        exitTexture = render_text("Exit",berbas,hatchBlue);
        aboutTexture = render_text("About",berbas,hatchBlue);
        mandelbrotTexture = render_text("Mandelbrot",berbas,hatchBlue);

    }

void IntroAnimation::init()
{
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    this->interestsProcess = new Interests();
    this->academicsProcess = new Academics();
    this->aboutMeProcess = new AboutMe(kaiTexture,vec2d(10,10),vec2d(240,320),buttonOut,buttonIn,berbas);
    this->aboutProcess = new About();
    this->mandelbrotProcess = new Mandelbrot;

    this->aboutMe = ProcessButton(aboutMeProcess,buttonOut,buttonIn,10,370,128,128);
    this->interests = ProcessButton(interestsProcess,buttonOut,buttonIn,436,370,128,128);
    this->academics = ProcessButton(academicsProcess,buttonOut,buttonIn,10,706,128,128);
    this->exit = ProcessButton(NULL,buttonOut,buttonIn,862,706,128,128);
    this->about = ProcessButton(aboutProcess,buttonOut,buttonIn,862,370,128,128);
    this->mandelbrot = ProcessButton(mandelbrotProcess,buttonOut,buttonIn,436,706,128,128);
}

int main(int argc, char *argv[])
{
    if(!init())
    {
        return -1;
    }

    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    hatchTexture = load_texture("assets/hatch_logo.png");
    buttonOut = load_texture("assets/button_out.png");
    buttonIn = load_texture("assets/button_in.png");
    kaiTexture = load_texture("assets/terriblePhoto.jpg");
    alexTexture = load_texture("assets/Alexander_The_Great.jpg");
    napoleonTexture = load_texture("assets/Napoleon_Alps.jpg");
    codeTexture = load_texture("assets/code.png");
    aluTexture = load_texture("assets/ALU.png");
    cpuTexture = load_texture("assets/cpu.jpg");
    higgsTexture = load_texture("assets/higgs.jpg");
    nuclearTexture = load_texture("assets/reactor.jpg");
    waterlooTexture = load_texture("assets/waterloo.png");
    queenTexture = load_texture("assets/queen.png");
    awardsTexture = load_texture("assets/awards.png");
    sdlTexture = load_texture("assets/sdl.png");
    gccTexture = load_texture("assets/gcc.gif");
    cbTexture = load_texture("assets/codeblocks.png");
    mingwTexture = load_texture("assets/mingw.png");
    cppTexture = load_texture("assets/cpp.jpg");

    berbas = TTF_OpenFont("assets/berbas.ttf",96);


    float dt = 0;
    float prevTime = 0.0f;


    Process *process = NULL;
    process = new IntroAnimation
    (
         vec2d(44.0f,-1000.0f),
         vec2d(1024,128),
         10,
         hatchTexture,
         buttonOut,
         buttonIn,
         berbas
     );

    process->init();

    int frames = 0;

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

        frames++;

        if(frames >= 70)
        {
            SDL_RenderClear(renderer);

            process->draw();

            SDL_RenderPresent(renderer);

            frames = 0;
        }

    }

    TTF_CloseFont(berbas);

    SDL_DestroyTexture(buttonIn);
    SDL_DestroyTexture(buttonOut);
    SDL_DestroyTexture(hatchTexture);
    SDL_DestroyTexture(kaiTexture);
    SDL_DestroyTexture(buttonIn);
    SDL_DestroyTexture(buttonOut);
    SDL_DestroyTexture(alexTexture);
    SDL_DestroyTexture(napoleonTexture);
    SDL_DestroyTexture(codeTexture);
    SDL_DestroyTexture(aluTexture);
    SDL_DestroyTexture(cpuTexture);
    SDL_DestroyTexture(higgsTexture);
    SDL_DestroyTexture(nuclearTexture);
    SDL_DestroyTexture(waterlooTexture);
    SDL_DestroyTexture(queenTexture);
    SDL_DestroyTexture(awardsTexture);
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyTexture(gccTexture);
    SDL_DestroyTexture(cbTexture);
    SDL_DestroyTexture(mingwTexture);
    SDL_DestroyTexture(cppTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
