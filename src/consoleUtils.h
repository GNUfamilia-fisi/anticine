#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdio>
#if defined(_WIN32)
#   include <windows.h>
#   include <conio.h>
#   define POPEN _popen
#   define PCLOSE _pclose
#   define ANTICINE_WINDOWS
#elif defined(__unix__)
#   include <termios.h>
#   include <sys/ioctl.h>
// #   include <unistd.h>
#   include <poll.h>
#   define POPEN popen
#   define PCLOSE pclose
#   define ANTICINE_UNIX
#else
#   error "Unsupported platform"
#endif

#include <json.hpp>
#include <utf8.hpp>

using json = nlohmann::json;

namespace gnu {

#if defined(ANTICINE_WINDOWS)
    // Variable global para el handle de la consola proveída por Windows.h
    // Esta variable no debería ser usada directamente, sino a través de funciones
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    struct termios initial_state;
    struct termios term_state;
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
vec2d getCursorPosition();

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
    Up = 4283163,
    Down = 4348699,
    Left = 4479771,
    Right = 4414235,

    a = 97,
    w = 119,
    s = 115,
    d = 100,

    ExitKey = 27,
    Space = 32,
    Enter = 10
#endif
};

#include <fcntl.h>

int getch() {
#if defined(ANTICINE_WINDOWS)
    if (_kbhit()) {
        return _getch();
    }
    return 0;
#else
    int wbuff = 0;

    struct pollfd pollstdin[1];
    pollstdin[0].fd = STDIN_FILENO;
    pollstdin[0].events = POLLIN;

    // Poll one file descriptor (stdin) and waits 1 ms for the POLLIN event
    // https://man7.org/linux/man-pages/man2/poll.2.html
    poll(&pollstdin[0], 1, 500);

    if (pollstdin[0].revents & POLLIN) {
        read(STDIN_FILENO, &wbuff, 4);

        // Ctrl+C
        if (wbuff == 3) {
            raise(SIGTERM);
        }
        // Ctrl+Z
        if (wbuff == 26) {
            raise(SIGTSTP);
        }
        return wbuff;
    }
    return 0;
#endif
}

// gotoXY: Posiciona el cursor en { x, y } de la consola ("y" invertida)
// He hecho tres definiciones de gotoXY, una que usa short, otra que usa int
// y la última que utiliza los recomendados gnu::vect2d
void gotoXY(int x, int y) {
#if defined(ANTICINE_UNIX)
    gnu::print("\x1b[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H");
#else
    COORD cursorPosition = { (short)x, (short)y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoXY(gnu::vec2d pos) {
#if defined(ANTICINE_UNIX)
    gnu::print("\x1b[" + std::to_string(pos.y + 1) + ";" + std::to_string(pos.x + 1) + "H");
#else
    COORD cursorPosition = { pos.x, pos.y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoX(short x) {
#if defined(ANTICINE_UNIX)
    gnu::print("\x1b[" + std::to_string(gnu::getCursorPosition().y + 1) + ";" + std::to_string(x + 1) + "H");
#else
    COORD cursorPosition = { x, gnu::getCursorPosition().y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}
void gotoY(short y) {
#if defined(ANTICINE_UNIX)
    gnu::print(
        "\x1b[" + std::to_string(y + 1) + ";" + std::to_string(gnu::getCursorPosition().x + 1) + "H"
    );
#else
    COORD cursorPosition = { gnu::getCursorPosition().x, y };
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
#if defined(ANTICINE_UNIX)
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return { w.ws_col, w.ws_row };
#else
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &BufferInfo);
    coords = { BufferInfo.dwSize.X, BufferInfo.dwSize.Y };
#endif
    return coords;
}
gnu::vec2d getCursorPosition() {
    gnu::vec2d coords;
#if defined(ANTICINE_UNIX)
    write(STDOUT_FILENO, "\033[6n", 4);
    // returns ESC[(y);(x)R

    char buff[32] = {0};
    unsigned long i = 0;
    char ch = 0;

    for (;ch != 'R' && i < sizeof(buff) - 1; i++) {
        read(STDIN_FILENO, &ch, 1);
        buff[i] = ch;
    }
    buff[i] = '\0';

    sscanf(buff, "\033[%d;%dR", &coords.y, &coords.x);
    coords.y--; coords.x--;
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

void cleanupAnticine(void) {
    gnu::setCursorVisible(true);
#ifdef _WIN32
#else
    //clean up the alternate buffer
	printf("\x1b[2J");
	//switch back to the normal buffer
	printf("\x1b[?1049l");
	//show the cursor again
	gnu::setCursorVisible(true);

    tcsetattr(STDOUT_FILENO, TCSANOW, &initial_state);
#endif
}

void exit_with_code(int code) {
    exit(code);
}

// Inicia el programa con algunas configuraciones básicas
void initProgramConf() {
    gnu::setCursorVisible(false);
#if defined(ANTICINE_UNIX)
    tcgetattr(STDOUT_FILENO, &initial_state);
    tcgetattr(STDOUT_FILENO, &term_state);

    // cfmakeraw(&term_state);
    term_state.c_lflag &= ~(ICANON);
    term_state.c_lflag &= ~(ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &term_state);

    // Start the alternative buffer
    printf("\x1b[?1049h");
    // Clenaup
    printf("\033[2J");
    // Hide the cursor

    // Goto 0, 0 (y, x)
    printf("\x1b[0;0H");
    fflush(stdout);

    // Setup cleanups
    atexit(cleanupAnticine);
    signal(SIGINT, exit_with_code);
    signal(SIGTERM, exit_with_code);
    signal(SIGKILL, exit_with_code); // user responsibility?
    signal(SIGTSTP, exit_with_code); // Ctrl+Z
#else
    // Setea la consola para que acepte UTF-8
    SetConsoleOutputCP(65001);
#endif
}

// Modifica la visibilidad del cursor
void setCursorVisible(bool isVisible) {
#if defined(ANTICINE_UNIX)
    gnu::print("\x1b[?25" + std::string(isVisible ? "h" : "l"));
#else
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = isVisible;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
#endif
}

// Pausa el proceso por una cierta cantidad de milisegundos
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}

void cls() {
#if defined(ANTICINE_UNIX)
    // system("clear");
    printf("\033[2J");
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
#if defined(ANTICINE_UNIX)
    printf("%c", ch);
    fflush(stdout);
#else
    WriteConsole(consoleHandle, &ch, 1, NULL, NULL);
#endif
}
void print(std::string str) {
#if defined(ANTICINE_UNIX)
    printf("%s", str.c_str());
    fflush(stdout);
#else
    WriteConsole(consoleHandle, str.c_str(), str.length(), NULL, NULL);
#endif
}
void print(char* str) {
#if defined(ANTICINE_UNIX)
    printf("%s", str);
    fflush(stdout);
#else
    WriteConsole(consoleHandle, str, strlen(str), NULL, NULL);
#endif
}
void print(const char* str) {
#if defined(ANTICINE_UNIX)
    printf("%s", str);
    fflush(stdout);
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

    int offset = (gnu::getConsoleSize().x - utf8::str_length(subStringsList[0])) / 2;

    for (size_t i = 0; i < subStringsList.size(); i++){
        gnu::gotoX(offset);
        gnu::print(subStringsList[i] + "\n");
    }
}

} // namespace gnu
