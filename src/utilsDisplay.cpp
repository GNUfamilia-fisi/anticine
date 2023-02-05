#include <windows.h>

COORD pos;
HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);


void gotoXY(int x, int y){
    pos = {x,y};
    SetConsoleCursorPosition(consoleWinHandle, pos);
}