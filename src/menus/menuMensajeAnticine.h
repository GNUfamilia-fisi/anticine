#pragma once

#include "../consoleUtils.h"

namespace gnu
{

// Menu especial que muestra un mensaje y luego te redirige a otro menu (o al mismo)
std::string menuMensajeAnticine(std::string menuDeRegreso, std::string mensaje) {
    gnu::cls();
    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    while (true) {
        gnu::gotoY(15);
        style::bold();
        gnu::printRawCenter(gnu::anticineLogo);
        gnu::gotoY(25);
        gnu::printRawCenter(mensaje);
        gnu::gotoY(27);
        gnu::printRawCenter("Presione Enter tecla para continuar");

        if (lastConsoleSize != gnu::getConsoleSize()) {
            gnu::cls();
        }

        if (gnu::getch() == gnu::key::Enter) {
            style::reset();
            return menuDeRegreso;
        }
        lastConsoleSize = gnu::getConsoleSize();
    }
}

} // namespace gnu


