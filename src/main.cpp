#include "consoleUtils.h"
#include "menus/mainMenu.h"

int main(void) {
    gnu::setCursorVisible(false);

    gnu::loadingScreen();
    gnu::chooseCinemaScreen();
    gnu::carteleraFechaScreen();

    gnu::setCursorVisible(true);
    return 0;
}
