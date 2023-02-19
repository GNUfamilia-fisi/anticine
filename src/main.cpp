#include <thread>
#include <chrono>
#include <style.hpp>
#include <utf8.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <csignal>

#include "consoleUtils.h"
#include "components/Box.h"
#include "components/Card.h"
#include "menus/mainMenu.h"

int main(void) {
    gnu::initProgramConf();

    gnu::menuSelector();
    gnu::asientos();

    gnu::endProgram();
    return 0;
}
