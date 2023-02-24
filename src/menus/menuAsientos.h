#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../components/Box.h"

#include "globales.h"

namespace gnu {

std::string menuAsientos(){
    gnu::cls();

    json totalData = apifetch("/session/lol");

    int columns = totalData["room"]["columns_number"].get<int>();
    int rows = totalData["room"]["rows_number"].get<int>();

    //pantalla
    gnu::Box pantalla({ 135, 2 });
    pantalla.setBoxColor({ 20, 156, 178});
    pantalla.centerHorizontal();
    pantalla.showBorder = false;
    pantalla.draw();

    //borde para decorar los asientos
    gnu::Box salaBorder({ columns*6, rows*3 });
    salaBorder.setBoxColor({ 104, 19,1 }); // RGB(104,19,1)
    salaBorder.position.y = 5;
    salaBorder.centerHorizontal();
    salaBorder.showBorder = true;
    salaBorder.transparent = true;
    salaBorder.draw();

    //boton de seleccione el asiento
    gnu::Box boton({ 30, 3 });
    boton.setFontColor({ 255, 138, 208 });
    boton.content = "Seccione su asiento";
    boton.position = { 80, 34 };
    boton.showBorder = true;
    boton.draw();

    //caja para seleccionar asientos
    gnu::Box canvaBox({4,1});
    canvaBox.setBoxColor({ 0, 29, 158 }); //RGB(0, 29, 158)
    canvaBox.transparent = false;
    canvaBox.showBorder = true;

    std::vector<json> eachRow = totalData["room"]["rows"].get<std::vector<json>>();

    int rowY = 0;
    int colX = 0;

    const size_t totalsize = rows * columns;

    std::vector<gnu::vec2d> allPositions(totalsize);

    size_t iter = 0;

    for (int i = 0; i < rows; i++) {
        for (auto seat : eachRow[i]["seats"]) {
            rowY = i;
            colX = seat["col_number"].get<int>();

            if (seat["is_available"].get<bool>()) canvaBox.setBoxColor({ 0, 29, 158 }); 
            else canvaBox.setBoxColor({ 100, 100, 100 }); 

            canvaBox.position = gnu::vec2d({
                6*colX + salaBorder.position.x + 1,
                3*rowY + 6
            });

            canvaBox.draw();

            allPositions[iter] = canvaBox.position;
            iter++;
        }
    }

    // gnu::vec2d lastConsoleSize = gnu::getConsoleSize(); TODO: handle resize
    gnu::Box cursor({4, 1});
    cursor.setBoxColor({28, 209, 61});
    cursor.showBorder = true;
    cursor.transparent = false;

    short cursorX = allPositions[0].x;
    short cursorY = allPositions[0].y;

    gnu::vec2d lastCursorPosition = allPositions[0];

    int input;
    bool redrawLayout = false;

    cursor.position = allPositions[0];
    cursor.draw();

    while(true) {
        input = gnu::getch();

        if (input) {
            redrawLayout = true;

            switch (input) {
            case gnu::key::Right:
                if (std::find(allPositions.begin(), allPositions.end(), gnu::vec2d({cursorX + cursor.size.x + 2, cursorY})) != allPositions.end()) {
                    cursorX += cursor.size.x + 2;
                }
                break;
            case gnu::key::Left:
                if (std::find(allPositions.begin(), allPositions.end(), gnu::vec2d({cursorX - cursor.size.x - 2, cursorY})) != allPositions.end()) {
                    cursorX -= cursor.size.x + 2;
                }
                break;
            case gnu::key::Up:
                if (std::find(allPositions.begin(), allPositions.end(), gnu::vec2d({cursorX, cursorY - cursor.size.y - 2})) != allPositions.end()) {
                    cursorY -= cursor.size.y + 2;
                }
                break;
            case gnu::key::Down:
                if (std::find(allPositions.begin(), allPositions.end(), gnu::vec2d({cursorX, cursorY + cursor.size.y + 2})) != allPositions.end()) {
                    cursorY += cursor.size.y + 2;
                }
                break;
            }
        }


        if (redrawLayout) {
            //"limpiamos" la ultima posicion del cursor
            cursor.position = lastCursorPosition;
            cursor.setBoxColor({ 0, 29, 158 });
            cursor.draw();

            //redibujamos en la nueva posicion
            cursor.position = {cursorX, cursorY};
            cursor.setBoxColor({ 28, 209, 61 });
            cursor.draw();

            lastCursorPosition = {cursorX, cursorY};

            redrawLayout = false;
        }
        gnu::sleep(5);
    }

    return "hola";
}
} // namespace gnu
