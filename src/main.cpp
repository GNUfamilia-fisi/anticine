#include <thread>
#include <chrono>
#include "consoleUtils.h"
#include "menus/mainMenu.h"
#include "components/Box.h"
#include "components/RootBox.h"

// #pragma execution_character_set("utf-8");

void cls(HANDLE hConsole) {
    char fill = ' ';
    COORD tl = {0,0};
    CONSOLE_SCREEN_BUFFER_INFO s;
    GetConsoleScreenBufferInfo(hConsole, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, fill, cells, tl, &written);
    FillConsoleOutputAttribute(hConsole, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(hConsole, tl);
}

#include "utf8.hpp"

int main(void) {
    // https://stackoverflow.com/a/35576741
    SetConsoleOutputCP(65001);
    system("cls");

    std::string test = "├─┤ │││ │  │ │   │ │ │  │├┤";
    std::cout << "std::string::length(): " << test.length() << "\n";
    std::cout << "utf8::str_length(): " << utf8::str_length(test) << "\n";
    exit(0);

    gnu::setCursorVisible(false);

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    gnu::Box box({ 16, 15 });
    // box.color = gnu::color::RED;
    std::string str = "áéíóú 🙀👌👌";
    std::cout << "length: " << str.length() << "\n";
    std::cout << "utf8::str_length(str): " << utf8::str_length(str) << "\n";
    exit(0);

    box.content = "lorem hugos ipsum dolor sit amet, consectetur adipiscing elit Sed euismod";

    char input = '\0';
    while (true) {
        std::thread clear;
        // ---- Receive input ----
        if (_kbhit()) input = _getch();
        gnu::gotoXY(0, 0);
        // std::cout << "Input: " << input << " (" << (int)input << ")     ";

        // ---- Update ----
        box.draw();
        // ----------------

        // ----- Clear ----
        if (gnu::getConsoleSize() != lastConsoleSize) {
            clear = std::thread(cls, gnu::consoleHandle);
        }

        lastConsoleSize = gnu::getConsoleSize();
        if (clear.joinable()) clear.join();
    }


    // gnu::loadingScreen();
    // gnu::chooseCinemaScreen();
    // gnu::carteleraFechaScreen();

    gnu::setCursorVisible(true);
    return 0;
}
