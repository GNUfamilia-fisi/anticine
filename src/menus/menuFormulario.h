#pragma once

#include "../components/Input.h"
#include "../consoleUtils.h"

#include "globales.h"

namespace gnu {

#define N_FIELDS 3

// Demostración de la Input API, sin uso real
// TODO: Implementar pequeños campos de texto encima del input
//       que describan su contenido (ej: "Nombre", "Edad", "Dirección")
// TODO: input numérico
// TODO: validación de input y estado de error (rojo)

std::string menuFormulario() {

    // Fields creadas en un array para acceder a posiciones dinámicamente
    int focus_index = 0;
    gnu::Input fields[N_FIELDS] = {
        gnu::Input(20),
        gnu::Input(10),
        gnu::Input(30)
    };

    // Posicionamos los inputs relativamente
    fields[0].centerHorizontal();
    fields[1].setPosition({ fields[0].position.x, fields[0].position.y + 4 });
    fields[2].setPosition({ fields[1].position.x, fields[1].position.y + 4 });

    // Los dibujamos por primera vez
    for (int i = 0; i < N_FIELDS; i++) {
        fields[i].draw();
    }

    while (true) {
        // Actualizamos el estado de focus de cada input
        for (int i = 0; i < N_FIELDS; i++) {
            // Al cambiar el focus, se redibuja el input automáticamente
            fields[i].setFocus(i == focus_index);
        }

        int input = gnu::getch();
        if (input) {
            gnu::Input::handleInfo result = fields[focus_index].handle(input);

            if (result.wanna_go_previous && focus_index > 0) {
                focus_index--;
            }
            if (result.wanna_go_next && focus_index + 1 < N_FIELDS) {
                focus_index++;
            }
        }

    }

    return "exit"; // no implementado
}
} // namespace gnu
