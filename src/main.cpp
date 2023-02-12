#include <thread>
#include <chrono>
#include "consoleUtils.h"
#include "menus/mainMenu.h"
#include "components/Box.h"
#include "components/RootBox.h"

#include "utf8.hpp"

int main(void) {
    SetConsoleOutputCP(65001);
    system("cls");
    gnu::setCursorVisible(false);
    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    gnu::loadingScreen();
    gnu::chooseCinemaScreen();
    gnu::carteleraFechaScreen();

    char input = '\0';
    while (true) {
        std::thread clear;
        // ---- Receive input ----
        if (_kbhit()) input = _getch();

        /* drawing... */

        // ----- Clear ----
        gnu::gotoXY(0, 0);
        gnu::vec2d size = gnu::getConsoleSize();
        std::cout << "size: (" << size.x << ", " << size.y << ")      ";
        if (gnu::getConsoleSize() != lastConsoleSize) {
            clear = std::thread(system, "cls");
        }

        lastConsoleSize = gnu::getConsoleSize();
        if (clear.joinable()) clear.join();
    }


    gnu::setCursorVisible(true);
    return 0;
}
