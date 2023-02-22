#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../services.h"

#include "globales.h"

namespace gnu {

// // debería ser una variable global
// std::string cineID;

void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize);


std::string elijaLocalTitulo = R"(
         _             _
 ___ ___| |___ ___ ___|_|___ ___ ___
|_ -| -_| | -_|  _|  _| | . |   | .'|
|___|___|_|___|___|___|_|___|_|_|__,|
               _             _
    _ _ ___   | |___ ___ ___| |
   | | |   |  | | . |  _| .'| |
   |___|_|_|  |_|___|___|__,|_|
)";

std::string menuElegirCine() {
    gnu::cls();

    // Retorna una lista de cines ordenados según la geolocalización
    // del usuario, el primero siempre es el más cercano.
    // Dependiendo de 
    
    json nearCinemasData = gnu::apifetch("/cines/cercanos");

    size_t currentCine = 0;
    int nameCursorPos = 0;
    short listLength = 5;
    bool lock = true;
    
    gnu::printRawCenter(elijaLocalTitulo);
    int input;
    
    // main menu loop
    while (lock) {
        gnu::gotoXY(0, 2);
        input = gnu::getch();

        switch (input) {
            case gnu::key::Down: {
                for (short i = 0; i < listLength; i++) {
                    gnu::cleanLine(20+i);
                }

                if (currentCine < nearCinemasData["cinemas"].size() - 1) {
                    currentCine++;
                }
                if (nameCursorPos < listLength - 1) {
                    nameCursorPos++;
                }
                else {
                    nameCursorPos = listLength - 1;
                }
                break;
            }
            case gnu::key::Up: {
                for (short i = 0; i < listLength; i++) {
                    gnu::cleanLine(20 + i);
                }
                if (currentCine > 0) {
                    currentCine--;
                }
                break;
            }
            // Si selecciona un cine, lo guardamos en la variable global cinemaID
            // que será usado por el siguiente menú
            case gnu::key::Enter: {
                g_cineID = nearCinemasData["cinemas"][currentCine]["cinema_id"].get<std::string>();
                lock = false;
                break;
            }
        }

        gnu::cinemaListDisplay(nearCinemasData["cinemas"], currentCine, nameCursorPos, listLength);
        gnu::sleep(5);
    }

    return "menuCartelera";
}

void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize) {
    style::setColor(style::color::WHITE);

    size_t indic = namePos;

    std::string displayName;

    if (closeData.size() < showSize) {
        showSize = closeData.size();
    }
    if (current < showSize) {
        indic = current;
    }

    // std::string names[showSize];
    std::string names[128];

    if (current < showSize) {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i]["name"].get<std::string>();
        }
    }
    else {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i + current - showSize + 1]["name"].get<std::string>();
        }
    }

    for (size_t i = 0; i < showSize; i++) {

        if (i == indic) {
            gnu::gotoXY(
                (gnu::getConsoleSize().x / 2) - names[i].length() - 1,
                20 + i
            );

            style::setColor(style::GREEN);
            gnu::print(">>");
        }
        else {
            style::setColor(style::WHIRTE);
        }
        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (names[i].length() / 2),
            20 + i
        );

        gnu::print(names[i]);
    }
}

} // namespace gnu
