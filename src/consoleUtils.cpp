#include <Windows.h>
#include <json.hpp>
#include "consoleUtils.h"

using json = nlohmann::json;

// Variable global para el manejo de la consola
HANDLE gnu::consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

// Posiciona el cursor en { x, y } de la consola ("y" invertida)
void gnu::gotoXY(short x, short y) {
    COORD cursorPosition = { x, y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}

// Ejecuta un comando de shell de forma síncrona
// y devuelve su stdout como string
std::string gnu::exec(std::string command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}

gnu::vec2d gnu::getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    gnu::vec2d coords = { BufferInfo.dwSize.X, BufferInfo.dwSize.Y };
    return coords;
}

void gnu::cleanLine() {
    for (int i = 0; i < gnu::getConsoleSize().x; i++) {
        std::cout << " ";
    }
}

void gnu::setCursorVisible(bool isVisible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = isVisible;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void gnu::sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(ms));
}
