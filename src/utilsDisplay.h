#pragma once

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <ctime>
#include <numeric>
#include "json.hpp"

using json = nlohmann::json;


typedef struct {
    int x;
    int y;
} consoleSize;

void gotoXY(short x, short y);
json fetch(std::string url);
std::string exec(const char* cmd);
unsigned int get_term_width();
consoleSize getConsoleRectSize();

void cleanLine();
void ShowConsoleCursor(bool showFlag);
void logoDisplay3D(DWORD color);
void logoDisplay2D(DWORD color);
void cargandoDisplay();

enum ForegroundColors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHTGRAY = 7,
    GRAY = 8,
    LIGHTBLUE = 9,
    LIGHTGREEN = 10,
    LIGHTCYAN = 11,
    LIGHTRED = 12,
    LIGHTMAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

// códigos de teclas para _getch()
enum KeyCode {
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77,

    a = 97,
    w = 119,
    s = 115,
    d = 100,

    ExitKey = 27,
    Space = 32,
    Enter = 13
};
