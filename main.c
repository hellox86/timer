#include <stdio.h>
#include <signal.h>
#include <ncurses.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    initscr();
    noecho();
    printw("Hello, World");
   
    refresh();
    sleep(5);
    endwin();
    return 0;
}
