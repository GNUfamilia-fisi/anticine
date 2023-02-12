#pragma once

#include <json.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <cstdio>

#include <utf8.hpp>

using json = nlohmann::json;

namespace gnu {

// Variable global para el handle de la consola proveída por Windows.h
// Esta variable no debería ser usada directamente, sino a través de funciones
HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

typedef struct {
    int x;
    int y;
} vec2d;

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

// Posiciona el cursor en { x, y } de la consola ("y" invertida)
void gotoXY(short x, short y) {
    COORD cursorPosition = { x, y };
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
void cleanLine(short y) {
    gotoXY(0, y);
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

void setColor(gnu::color color) {
    SetConsoleTextAttribute(consoleHandle, color);
}

// Pausa el proceso por una cierta cantidad de milisegundos
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}

//Prints multiline raw text at the center of the screen (without padding)
void printRawCenter(std::string raw) {
    std::vector<std::string> subStringsList;
    std::string buffer = "";

    for (size_t i = 0; i < raw.length(); i++){
        if (raw[i] != '\n' && i != raw.length() - 1){
            buffer += raw[i];
        }
        else{
            if (buffer.empty()) continue;
            if (i == raw.length() - 1) buffer += raw[i];
            subStringsList.push_back(buffer);
            buffer = "";
        }
    }

    int padding = (getConsoleSize().x / 2) - (utf8::str_length(subStringsList[0]) / 2);

    for (size_t i = 0; i < subStringsList.size(); i++){
        for (int j = 0; j < padding; j++){
            std::cout << " ";
        }
        std::cout << subStringsList[i] << std::endl;
    }
}

} // namespace gnu
