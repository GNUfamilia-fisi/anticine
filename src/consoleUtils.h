#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdio>
#include <csignal>
#include <fstream>
#include <json.hpp>
#include <utf8.hpp>
#if defined(_WIN32)
#   include <windows.h>
#   include <conio.h>
#   define POPEN _popen
#   define PCLOSE _pclose
#   define ANTICINE_WINDOWS
#elif defined(__unix__)
#   include <termios.h>
#   include <sys/ioctl.h> // ioctl
#   include <unistd.h>    // write/read
#   include <poll.h>      // polling
#   define POPEN popen
#   define PCLOSE pclose
#   define ANTICINE_UNIX
#else
#   error "Sistema operativo no soportado"
#endif

using json = nlohmann::json;

namespace gnu {

#if defined(ANTICINE_WINDOWS)
    // Variable global para el handle de la consola proveída por Windows.h
    // Esta variable no debería ser usada directamente, sino a través de funciones
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    // Variables globales para el manejo de la consola en UNIX
    // Guardan el estado inicial y el estado actual de la consola
    // El estado inicial es restaurado al final del programa
    struct termios initial_state;
    struct termios term_state;
#endif

// Prototipos
void print(char ch);
void print(char* str);
void print(const char* str);
void print(std::string str);
void setCursorVisible(bool isVisible);

/* --- Utilidades --- */

// Hacer log a un archivo
// Ejemplo: LOG_FILE("Hola mundo" << 123 << std::endl);
#define LOG_FILE(stream) \
    std::ofstream logfile; \
    logfile.open("debug.log", std::ios::app); \
    logfile << stream; \
    logfile.close();

struct vec2d {
    int x;
    int y;
    bool operator == (const vec2d& vec) { return (x == vec.x && y == vec.y); }
    bool operator != (const vec2d& vec) { return !(*this == vec); }
    vec2d operator + (const vec2d& vec) { return { x + vec.x, y + vec.y }; }
    vec2d operator - (const vec2d& vec) { return { x - vec.x, y - vec.y }; }
};

vec2d getConsoleSize();
vec2d getCursorPosition();

// códigos de teclas para usarse con gnu::getch()
enum key {
#if defined(ANTICINE_WINDOWS)
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77,

    Backspace = 127,   // not sure
    Tab = 9,           // not sure
    Supr = 2117294875, // not sure

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

    Backspace = 127,
    Supr = 2117294875,
    Tab = 9,
    Home = 4741915,
    End = 4610843,

    a = 97,
    w = 119,
    s = 115,
    d = 100,

