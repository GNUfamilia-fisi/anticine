#include <style.hpp>
#include <utf8.hpp>

#include "consoleUtils.h"
#include "menus/selectorDeMenus.h"
#include "menus/menuFormulario.h"

int main(void) {
    gnu::initProgram();

    // Selector de menús
    gnu::menuSelector();

    // Formulario en modo pruebita (demostración de la Input API)
    gnu::menuFormulario();

    return 0;
}
