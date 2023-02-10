#include "consoleUtils.h"
#include "menus/mainMenu.h"

int main(void) {
    gnu::setCursorVisible(false);

    gnu::cargandoDisplay();
    gnu::chooseCinema();
    gnu::carteleraFecha();

    return 0;
}
