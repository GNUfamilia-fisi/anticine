#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdio>
#ifdef _WIN32
#   include <windows.h>
#   include <conio.h>
#   define POPEN _popen
#   define PCLOSE _pclose
#else
#   include <ncurses.h>
#   define POPEN popen
#   define PCLOSE pclose
#   define NCURSES_ENABLED
#endif

#include <json.hpp>
#include <utf8.hpp>

using json = nlohmann::json;

namespace gnu {

#ifdef _WIN32
    // Variable global para el handle de la consola proveída por Windows.h
    // Esta variable no debería ser usada directamente, sino a través de funciones
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

// Prototipos
void print(char ch);
void print(char* str);
void print(const char* str);
void print(std::string str);
void setCursorVisible(bool isVisible);

struct vec2d {
    int x;
    int y;
    bool operator == (const vec2d& vec) {
        return (x == vec.x && y == vec.y);
    }
    bool operator != (const vec2d& vec) {
        return !(*this == vec);
    }
    vec2d operator + (const vec2d& vec) {
        return { x + vec.x, y + vec.y };
    }
    vec2d operator - (const vec2d& vec) {
        return { x - vec.x, y - vec.y };
    }
};
vec2d getConsoleSize();
vec2d getCursorPos();

// códigos de teclas para _getch()
enum key {
#ifdef _WIN32
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
#else
    Up = 259,
    Down = 258,
    Left = 260,
    Right = 261,

    a = 'a',
    w = 'w',
    s = 's',
    d = 'd',

    ExitKey = 27,
    Space = ' ',
    Enter = 10
#endif
};

// gotoXY: Posiciona el cursor en { x, y } de la consola ("y" invertida)
// He hecho tres definiciones de gotoXY, una que usa short, otra que usa int
// y la última que utiliza los recomendados gnu::vect2d
void gotoXY(int x, int y) {
#ifdef NCURSES_ENABLED
    move(y, x);
#else
    COORD cursorPosition = { (short)x, (short)y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoXY(gnu::vec2d pos) {
#ifdef NCURSES_ENABLED
    move((int)pos.y, (int)pos.x);
#else
    COORD cursorPosition = { pos.x, pos.y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoX(short x) {
#ifdef NCURSES_ENABLED
    move((int)gnu::getCursorPos().y, (int)x);
#else
    COORD cursorPosition = { x, gnu::getCursorPos().y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoY(short y) {
#ifdef NCURSES_ENABLED
    move((int)y, (int)gnu::getCursorPos().x);
#else
    COORD cursorPosition = { gnu::getCursorPos().x, y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}

// Ejecuta un comando de shell (consola) y devuelve su resultado (stdout)
// como string (texto plano)
std::string exec(std::string command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = POPEN(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        PCLOSE(pipe);
        throw;
    }
    PCLOSE(pipe);
    return result;
}

// Devuelve el tamaño de la consola como vector 2d { x, y }
// El tamaño es en caracteres
gnu::vec2d getConsoleSize() {
    gnu::vec2d coords;
#ifdef NCURSES_ENABLED
    getmaxyx(stdscr, coords.y, coords.x);
#else
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);
    coords = { BufferInfo.dwSize.X, BufferInfo.dwSize.Y };
#endif
    return coords;
}
gnu::vec2d getCursorPos() {
    gnu::vec2d coords;
#ifdef NCURSES_ENABLED
    getyx(stdscr, coords.y, coords.x);
#else
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &BufferInfo);

    coords = { BufferInfo.dwCursorPosition.X, BufferInfo.dwCursorPosition.Y };
#endif
    return coords;
}

// Sobreescribe toda una línea entera de la consola con espacios en blanco
void cleanLine(short y) {
    gnu::gotoXY(0, y);
    for (int i = 0; i < gnu::getConsoleSize().x; i++) {
        gnu::print(" ");
    }
}

// Inicia el programa con algunas configuraciones básicas
void initProgramConf() {
    gnu::setCursorVisible(false);
#ifdef NCURSES_ENABLED
    // setlocale para que ncurses acepte UTF-8
    setlocale(LC_ALL, "");
    if (!initscr()) {
        std::cout << "Fatal error: could not initialize ncurses" << std::endl;
        exit(1);
    }
    // Configuración de ncurses
    use_extended_names(true);
    // oculta el cursor
    curs_set(0);
    // desactiva el buffering de teclado y el delay
    // (permite leer teclas sin presionar enter)
    cbreak();
    nodelay(stdscr, true);
    // desactiva el eco de las teclas
    // (no se ven las teclas presionadas)
    noecho();
    // permite leer un set extendido de teclas
    keypad(stdscr, TRUE);
#else
    // Setea la consola para que acepte UTF-8
    SetConsoleOutputCP(65001);
#endif
}

void endProgram() {
    gnu::setCursorVisible(true);
#ifdef _WIN32
#else
    endwin();
#endif
}

// Modifica la visibilidad del cursor
void setCursorVisible(bool isVisible) {
#ifdef NCURSES_ENABLED
    curs_set(isVisible);
#else
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = isVisible;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
#endif
}

// Pausa el proceso por una cierta cantidad de milisegundos
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}
void cls() {
#ifdef NCURSES_ENABLED
    clear();
#else
    system("cls");
#endif
}

/**
 * Repite un string una cierta cantidad de veces
 * 
 * @example
 * repeat("hola", 3) -> "holaholahola"
*/
std::string repeat(std::string str, int times) {
    std::string result = "";
    for (int i = 0; i < times; i++) {
        result += str;
    }
    return result;
}
std::string repeat(const char* str, int times) {
    std::string result = "";
    for (int i = 0; i < times; i++) {
        result += str;
    }
    return result;
}

// Para imprimir cosas a la velocidad de la luz
void print(char ch) {
#ifdef NCURSES_ENABLED
    printw("%c", ch);
#else
    WriteConsole(consoleHandle, &ch, 1, NULL, NULL);
#endif
}
void print(std::string str) {
#ifdef NCURSES_ENABLED
    printw(str.c_str());
#else
    WriteConsole(consoleHandle, str.c_str(), str.length(), NULL, NULL);
#endif
}
void print(char* str) {
#ifdef NCURSES_ENABLED
    printw(str);
#else
    WriteConsole(consoleHandle, str, strlen(str), NULL, NULL);
#endif
}
void print(const char* str) {
#ifdef NCURSES_ENABLED
    printw(str);
#else
    WriteConsole(consoleHandle, str, strlen(str), NULL, NULL);
#endif
}

// Imprime una línea centrada en la consola
void printLineCentered(std::string line) {
    gnu::gotoX((short)((gnu::getConsoleSize().x - utf8::str_length(line)) / 2));
    gnu::print(line);
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

    int offset = (getConsoleSize().x / 2) - (utf8::str_length(subStringsList[0]) / 2);

    for (size_t i = 0; i < subStringsList.size(); i++){
        gnu::gotoX(offset);
        gnu::print(subStringsList[i] + "\n");
    }
}

} // namespace gnu
