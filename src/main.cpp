#include <thread>
#include <chrono>
#include <style.hpp>
#include <utf8.hpp>
#include "consoleUtils.h"
#include "components/Box.h"

int main(void) {
    SetConsoleOutputCP(65001);
    gnu::cls();
    gnu::setCursorVisible(false);
    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    std::string logo = R"(
_______       __________      _____             
___    |________  /___(_)________(_)___________ 
__  /| |_  __ \  __/_  /_  ___/_  /__  __ \  _ \
_  ___ |  / / / /_ _  / / /__ _  / _  / / /  __/
/_/  |_/_/ /_/\__/ /_/  \___/ /_/  /_/ /_/\___/ 
)";
    gnu::Box box1({ 18, 15 });
    box1.content = "AVATAR 2 EL CAMINO DEL AGUA";
    box1.setBoxColor({ 209, 167, 77 });
    box1.position.y = 11;

    gnu::Box box2({ 20, 17 });
    box2.content = "ASU MARE LOS AMIGOS";
    box2.setBoxColor({ 102, 196, 127 });
    box2.position.y = 10;

    gnu::Box box3({ 18, 15 });
    box3.content = "GATO CON BOTAS EL ULTIMO DESEO";
    box3.setBoxColor({ 189, 121, 163 });
    box3.position.y = 11;

    gnu::Box textBox({ 70, 8 });
    textBox.content = "Luego del éxito de taquilla de Asu Mare 1, 2 y 3 una nueva aventura está por empezar. Los recordados amigos de Cachin están de regreso: El Culi, Lechuga, El Chato y Poroto nos demostrarán que la amistad lo puede todo. Después de superar muchas adversidades, una herencia los pondrá a prueba, enfrentándose en divertidas situaciones y a oscuros personajes que     intentarán boicotear uno de sus más grandes sueños. ¡La apertura del nuevo restaurante Asu Mare!";
    textBox.setBoxColor({ 100, 100, 100 });
    textBox.setFontColor({ 220, 220, 220 });
    textBox.position.y = 33;
    textBox.transparent = true;
    textBox.showBorder = false;

    gnu::Box buttonLeft({ 6, gnu::getConsoleSize().y });
    buttonLeft.setBoxColor({ 35, 35, 35 });
    buttonLeft.position.x = 0;
    buttonLeft.content = "<-";
    buttonLeft.showBorder = false;
    buttonLeft.setFontColor({ 150, 150, 150 });

    gnu::Box buttonRight({ 6, gnu::getConsoleSize().y });
    buttonRight.setBoxColor({ 35, 35, 35 });
    buttonRight.position.x = gnu::getConsoleSize().x - 6;
    buttonRight.content = "->";
    buttonRight.showBorder = false;
    buttonRight.setFontColor({ 150, 150, 150 });
    char input = '\0';

    while (true) {
        // ---- Receive input ----
        if (_kbhit()) input = _getch();

        // ---- Update ----
        style::setFg({ 128, 186, 209 });
        gnu::printRawCenter(logo);
        style::setFg({ 222, 197, 153 });

        style::underline();
        gnu::printLineCentered("Estás en Anticine - GAMARRA");
        style::underline_off();

        box2.centerHorizontal();
        box1.setPosition( box2.position + gnu::vec2d{ (short)(-box1.size.x - 10), 1 } );
        box3.setPosition( box2.position + gnu::vec2d{ (short)(box2.size.x + 10), 1 } );
        box1.draw();
        box2.draw();
        box3.draw();

        gnu::print("\n\n\n");
        gnu::printLineCentered("ASU MARE 4: LA HERENCIA DE CACHIN");
        gnu::print("\n");
        gnu::printLineCentered("😎 🤑 🙄");
        gnu::print("\n");
        gnu::printLineCentered("90m | 3D, 2D | 14PG");
        textBox.centerHorizontal();
        textBox.draw();

        buttonLeft.size.y = gnu::getConsoleSize().y;
        buttonRight.size.y = gnu::getConsoleSize().y;
        buttonRight.position.x = gnu::getConsoleSize().x - 7;
        buttonLeft.draw();
        buttonRight.draw();

        // ----- Clear ----
        gnu::gotoXY(0, 0);
        gnu::vec2d size = gnu::getConsoleSize();
        if (gnu::getConsoleSize() != lastConsoleSize) {
            gnu::cls();
        }

        lastConsoleSize = gnu::getConsoleSize();
    }
    gnu::setCursorVisible(true);
    return 0;
}
