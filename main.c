#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <raylib.h>
#include <pthread.h>

#define INTERVAL 1000 // interval timer(ms)

#ifdef ERR
    #undef ERR
#endif

#define ERR(s, er_code) {fprintf(stderr, "%s\n", s); exit(er_code);}

unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;
unsigned char days = 0;

unsigned int scr_w = 1920;
unsigned int scr_h = 800;

char seconds_string[4] = "0%d";
char minutes_string[4] = "0%d";
char hours_string[4] = "0%d";

char time1[20];

void sigint_handler(void)
{
    printf("Goodbye!\n");
    CloseWindow();
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
    InitWindow(scr_w, scr_h, "timer");
    
    SetTargetFPS(60);

    Font font_iosevka = LoadFontEx("/usr/share/fonts/TTF/Iosevka-ExtraBold.ttf", 50, NULL, 0);
    
    while(!WindowShouldClose())
    {
	pause();
	++seconds;
	
	BeginDrawing();
	ClearBackground(BLACK);
	
	Vector2 mid_pos = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
	snprintf(time1, sizeof(time1), "%s:%s:%s", hours_string, minutes_string, seconds_string);
	
	DrawTextEx(font_iosevka, time1, mid_pos, 50, 25, WHITE);
	EndDrawing();	
    }
    CloseWindow();
    return 0;
}
