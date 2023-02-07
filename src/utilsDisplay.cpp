#include "json.hpp"
#include "utilsDisplay.h"

using json = nlohmann::json;
using namespace nlohmann::literals;

COORD cursorPosition;
HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//
//
void gotoXY(short x, short y) {
    cursorPosition = { x, y };
    SetConsoleCursorPosition(consoleWinHandle, cursorPosition);
}

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

json fetch(std::string f) {
    std::string ToFetch = "curl -s " + f;
    std::string fetchedString = exec(ToFetch.c_str());
    json data = json::parse(fetchedString);

    return data;
}

consoleSize getConsoleRectSize() {
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    consoleSize obj = {
        BufferInfo.srWindow.Right - BufferInfo.srWindow.Left + 1,
        BufferInfo.srWindow.Bottom - BufferInfo.srWindow.Top + 1,
    };

    return obj;
}

void cleanLine() {
    for (int i = 0; i < getConsoleRectSize().x; i++) {
        std::cout << " ";
    }
}

void ShowConsoleCursor(bool showFlag) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleWinHandle, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(consoleWinHandle, &cursorInfo);
}

void logoDisplay3D(DWORD color) {
    SetConsoleTextAttribute(consoleWinHandle, color);
    std::cout << R"(
         $$$$$$\  $$\ $$\   $$\ $$$$$$$$\ $$\   $$\ $$$$$$$$\
        $$  __$$\ \__|$$$\  $$ |$$  _____|$$ |  $$ |\__$$  __|
        $$ /  \__|$$\ $$$$\ $$ |$$ |      \$$\ $$  |   $$ |   
        $$ |      $$ |$$ $$\$$ |$$$$$\     \$$$$  /    $$ |   
        $$ |      $$ |$$ \$$$$ |$$  __|    $$  $$<     $$ |   
        $$ |  $$\ $$ |$$ |\$$$ |$$ |      $$  /\$$\    $$ |   
        \$$$$$$  |$$ |$$ | \$$ |$$$$$$$$\ $$ /  $$ |   $$ |   
         \______/ \__|\__|  \__|\________|\__|  \__|   \__|   
    )" << std::endl;
}

void logoDisplay2D(DWORD color) {
    SetConsoleTextAttribute(consoleWinHandle, color);
    std::cout << R"(
       _______ _   _________  ________
      / ____(_/ | / / ____| |/ /_  __/
     / /   / /  |/ / __/  |   / / /   
    / /___/ / /|  / /___ /   | / /    
    \____/_/_/ |_/_____//_/|_|/_/     
                                  
    )" << std::endl;
}
