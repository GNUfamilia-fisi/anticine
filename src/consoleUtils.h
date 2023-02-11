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

// Variable global para el handle de la consola proveída por Windows.h
// Esta variable no debería ser usada directamente, sino a través de funciones
HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

struct vec2d {
    short x;
    short y;
    bool operator == (const vec2d& vec) {
        return (x == vec.x && y == vec.y);
    }
    bool operator != (const vec2d& vec) {
        return !(*this == vec);
    }
};

// Para acceder al código de un color escribe
//  gnu::color::BLACK, por ejemplo
// Estos pueden ser usados en funciones como
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

// gotoXY: Posiciona el cursor en { x, y } de la consola ("y" invertida)
// He hecho tres definiciones de gotoXY, una que usa short, otra que usa int
// y la última que utiliza los recomendados gnu::vect2d
void gotoXY(short x, short y) {
    COORD cursorPosition = { x, y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}
void gotoXY(int x, int y) {
    COORD cursorPosition = { (short)x, (short)y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}
void gotoXY(gnu::vec2d pos) {
    COORD cursorPosition = { pos.x, pos.y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}

// Ejecuta un comando de shell (consola) y devuelve su resultado (stdout)
// como string (texto plano)
std::string exec(std::string command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}

// Devuelve el tamaño de la consola como vector 2d { x, y }
// El tamaño es en caracteres
gnu::vec2d getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    gnu::vec2d coords = { BufferInfo.dwSize.X, BufferInfo.dwSize.Y };
    return coords;
}

// Sobreescribe toda una línea entera de la consola con espacios en blanco
void cleanLine() {
    for (int i = 0; i < gnu::getConsoleSize().x; i++) {
        std::cout << " ";
    }
}

// Modifica la visibilidad del cursor
void setCursorVisible(bool isVisible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = isVisible;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void setConsoleColor(gnu::color color) {
    SetConsoleTextAttribute(consoleHandle, color);
}

// Pausa el proceso por una cierta cantidad de milisegundos
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}

} // namespace gnu