    ExitKey = 27,
    Space = 32,
    Enter = 10
#endif
};

int getch() {
#if defined(ANTICINE_WINDOWS)
    if (_kbhit()) {
        return _getch();
    }
    return 0;
#else
    int wbuff = 0;

    struct pollfd pollstdin[1];
    pollstdin[0].fd = STDIN_FILENO; // Standard input
    pollstdin[0].events = POLLIN;   // evento: "There is data to read"

    // Hace un poll de 500ms para ver si hay algo en el buffer de entrada
    // Actualmete este buffer no se está leyendo apropiadamente, es por eso
    // que tiene 500ms de timeout. Si tuviese un timeout más corto
    // hay probabilidades de que nunca se procese la entrada
    // https://man7.org/linux/man-pages/man2/poll.2.html
    poll(&pollstdin[0], 1, 500);

    if (pollstdin[0].revents & POLLIN) {
        read(STDIN_FILENO, &wbuff, 4);
        // Ctrl+C
        if (wbuff == 3) raise(SIGTERM);
        // Ctrl+Z
        if (wbuff == 26) raise(SIGTSTP);
        // Entrada cualquiera
        return wbuff;
    }
    return 0;
#endif
}

// gotoXY: Posiciona el cursor en { x, y } de la consola
// Se hicieron tres definiciones de gotoXY: una con integers, y otra con vec2d
// Adicionalmente se definieron gotoX y gotoY para mover el cursor en una sola dirección
void gotoXY(int x, int y) {
#if defined(ANTICINE_UNIX)
    gnu::print(
        "\x1b[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H"
    );
#else
    COORD cursorPosition = { (short)x, (short)y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}

void gotoXY(gnu::vec2d pos) {
#if defined(ANTICINE_UNIX)
    gnu::print(
        "\x1b[" + std::to_string(pos.y + 1) + ";" + std::to_string(pos.x + 1) + "H"
    );
#else
    COORD cursorPosition = { short(pos.x), short(pos.y) };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}

void gotoX(short x) {
#if defined(ANTICINE_UNIX)
    gnu::print(
        "\x1b[" + std::to_string(gnu::getCursorPosition().y + 1) + ";" + std::to_string(x + 1) + "H"
    );
#else
    COORD cursorPosition = { x, short(gnu::getCursorPosition().y) };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}

void gotoY(short y) {
#if defined(ANTICINE_UNIX)
    gnu::print(
        "\x1b[" + std::to_string(y + 1) + ";" + std::to_string(gnu::getCursorPosition().x + 1) + "H"
    );
#else
    COORD cursorPosition = { short(gnu::getCursorPosition().x), y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
#endif
}

// Ejecuta un comando de shell (consola) y devuelve su output como string
std::string exec(std::string command) {
    char buffer[256];
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

// Devuelve el tamaño de la consola (columns/filas) como vector 2d { x, y }
gnu::vec2d getConsoleSize() {
    gnu::vec2d coords;
#if defined(ANTICINE_UNIX)
    // Llamada al sistema para obtener el tamaño de la consola
    // TIOCGWINSZ: "Terminal Input/Output Get Window Size"
    // documentado en: https://man7.org/linux/man-pages/man2/ioctl_tty.2.html
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

// Obtiene la posición absoluta del cursor en la consola
gnu::vec2d getCursorPosition() {
    gnu::vec2d coords;
#if defined(ANTICINE_UNIX)
    // ANSI escape sequence para obtener la posición del cursor
    // La respuesta tiene el siguiente formato: ESC[{y};{x}R
    // y es recibido desde la entrada estándar (stdin)
    write(STDOUT_FILENO, "\x1b[6n", 4);

    char buff[32] = { 0 };
    unsigned long i = 0;
    char ch = 0;

    // Leemos hasta encontrar el caracter 'R'
    for (; ch != 'R' && i < sizeof(buff) - 1; i++) {
        // Leemos "stdin" caracter por caracter
        read(STDIN_FILENO, &ch, 1);
        buff[i] = ch;
    }
    buff[i] = '\0';

    // Parseamos el formato de respuesta
    sscanf(buff, "\x1b[%d;%dR", &coords.y, &coords.x);
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
    std::string line = "";
    int size_x = gnu::getConsoleSize().x;
    for (int i = 0; i < size_x; i++) {
        line += " ";
    }
    gnu::print(line);
}

// Para invocar a la función atexit()
void exit_with_code(int code) {
    exit(code);
}

void cleanupProgram(void) {
    gnu::setCursorVisible(true);
	printf("\x1b[2J");
	// Regresa al buffer normal de la consola
    // (al inicio se cambió al alternative buffer)
	printf("\x1b[?1049l");
#if defined(ANTICINE_UNIX)
    // cleanup
    // Restaura la configuración inicial de la consola
    tcsetattr(STDOUT_FILENO, TCSANOW, &initial_state);
#endif
}

// Inicia el programa con algunas configuraciones básicas
void initProgram() {
    gnu::setCursorVisible(false);
    std::srand(std::time(nullptr));
    // Signals handlers
    atexit(cleanupProgram);
    signal(SIGINT, exit_with_code);  // Ctrl+C
    signal(SIGTERM, exit_with_code); // kill
    // Inicia el modo "alternative buffer"
    printf("\x1b[?1049h");
    // Clenaup
    printf("\x1b[2J");
// En UNIX, configuramos la consola en modo raw
// De esta forma podemos recibir las teclas presionadas como
// un stream de bytes, sin necesidad de presionar enter
#if defined(ANTICINE_UNIX)
    signal(SIGKILL, exit_with_code); // user responsibility?
    signal(SIGTSTP, exit_with_code); // Ctrl+Z

    // Guardamos la configuración inicial de la consola
    tcgetattr(STDOUT_FILENO, &initial_state);
    tcgetattr(STDOUT_FILENO, &term_state);

    // Configuramos la consola en modo raw
    // cfmakeraw(&term_state);
    term_state.c_lflag &= ~(ICANON);
    term_state.c_lflag &= ~(ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &term_state);
    fflush(stdout);
#else
    signal(SIGABRT, exit_with_code); // user responsibility?
    // En windows, configuramos manualmente la salida de la consola
    // para soportar UTF-8
    SetConsoleOutputCP(65001);
#endif
    gnu::gotoXY(0, 0);
}

// Modifica la visibilidad del cursor
void setCursorVisible(bool isVisible) {
    // Se usan escape sequences para modificar la visibilidad del cursor
    gnu::print("\x1b[?25" + std::string(isVisible ? "h" : "l"));
}

void cls() {
#if defined(ANTICINE_UNIX)
    // Se prefirió usar el escape sequence \x1b[2J en lugar de system("clear")
    printf("\x1b[2J");
#else
    system("cls");
#endif
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
    gnu::gotoX(
        short((gnu::getConsoleSize().x - utf8::str_length(line)) / 2)
    );
    gnu::print(line);
}

// Imprime un texto multilinea de forma centrada en la consola
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

    size_t biggestSlice = 0;

    for (std::string substr : subStringsList) {
        if (utf8::str_length(substr) >= biggestSlice) {
            biggestSlice = utf8::str_length(substr);
        }
    }

    int offset = (getConsoleSize().x / 2) - (biggestSlice / 2);

    for (size_t i = 0; i < subStringsList.size(); i++){
        gnu::gotoX(offset);
        gnu::print(subStringsList[i] + "\n");
    }
}

/*
 --------------------------------
 ---- Utilidades adicionales ----
 --------------------------------
*/

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

// Imprime un string multilínea con un offset en X
// Para imprimir centrádamente, véase printRawCenter
void printRawOffset(std::string raw, int offset) {
    std::string buffer = "";

    for (size_t i = 0; i < raw.length(); i++){
        if (raw[i] != '\n' && i != raw.length() - 1){
            buffer += raw[i];
        }
        else{
            if (buffer.empty()) continue;
            if (i == raw.length() - 1) buffer += raw[i];
            gnu::gotoX(offset);
            printf("%s", (buffer + "\n").c_str());
            buffer = "";
        }
    }
    fflush(stdout);
}

// Pausa el proceso por una cierta cantidad de milisegundos
// Esta función no hace efecto en UNIX. Sospecho que se debe al modo raw
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}

void openBrowser(std::string url) {
#if defined(ANTICINE_UNIX)
    std::string command = "xdg-open " + url;
#else
    std::string command = "start " + url;
#endif
    system(command.c_str());
}

} // namespace gnu
