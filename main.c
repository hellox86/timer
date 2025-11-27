#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <pthread.h>

#define SCR_W 800
#define SCR_H 600

#define INTERVAL 1000 // interval timer(ms)

#ifdef ERR
    #undef ERR
#endif

#define ERR(s, er_code) {fprintf(stderr, "%s\n", s); exit(er_code);}

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

TTF_Font* font = NULL;
SDL_Surface* text_surf = NULL;
bool is_running = true;
void* initialize_sdl(void* arg)
{
    int rend_flags = SDL_RENDERER_ACCELERATED;
    int win_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    
    char err_string[250];
	
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
	snprintf(err_string, sizeof(err_string), "[SDL_ERROR]: error init SDL3: %s", SDL_GetError());
	ERR(err_string, 1);
	
    }
    if((TTF_Init()) < 0)
    {
	snprintf(err_string, sizeof(err_string), "[SDL_ttf ERROR]: error init TTF: %s", TTF_GetError());
	ERR(err_string, 1);	
    }
    window = SDL_CreateWindow("timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_W, SCR_H, win_flags);
    if(!window)
    {
	snprintf(err_string, sizeof(err_string), "[SDL_ERROR]: error at creation window: %s", SDL_GetError());
	ERR(err_string, 1);
    }
    renderer = SDL_CreateRenderer(window, -1, rend_flags);
    if(!renderer)
    {
	snprintf(err_string, sizeof(err_string), "[SDL_ERROR]: error at creation renderer: %s", SDL_GetError());
	ERR(err_string, 1);
	
    }
    return NULL;
}
void* detect_event(void* arg)
{
    SDL_Event e;
    bool event_detected;
    while (SDL_PollEvent(&e))
    {
	switch(e.type)
	{
	    case SDL_QUIT:
		is_running = false;
		event_detected = true;
		break;
	    case SDL_KEYDOWN:
		switch(e.key.keysym.sym)
		{
		    case SDLK_q : case SDLK_ESCAPE :
			is_running = false;
			event_detected = true;

			break;
		}
		break;
	    }
	    
	    if(event_detected) break;
	}
	return NULL;
}
void end_of_sdl()
{
    if (renderer)
    {
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
    }
    if (window)
    {
	SDL_DestroyWindow(window);
	window = NULL;
    }
    SDL_Quit();
    
}
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;
unsigned char days = 0;

char seconds_string[4] = "0%d";
char minutes_string[4] = "0%d";
char hours_string[4] = "0%d";

char time1[20] = "00:00:00";

void sigint_handler(void)
{
    printf("Goodbye!\n");
    SDL_Quit();
    end_of_sdl();
   
    TTF_CloseFont(font);
    TTF_Quit();
    exit(0);
}
void check_time(void)
{       
    if (seconds == 60)
    {
	seconds = 0;
	++minutes;
    }
    
    if(minutes == 60)
    {
	minutes = 0;
	++hours;
    }
    if (hours == 24)
    {
	hours = 0;
	++days;
    }
    
    snprintf(hours_string, sizeof(hours_string), (hours >= 10) ? "%d" : "0%d", hours);
    snprintf(minutes_string, sizeof(minutes_string), (minutes >= 10) ? "%d" : "0%d", minutes);
    snprintf(seconds_string, sizeof(seconds_string), (seconds >= 10) ? "%d" : "0%d", seconds);
}

int main(int argc, char *argv[])
{

    struct itimerval timer1;
    if (signal(SIGALRM, (void(*)(int))check_time) == SIG_ERR)
    {
	ERR("failed to catch SIGALRM", 1);
    }
    
    signal(SIGINT, (void(*)(int))sigint_handler);
    
    timer1.it_value.tv_sec = INTERVAL/1000;
    timer1.it_value.tv_usec = (INTERVAL*1000)%1000000;

    timer1.it_interval = timer1.it_value;

    if (setitimer(ITIMER_REAL, &timer1, NULL) == -1)
    {
	ERR("error calling timer", 1);
    }
    pthread_t sdlinit_thread;
    pthread_create(&sdlinit_thread, NULL, initialize_sdl, "initialize sdl...");
    
    pthread_join(sdlinit_thread, NULL);
    font = TTF_OpenFont("./IosevkaNerdFont-Bold.ttf", 10);
    
    SDL_Color foreground = {255, 255, 255};
   
    SDL_Texture* text_t;
    SDL_Rect text_rect;
    text_rect.x = SCR_W/2;
    text_rect.y = SCR_H/2;
    text_rect.w = SCR_W/2;
    text_rect.h = SCR_H/2;
    pthread_t event_thread;
   
    while(is_running)
    {
	pthread_create(&event_thread, NULL, detect_event, "detect event(thread) running");
	pthread_join(event_thread, NULL);
	text_surf = TTF_RenderText_Solid(font, time1, foreground); 
	text_t = SDL_CreateTextureFromSurface(renderer, text_surf);
	
	SDL_RenderCopy(renderer, text_t, NULL, &text_rect);
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	SDL_DestroyTexture(text_t);

	pause();
	++seconds;
	snprintf(time1, sizeof(time1), "%s:%s:%s", hours_string, minutes_string, seconds_string);
	
    }
    end_of_sdl();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
