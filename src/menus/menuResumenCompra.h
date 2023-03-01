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
                g_cineData = nearCinemasData["cinemas"][currentCine];
                g_cineID = g_cineData["cinema_id"].get<std::string>();
                lock = false;
                break;
            }
        }

        gnu::cinemaListDisplay(nearCinemasData["cinemas"], currentCine, nameCursorPos, listLength);
        gnu::sleep(5);
    }

    return "menuCartelera";
}

} // namespace gnu
