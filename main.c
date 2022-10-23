#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

//
// TODO: The head is already the right colour or 144, 238, 144. The neck requires a darker green colour and the spacing needs to be modified back to -20 also requires a second random generator also requires modication to the arry on both spawn and move functions *tip explicitly say app.srain[i][1].
//       The tail is as is however this will require it to have the spacing to be -1000 or -500 and requires the modifcation to the array setup both spawn and move functions *tip explicitly say app.srain[i][2].
//       int r = (rand() % (max - min + 1)) + min; is a number generator with range

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1
#define RAIN_WIDTH      20
#define RAIN_HEIGHT     20
#define RAIN_START_Y    0
#define RAINDROP_COUNT  2048
#define SpawnFrame      0.01f
#define MoveFrame       0.075f
#define RFrame          0.075f
#define Increment       2 //tail effect remember to also modify incrementmax which is n-1 due to array null terminator
#define IncrementMax    3 //increment - 1

void initialize(void);
void terminate(int exit_code);
void handle_input(void);

int spawn_rain(int);
int move_rain(int);

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    int running;
    SDL_Rect srain[RAINDROP_COUNT][IncrementMax];
    int dx;
    int dy;
    int fps;
} SDL2APP;

// initialize global structure to store app state
// and SDL renderer for use in all functions
SDL2APP app = {
  .running = 1,
  .srain = {0},
  .dx = 0,
  .dy = RAIN_HEIGHT,
  .fps = 0,
};

SDL_Rect srain = {
    .w = 0,
    .h = 0,
    .x = 0,
    .y = 0
};

SDL_DisplayMode DM = {
    .w = 0,
    .h = 0
};

//magic number table for the x coordinator used in random spawning prevents weird overlaps
const int mn[172] = {
    0,
    20,
    40,
    60,
    80,
    100,
    120,
    140,
    160,
    180,
    200,
    220,
    240,
    260,
    280,
    300,
    320,
    340,
    360,
    380,
    400,
    420,
    440,
    460,
    480,
    500,
    520,
    540,
    560,
    580,
    600,
    620,
    640,
    660,
    680,
    700,
    720,
    740,
    760,
    780,
    800,
    820,
    840,
    860,
    880,
    900,
    920,
    940,
    960,
    980,
    1000,
    1020,
    1040,
    1060,
    1080,
    1100,
    1120,
    1140,
    1160,
    1180,
    1200,
    1220,
    1240,
    1260,
    1280,
    1300,
    1320,
    1340,
    1360,
    1380,
    1400,
    1420,
    1440,
    1460,
    1480,
    1500,
    1520,
    1540,
    1560,
    1580,
    1600,
    1620,
    1640,
    1660,
    1680,
    1700,
    1720,
    1740,
    1760,
    1780,
    1800,
    1820,
    1840,
    1860,
    1880,
    1900,
    1920,
    1940,
    1960,
    1980,
    2000,
    2020,
    2040,
    2060,
    2080,
    2100,
    2120,
    2140,
    2160,
    2180,
    2200,
    2220,
    2240,
    2260,
    2280,
    2300,
    2320,
    2340,
    2360,
    2380,
    2400,
    2420,
    2440,
    2460,
    2480,
    2500,
    2520,
    2540,
    2560,
    2580,
    2600,
    2620,
    2640,
    2660,
    2680,
    2700,
    2720,
    2740,
    2760,
    2780,
    2800,
    2820,
    2840,
    2860,
    2880,
    2900,
    2920,
    2940,
    2960,
    2980,
    3000,
    3020,
    3040,
    3060,
    3080,
    3100,
    3120,
    3140,
    3160,
    3180,
    3200,
    3220,
    3240,
    3260,
    3280,
    3300,
    3320,
    3340,
    3360,
    3380,
    3400,
    3420,
};

const char* alphabet[62] = {
    "0", //0
    "1", //1
    "2", //2
    "3", //3
    "4", //4
    "5", //5
    "6", //6
    "7", //7
    "8", //8
    "9", //9
    "A", //10
    "B", //11
    "C", //12
    "D", //13
    "E", //14
    "F", //15
    "G", //16
    "H", //17
    "I", //18
    "J", //19
    "K", //20
    "L", //21
    "M", //22
    "N", //23
    "O", //24
    "P", //25
    "Q", //26
    "R", //27
    "S", //28
    "T", //29
    "U", //30
    "V", //31
    "W", //32
    "X", //33
    "Y", //34
    "Z", //35
    "a", //36
    "b", //37
    "c", //38
    "d", //39
    "e", //40
    "f", //41
    "g", //42
    "h", //43
    "i", //44
    "j", //45
    "k", //46
    "l", //47
    "m", //48
    "n", //49
    "o", //50
    "p", //51
    "q", //52
    "r", //53
    "s", //54
    "t", //55
    "u", //56
    "v", //57
    "w", //58
    "x", //59
    "y", //60
    "z", //61
};

