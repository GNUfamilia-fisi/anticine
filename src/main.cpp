#include <thread>
#include <chrono>
#include <style.hpp>
#include <utf8.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "consoleUtils.h"
#include "components/Box.h"
#include "components/Card.h"
#include "menus/mainMenu.h"

int main(void) {
    std::string cinemaID;

    SetConsoleOutputCP(65001);
    //SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_MOUSE_INPUT);

    gnu::setCursorVisible(false);
    gnu::menuSelector();
    gnu::asientos();
    gnu::setCursorVisible(true);

    return 0;
}
