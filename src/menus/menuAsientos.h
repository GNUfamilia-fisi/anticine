#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../components/Box.h"
#include "../components/Button.h"

#include "globales.h"

namespace gnu {

#define SCREEN_COLOR style::rgb({ 20, 156, 178 })
#define BORDER_DECORATION_COLOR style::rgb({ 104, 19, 1 })

#define OPCION_SELECCIONANDO_SILLAS 0
#define OPCION_BOTON_CONFIRMAR 1

#define COLOR_SILLA_DISPONIBLE style::rgb({ 0, 29, 158 })
#define COLOR_SILLA_OCUPADA style::rgb({ 255, 0, 0 })
#define COLOR_SILLA_SELECCIONADA style::rgb({ 255, 138, 208 })
#define COLOR_BORDE_SILLAS_EN_CURSOR style::rgb({ 0, 200, 0 })

#define FIND(vec, val) (std::find(vec.begin(), vec.end(), (val)) != vec.end())

#define MENU_OPTION_SILLAS 0
#define MENU_OPTION_BOTON_CONFIRMAR 1

// NOTA: ahora las sillas seleccionadas se muestran en color rosado
//       y el botón de confirmar funciona
//       esto ya estaría casi listo, solo falta pequeños retoques y guardar las sillas
//       seleccionadas como variable global
//       (probablemente también se manden al servidor para verificar si esas sillas no están ocupadas)

// NOTA: faltan elegir buenos colores

// antes de refactorizar el código, las sillas se seleccionaban
// según su posición en la console, cosa que no era buena idea y casi me hace enloquecer.
// Ahora se seleccionan según su posición en la sala (fila y columna)

std::string menuAsientos(){
    gnu::cls();

    json sessionData = apifetch("/session/" + g_sessionID);

    const int columns = sessionData["room"]["columns_number"].get<int>();
    const int rows = sessionData["room"]["rows_number"].get<int>();

    int menuOption = MENU_OPTION_SILLAS;

    // pantalla
    gnu::Box pantalla({ 135, 2 });
    pantalla.setBoxColor(SCREEN_COLOR);
    pantalla.centerHorizontal();
    pantalla.showBorder = false;
    pantalla.draw();

    // borde para decorar los asientos
    gnu::Box salaBorder({ columns*6, rows*3 });
    salaBorder.setBoxColor(BORDER_DECORATION_COLOR);
    salaBorder.position.y = 5;
    salaBorder.centerHorizontal();
    salaBorder.showBorder = true;
    salaBorder.transparent = true;
    salaBorder.draw();

    // boton de seleccione el asiento
    gnu::Button botonConfirmar("Confirmar selección", { 30, 1 });
    botonConfirmar.setFontColor({ 255, 138, 208 });
    botonConfirmar.position.y = gnu::getConsoleSize().y - 3;
    botonConfirmar.centerHorizontal();
    botonConfirmar.draw();

    std::vector<json> roomRows = sessionData["room"]["rows"].get<std::vector<json>>();

    const size_t totalsize = rows * columns;

    // Las posiciones de las sillas en la pantalla
    // Las coordenadas de las sillas en la sala
    std::vector<gnu::vec2d> seatsRoomPosition(totalsize);
    // El estado de las sillas (ocupado/disponible)
    std::vector<std::vector<bool>> statusSelectable(rows, std::vector<bool>(columns));

    // caja reutilizable para dibujar las sillas
    gnu::Box canvaBox({ 4, 1 });
    canvaBox.setBoxColor(COLOR_SILLA_DISPONIBLE);
    canvaBox.setBorderColor(COLOR_BORDE_SILLAS_EN_CURSOR);
    canvaBox.transparent = false;
    canvaBox.showBorder = false;

    size_t seats_i = 0;
    bool firstWithBorder = true;
    // primer dibujado y llenado de variables
    for (int rowY = 0; rowY < rows; rowY++) {
        for (auto seat : roomRows[rowY]["seats"]) {
            int colX = seat["col_number"].get<int>();
            bool seat_ocupied = seat["is_ocupied"].get<bool>();
            statusSelectable[rowY][colX] = !seat_ocupied;

            canvaBox.setBoxColor(seat_ocupied ? COLOR_SILLA_OCUPADA : COLOR_SILLA_DISPONIBLE);

            canvaBox.position = gnu::vec2d({
                6 * colX + salaBorder.position.x + 1,
                3 * rowY + 6
            });

            // esto es para arreglar un bug muy raro que hace que la primera
            // silla SELECCIONADA no tenga border en el primer dibujado
            if (firstWithBorder) {
                canvaBox.showBorder = true;
                firstWithBorder = false;
            }
            else {
                canvaBox.showBorder = false;
            }
            canvaBox.draw();
            seatsRoomPosition[seats_i] = gnu::vec2d({ colX, rowY });
            seats_i++;
        }
    }

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    std::vector<gnu::vec2d> selectedPositions;

    // cursorCoord hace referencia a la coordenada del cursor en [columnas, filas]
    //  Anteriormente hacía referencia al cursor real en la pantalla, pero
    //  esto era poco eficiente
    gnu::vec2d cursorCoord = seatsRoomPosition[0];
    gnu::vec2d lastCursorCoord = cursorCoord;
    int input;
    bool hasPressedKey = false;

    while (true) {
        input = gnu::getch();

        if (input) {
            // asumimos que se presionó una tecla,
            // pero si llegamos al case: default, no se presionó ninguna
            hasPressedKey = true;
            lastCursorCoord = { cursorCoord.x, cursorCoord.y };

            switch (input) {
            case gnu::key::Right:
                if (menuOption != MENU_OPTION_SILLAS) break;
                if (FIND(seatsRoomPosition, gnu::vec2d({cursorCoord.x + 1, cursorCoord.y}))) {
                    cursorCoord.x++;
                }
                break;
            case gnu::key::Left:
                if (menuOption != MENU_OPTION_SILLAS) break;
                if (FIND(seatsRoomPosition, gnu::vec2d({cursorCoord.x - 1, cursorCoord.y}))) {
                    cursorCoord.x--;
                }
                break;
            case gnu::key::Up:
                if (menuOption == MENU_OPTION_BOTON_CONFIRMAR) {
                    menuOption = MENU_OPTION_SILLAS;
                    break;
                }
                if (FIND(seatsRoomPosition, gnu::vec2d({cursorCoord.x, cursorCoord.y - 1}))) {
                    cursorCoord.y--;
                }
                break;
            case gnu::key::Down:
                if (menuOption == MENU_OPTION_SILLAS && cursorCoord.y == rows - 1) {
                    menuOption = MENU_OPTION_BOTON_CONFIRMAR;
                    break;
                }
                if (FIND(seatsRoomPosition, gnu::vec2d({cursorCoord.x, cursorCoord.y + 1}))) {
                    cursorCoord.y++;
                }
                break;
            case gnu::key::Enter: {
                if (menuOption == MENU_OPTION_BOTON_CONFIRMAR) {
                    // sillas guardadas en el vector selectedPositions
                    // TODO: enviar al servidor y esperar respuesta
                    g_selectedSeats = selectedPositions;
                    return "menuFormularioLogin";
                }
                bool is_selectable = statusSelectable[lastCursorCoord.y][lastCursorCoord.x];
                LOG_FILE("is_selectable: " << is_selectable << "\n");

                if (is_selectable) {
                    // Si no esta en la lista, agregarlo
                    const gnu::vec2d currentPos = { lastCursorCoord.x, lastCursorCoord.y };
                    if (!FIND(selectedPositions, gnu::vec2d(currentPos))) {
                        selectedPositions.push_back(gnu::vec2d(currentPos));
                        // Beep(500,200);
                    }
                    else {
                        // si no esta en la lista, quitarlo (toggle)
                        selectedPositions.erase(
                            std::remove(
                                selectedPositions.begin(), selectedPositions.end(),
                                gnu::vec2d(currentPos)
                            ), selectedPositions.end()
                        );
                    }
                }
                break;
            }
            default:
                hasPressedKey = false;
                break;
            }
        }

        bool needToRedraw = lastConsoleSize != gnu::getConsoleSize();

        if (hasPressedKey || needToRedraw) {
            if (needToRedraw) {
                gnu::cls();
                pantalla.centerHorizontal();
                salaBorder.centerHorizontal();
                botonConfirmar.position.y = gnu::getConsoleSize().y - 3;
                botonConfirmar.centerHorizontal();
                pantalla.draw();
                salaBorder.draw();
                botonConfirmar.draw();
            }
            // se recorren todas las sillas para actualizar su estado en pantalla
            for (int rowY = 0; rowY < rows; rowY++) {
                for (auto seat : roomRows[rowY]["seats"]) {
                    int colX = seat["col_number"].get<int>();
                    bool seat_ocupied = !statusSelectable[rowY][colX];

                    canvaBox.position = gnu::vec2d({
                        6 * colX + salaBorder.position.x + 1,
                        3 * rowY + 6
                    });

                    bool is_selected_by_user = FIND(selectedPositions, gnu::vec2d({ colX, rowY }));
                    bool is_on_cursor = cursorCoord.x == colX && cursorCoord.y == rowY;

                    if (is_selected_by_user) {
                        canvaBox.setBoxColor(COLOR_SILLA_SELECCIONADA);
                        if (!is_on_cursor) {
                            canvaBox.flushBorders();
                            canvaBox.showBorder = false;
                        }
                        else canvaBox.showBorder = true;
                    }
                    else if (is_on_cursor && menuOption == MENU_OPTION_SILLAS) {
                        canvaBox.setBoxColor(
                            statusSelectable[rowY][colX] ? COLOR_SILLA_DISPONIBLE : COLOR_SILLA_OCUPADA
                        );
                        canvaBox.showBorder = true;
                    }
                    else {
                        canvaBox.setBoxColor(
                            seat_ocupied ? COLOR_SILLA_OCUPADA : COLOR_SILLA_DISPONIBLE
                        );
                        canvaBox.showBorder = false;
                        canvaBox.flushBorders();
                    }
                    canvaBox.draw();
                }
            }
        }
        if (menuOption == MENU_OPTION_BOTON_CONFIRMAR) {
            botonConfirmar.focus();
        }
        else {
            botonConfirmar.unfocus();
        }
        botonConfirmar.draw();

        lastConsoleSize = gnu::getConsoleSize();
        gnu::sleep(5);
    }

    return "hola";
}
} // namespace gnu
