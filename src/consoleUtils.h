#pragma once

#include <json.hpp>
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <cstdio>

using json = nlohmann::json;

namespace gnu {
    // Variable global para el handle de la consola
    // proveída por Windows.h
    extern HANDLE consoleHandle;

    typedef struct {
        int x;
        int y;
    } vec2d;

    void gotoXY(short x, short y);
    std::string exec(std::string command);
    unsigned int get_term_width();
    vec2d getConsoleSize();
    void sleep(int ms);

    void cleanLine();
    void setCursorVisible(bool isVisible);

    enum color : unsigned short {
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
    enum key {
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
} // namespace gnu
