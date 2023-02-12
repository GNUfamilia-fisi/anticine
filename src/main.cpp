#include <thread>
#include <chrono>
#include "consoleUtils.h"
#include "menus/mainMenu.h"
#include "components/Box.h"
#include "components/RootBox.h"

// #pragma execution_character_set("utf-8");

void cls() {
    system("cls");
}

#include "utf8.hpp"

int main(void) {
    // https://stackoverflow.com/a/35576741
    SetConsoleOutputCP(65001);
    // system("cls");
    gnu::setCursorVisible(false);
    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

//     gnu::setFgRGBColor({ 128, 186, 209 });
//     std::string pstr = R"(
//                      _______       __________      _____             
//                      ___    |________  /___(_)________(_)___________ 
//                      __  /| |_  __ \  __/_  /_  ___/_  /__  __ \  _ \
//                      _  ___ |  / / / /_ _  / / /__ _  / _  / / /  __/
//                      /_/  |_/_/ /_/\__/ /_/  \___/ /_/  /_/ /_/\___/ 
// )";
// //     std::string pstr = R"(
// //                                    ┌─┐┌┐┌┬┐┬┌─┐┬┌┐┌┌─┐
// //                                    ├─┤││││ ││  ││││├┤
// //                                    ┴ ┴┘└┘┴ ┴└─┘┴┘└┘└─┘
// // )";
//     std::cout << pstr << std::endl;
//     gnu::setFgRGBColor({ 157, 159, 212 });
//     std::cout << "                              Estás en ";
//     gnu::setFgRGBColor({ 222, 197, 153 });
//     std::cout << "Anticine - GAMARRA";

//     // ┌───────┐
//     // │       │
//     // └───────┘
//     /*
//     Luego del éxito de taquilla de Asu Mare 1, 2 y 3 una nueva aventura
//     está por empezar, los recordados amigos de Cachin están de regreso:
//     El Culi, Lechuga, El Chato y Poroto nos demostrarán que la amistad
//     lo puede todo. Después de superar muchas adversidades, una herencia
//     los pondrá a prueba, enfrentándose en divertidas situaciones y a
//     oscuros personajes que intentarán boicotear uno de sus más grandes
//     sueños. ¡La apertura del nuevo restaurante Asu Mare!*/
//     gnu::Box box1({ 18, 15 });
//     box1.content = "AVATAR 2 EL CAMINO DEL AGUA";
//     box1.setColor({ 209, 167, 77 });
//     box1.position = { 8, 11 };
//     box1.draw();

//     gnu::Box box2({ 20, 17 });
//     box2.content = "ASU MARE LOS AMIGOS";
//     box2.setColor({ 102, 196, 127 });
//     box2.position = { 34, 10 };
//     box2.draw();

//     gnu::Box box3({ 18, 15 });
//     box3.content = "GATO CON BOTAS EL ULTIMO DESEO";
//     box3.setColor({ 189, 121, 163 });
//     box3.position = { 62, 11 };
//     box3.draw();

//     std::cout << "\n\n\n                          ASU MARE 4: LA HERENCIA DE CACHIN\n";
//     std::cout << "                                       😎 🤑 🙄\n";
//     std::cout << "\n                                  90m | 3D, 2D | 14PG\n\n";

//     gnu::Box textBox({ 70, 8 });
//     textBox.content = "Luego del éxito de taquilla de Asu Mare 1, 2 y 3 una nueva aventura está por empezar, los recordados amigos de Cachin están de regreso: El Culi, Lechuga, El Chato y Poroto nos demostrarán que la amistad lo puede todo. Después de superar muchas adversidades, una herencia los pondrá a prueba, enfrentándose en divertidas situaciones y a oscuros personajes que     intentarán boicotear uno de sus más grandes sueños. ¡La apertura del nuevo restaurante Asu Mare!";
//     textBox.setColor({ 0, 0, 0 });
//     textBox.setFontColor({ 220, 220, 220 });
//     textBox.position = { 8, 33 };
//     textBox.draw();

//     // std::cout << "       ┌─────────────────────────────────────────────────────────────────────┐" << std::endl;
//     // std::cout << "       │ Luego del éxito de taquilla de Asu Mare 1, 2 y 3 una nueva aventura │" << std::endl;
//     // std::cout << "       │ está por empezar, los recordados amigos de Cachin están de regreso: │" << std::endl;
//     // std::cout << "       │ El Culi, Lechuga, El Chato y Poroto nos demostrarán que la amistad  │" << std::endl;
//     // std::cout << "       │ lo puede todo. Después de superar muchas adversidades, una herencia │" << std::endl;
//     // std::cout << "       │ los pondrá a prueba, enfrentándose en divertidas situaciones y a    │" << std::endl;
//     // std::cout << "       │ oscuros personajes que intentarán boicotear uno de sus más grandes  │" << std::endl;
//     // std::cout << "       │ sueños. ¡La apertura del nuevo restaurante Asu Mare!                │" << std::endl;
//     // std::cout << "       └─────────────────────────────────────────────────────────────────────┘" << std::endl;

    // exit(0);


    char input = '\0';
    while (true) {
        std::thread clear;

        // ---- Receive input ----
        if (_kbhit()) input = _getch();

        // ---- Update ----
        // box1.draw();
        // ----------------

        // ----- Clear ----
        gnu::gotoXY(0, 0);
        gnu::vec2d size = gnu::getConsoleSize();
        std::cout << "size: (" << size.x << ", " << size.y << ")      ";
        if (gnu::getConsoleSize() != lastConsoleSize) {
            clear = std::thread(cls);
        }

        lastConsoleSize = gnu::getConsoleSize();
        if (clear.joinable()) clear.join();
    }


    // gnu::loadingScreen();
    // gnu::chooseCinemaScreen();
    // gnu::carteleraFechaScreen();

    gnu::setCursorVisible(true);
    return 0;
}
