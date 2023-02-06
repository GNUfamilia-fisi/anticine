#ifndef GRANDPARENT_H
#define GRANDPARENT_H

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <windows.h>
#include "json.hpp"

using json = nlohmann::json;


typedef struct {
    int x;
    int y;
} consoleSize;


void gotoXY(int x, int y);

json fetch(std::string f);

std::string exec(const char* cmd);

unsigned int get_term_width();

consoleSize getConsoleRectSize();

void cleanLine();


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


#endif