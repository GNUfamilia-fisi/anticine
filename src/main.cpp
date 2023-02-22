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
#include "components/MovieCard.h"
#include "menus/selectorDeMenus.h"

#include <iostream>
#include <unistd.h>

int main(void) {
    gnu::initProgram();
    gnu::menuAsientos();

    return 0;
}