Mix_Music* music = NULL;
TTF_Font* font1 = NULL;
TTF_Font* font2 = NULL;
TTF_Font* font3 = NULL;
SDL_Texture* text1 = NULL;
SDL_Surface* surface1 = NULL;
SDL_Texture* text2 = NULL;
SDL_Surface* surface2 = NULL;

int main(int argc, char* argv[])
{
    // Initialize SDL and the relevant structures
    initialize();

    Mix_PlayingMusic();

    float FrameTime1 = 0, FrameTime2 = 0, FrameTimeR = 0;

    int PreviousTime = 0;
    int CurrentTime = 0;
    float DeltaTime = 0;

    int i = 0;

    font1 = TTF_OpenFont("matrix.ttf", 24);

    int rval1 = 0;
    int rc1 = 0;
    int rc2 = 0;
    int rc3 = 0;


//    SDL_Color foreground1 = { 0, 0, 0, 255 };
    SDL_Color background1 = { 0, 0, 0, 0 };
    SDL_Color foreground2 = { 0, 0, 0, 0 };
    SDL_Color background2 = { 0, 0, 0, 0 };

    // enter app loop
    while (app.running) {

        PreviousTime = CurrentTime;
        CurrentTime = SDL_GetTicks();
        DeltaTime = (CurrentTime - PreviousTime) / 1000.0f;

        Uint64 start = SDL_GetPerformanceCounter();

        handle_input();

        FrameTime1 += DeltaTime;
        FrameTime2 += DeltaTime;
        FrameTimeR += DeltaTime;

        // clear the screen with all black before drawing anything 
//        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
//        SDL_RenderClear(app.renderer);

//        SDL_Color foreground1 = { 144, 238, 144, 255 };
//        SDL_Color background1 = { 0,0,0,0 };
//        SDL_Color foreground2 = { 0, 128, 0, 255 };
//        SDL_Color background2 = { 0,0,0,0 };

        //This is here because the value needs to change and gets updated in the main loop
        SDL_Color foreground1 = { rc1, 0, 0, 255 };

        surface1 = TTF_RenderText_Shaded(font1, alphabet[rval1], foreground1, background1);
        text1 = SDL_CreateTextureFromSurface(app.renderer, surface1);

        surface2 = TTF_RenderText_Shaded(font1, " ", foreground2, background2);
        text2 = SDL_CreateTextureFromSurface(app.renderer, surface2);

        if (FrameTimeR > RFrame)
        {
            rval1 = rand() % 61;
            rc1 = (rand() % (255 - 64 + 1)) + 64;//used for red colour random (rand() % (max - min + 1)) + min;
            rc2 = (rand() % (255 - 0 + 1)) + 0;//used for green colour random (rand() % (max - min + 1)) + min;
            rc3 = (rand() % (255 - 0 + 1)) + 0;//used for blue colour random (rand() % (max - min + 1)) + min;
            FrameTimeR = 0;
        }

        if (FrameTime1 > SpawnFrame)
        {
            if (i == RAINDROP_COUNT)
            {
                i = 1;
            }

            spawn_rain(i);
 
            i++;
            FrameTime1 = 0;
        }

        if (FrameTime2 >= MoveFrame)
        {
            for (int x = 1; x < RAINDROP_COUNT; ++x)
            {
                move_rain(x);
            }

            SDL_RenderPresent(app.renderer);

            Uint64 end = SDL_GetPerformanceCounter();

            float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();

            app.fps = (int)(1 / elapsed);

            char buffer[64];
            snprintf(buffer, 64, "Matrix-Code FPS:%d", app.fps);
            SDL_SetWindowTitle(app.window, buffer);

            FrameTime2 = 0;
        }

        SDL_DestroyTexture(text1);
        SDL_FreeSurface(surface1);
        SDL_DestroyTexture(text2);
        SDL_FreeSurface(surface2);
    }
    // make sure program cleans up on exit
    terminate(EXIT_SUCCESS);
}

