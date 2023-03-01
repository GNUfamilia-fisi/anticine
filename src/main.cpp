#include <style.hpp>
#include <utf8.hpp>

#include "consoleUtils.h"
#include "menus/selectorDeMenus.h"
#include "menus/menuFormInicioSesion.h"
#include "menus/menuFormRegistro.h"

int main(void) {
    gnu::initProgram();

    // Selector de menús
    gnu::menuSelector();

    // Formulario en modo pruebita (demostración de la Input API)
    // gnu::menuFormularioRegistro();
    // gnu::menuFormularioLogin();
    // std::cin.get();

    return 0;
}
