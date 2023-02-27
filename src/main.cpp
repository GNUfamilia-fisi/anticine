#include <style.hpp>
#include <utf8.hpp>

#include "consoleUtils.h"
#include "menus/selectorDeMenus.h"
#include "menus/menuFormulario.h"

int main(void) {
    gnu::initProgram();

    // Selector de menús
    gnu::menuSelector();
   
   //auto hola = gnu::menuAsientos();

    return 0;
}