void initialize()
{

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {

        printf("error: failed to initialize SDL: %s\n", SDL_GetError());
        terminate(EXIT_FAILURE);

    }

    SDL_GetCurrentDisplayMode(0, &DM);
    int SCREEN_WIDTH = DM.w;
    int SCREEN_HEIGHT = DM.h;
//    int SCREEN_WIDTH = 640;
//    int SCREEN_HEIGHT = 480;

    Mix_Init(MIX_INIT_MP3);

    // create the app window
    app.window = SDL_CreateWindow("Matrix-Code",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN //| SDL_WINDOW_MAXIMIZED
    );

    SDL_SetWindowFullscreen(app.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_ShowCursor(SDL_DISABLE);

    if (!app.window)
    {

        printf("error: failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        terminate(EXIT_FAILURE);

    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    app.renderer = SDL_CreateRenderer(app.window, 0, SDL_RENDERER_ACCELERATED);

    if (!app.renderer)
    {

        printf("error: failed to create renderer: %s\n", SDL_GetError());
        terminate(EXIT_FAILURE);

    }

    if (TTF_Init() < 0) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
    }

    Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(-1);
    Mix_VolumeMusic(25);
    music = Mix_LoadMUS("music.mp3");
    Mix_PlayMusic(music, -1);
}

void terminate(int exit_code)
{

    if (app.renderer)
    {
        SDL_DestroyRenderer(app.renderer);
    }

    if (app.window)
    {
        SDL_DestroyWindow(app.window);
    }

    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();

    TTF_CloseFont(font1);
    TTF_CloseFont(font2);
    TTF_Quit();

    SDL_Quit();
    exit(exit_code);
}

void handle_input()
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
        {
            app.running = 0;
        }
    }
}

int spawn_rain(int i)
{
    int RAIN_START_X = rand() % 172;

    app.srain[i][0].x = mn[RAIN_START_X];
    app.srain[i][0].y = RAIN_START_Y;

    SDL_QueryTexture(text1, NULL, NULL, &app.srain[i][0].w, &app.srain[i][0].h);

    for (int t = 0; t < Increment; t++)
    {
        app.srain[i][t].x = app.srain[i][0].x;
        if (t == 0)
        {
            app.srain[i][t].y = app.srain[i][t].y - 20;
        }
        else if (t == 1)
        {
            app.srain[i][t].y = app.srain[i][t - 1].y - DM.h;
        }

        SDL_QueryTexture(text2, NULL, NULL, &app.srain[i][t].w, &app.srain[i][t].h);
    }
    return i;
}

/*Original Code
int spawn_rain(int i)
{
    int RAIN_START_X = rand() % 172;

    app.srain[i][0].x = mn[RAIN_START_X];
    app.srain[i][0].y = RAIN_START_Y;

    SDL_QueryTexture(text1, NULL, NULL, &app.srain[i][0].w, &app.srain[i][0].h);

    for (int t = 0; t < Increment; t++)
    {
        app.srain[i][t].x = app.srain[i][0].x;
        if (t == 0)
        {
            app.srain[i][t].y = app.srain[i][t].y - 20;
        }
        else if (t == 1)
        {
            app.srain[i][t].y = app.srain[i][t - 1].y - 2000;
        }

        SDL_QueryTexture(text2, NULL, NULL, &app.srain[i][t].w, &app.srain[i][t].h);
    }
    return i;
}
*/

int move_rain(int i)
{
    app.srain[i][0].y = app.srain[i][0].y + app.dy;

    SDL_RenderCopy(app.renderer, text1, NULL, &app.srain[i][0]);

    for (int t = 1; t < Increment; t++)
    {
        app.srain[i][t].y = app.srain[i][t].y + app.dy;

        SDL_RenderCopy(app.renderer, text2, NULL, &app.srain[i][t]);
    }
    return i;
}

/*Original Code
int move_rain(int i)
{
//    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
//    SDL_SetRenderDrawColor(app.renderer, 255,255,255,255);
//    SDL_RenderFillRect(app.renderer, &app.srain[i][0]);

    app.srain[i][0].y = app.srain[i][0].y + app.dy;

    SDL_RenderCopy(app.renderer, text1, NULL, &app.srain[i][0]);

    for (int t = 1; t < Increment; t++)
    {
        app.srain[i][t].y = app.srain[i][t].y + app.dy;

        SDL_RenderCopy(app.renderer, text2, NULL, &app.srain[i][t]);
    }
    return i;
}
*/