#pragma once

#include "../components/Input.h"
#include "../consoleUtils.h"
#include "../menus/menuMensajeAnticine.h"

#include "globales.h"

namespace gnu {

#define REGISTRO_OPCION_FORMULARIO 0
#define REGISTRO_OPCION_BOTONES 1
#define REGISTRO_HEADER_OFFSET_Y 14

// Demostración de la Input API, sin uso real
// TODO: Implementar pequeños campos de texto encima del input
//       que describan su contenido (ej: "Nombre", "Edad", "Dirección")
// TODO: input numérico
// TODO: validación de input y estado de error (rojo)

std::string menuFormularioRegistro() {
    gnu::cls();

    // Fields creadas en un array para acceder a posiciones dinámicamente
    int focus_index = 0;
    constexpr int N_FIELDS = 5;
    gnu::Input registrationFields[N_FIELDS] = {
        gnu::Input("Nombres", 30),
        gnu::Input("Apellidos", 30),
        gnu::Input("DNI", 30),
        gnu::Input("Correo electrónico", 30),
        gnu::Input("Contraseña", 30)
    };
    gnu::Button alreadyHaveAccount("Ya tengo una cuenta");
    constexpr int N_BUTTONS = 3;
    gnu::Button buttons[] = {
        gnu::Button("Registrarse", { 30, 1 }),
        gnu::Button("Ya tengo una cuenta", { 30, 1 }),
        gnu::Button("Continuar como invitado", { 30, 1 })
    };

    enum Buttons : int {
        Registrarse,
        YaTengoUnaCuenta,
        ContinuarComoInvitado
    };

    // Posicionamos los inputs relativamente
    registrationFields[0].position.y = REGISTRO_HEADER_OFFSET_Y;
    registrationFields[0].centerHorizontal();
    registrationFields[1].setPosition({ registrationFields[0].position.x, registrationFields[0].position.y + 5 });
    registrationFields[2].setPosition({ registrationFields[1].position.x, registrationFields[1].position.y + 5 });
    registrationFields[3].setPosition({ registrationFields[2].position.x, registrationFields[2].position.y + 5 });
    registrationFields[4].setPosition({ registrationFields[3].position.x, registrationFields[3].position.y + 5 });
    buttons[Registrarse].position.y = registrationFields[4].position.y + 5;
    buttons[YaTengoUnaCuenta].position.y = buttons[Registrarse].position.y + 3;
    buttons[ContinuarComoInvitado].position.y = buttons[YaTengoUnaCuenta].position.y + 3;
    buttons[Registrarse].centerHorizontal();
    buttons[YaTengoUnaCuenta].centerHorizontal();
    buttons[ContinuarComoInvitado].centerHorizontal();

    int menuOption = REGISTRO_OPCION_FORMULARIO;
    int button_focus_i = 0;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    bool firstDraw = true;
    while (true) {
        int input = gnu::getch();
        if (input) {
            if (menuOption == REGISTRO_OPCION_FORMULARIO) {
                gnu::Input::handleInfo result = registrationFields[focus_index].handle(input);
                if (result.wanna_go_previous) {
                    if (focus_index > 0) {
                        focus_index--;
                    }
                    else if (menuOption == REGISTRO_OPCION_BOTONES) {
                        menuOption = REGISTRO_OPCION_FORMULARIO;
                    }
                }
                if (result.wanna_go_next) {
                    if (focus_index + 1 < N_FIELDS) {
                        focus_index++;
                    }
                    else {
                        menuOption = REGISTRO_OPCION_BOTONES;
                    }
                }
            }
            else switch (input) {
                case gnu::key::Up: {
                    if (button_focus_i > 0) button_focus_i--;
                    else menuOption = REGISTRO_OPCION_FORMULARIO; // volver al formulario
                    break;
                }
                case gnu::key::Down: {
                    if (button_focus_i + 1 < N_BUTTONS) button_focus_i++;
                    break;
                }
                case gnu::key::Enter: {
                    if (button_focus_i == Buttons::Registrarse) {
                        // No implementado
                        // Debería verificar los datos en la db y redirigir a menú de compra
                        json data = {
                            { "fullname", registrationFields[0].text + " " + registrationFields[1].text },
                            { "DNI", registrationFields[2].text },
                            { "email", registrationFields[3].text },
                            { "password", registrationFields[4].text }
                        };
                        json response = gnu::apipost("/register", data);
                        LOG_FILE("Registration response: " << response.dump(4) << std::endl);

                        int code = response["code"].get<int>();

                        if (code == 200) {
                            // registrado
                            g_usuarioInvitado = false;
                            g_usuarioLogueado = true;
                            g_userData = response["user"];
                            return "menuCompraEntradas";
                        }
                        else if (code == 409) {
                            // usuario ya existente
                            return gnu::menuMensajeAnticine("menuFormularioRegistro", "El correo electrónico ya está registrado!");
                        }
                        else {
                            // si llega aquí ya fue
                            // probablemente un colapso de la base de datos
                            // (si fuese un colapso de la api, simplemente crashearía)
                            LOG_FILE("Error inesperado al registrar usuario, probablemente un colapso de la base de datos, (si fuese un colapso de la api, simplemente crashearía)" << std::endl);
                            return gnu::menuMensajeAnticine("exit", "Ups! Algo salió mal, por favor intenta más tarde");
                        }
                    }
                    else if (button_focus_i == Buttons::YaTengoUnaCuenta) {
                        // Redirigir a formulario de registro
                        return "menuFormularioLogin";
                    }
                    else if (button_focus_i == Buttons::ContinuarComoInvitado) {
                        // Redirige al menús de compras, no es necesario loguearse,
                        // pero los datos de la compra igual serán pedidoss
                        g_usuarioInvitado = true;
                        g_usuarioLogueado = false;
                        return "menuCompraEntradas";
                    }
                    break;
                }
            }
        }

        /* ====== Drawing ====== */
        bool needToRedrawLayout = lastConsoleSize != gnu::getConsoleSize();
        if (firstDraw) {
            needToRedrawLayout = true;
            firstDraw = false;
        }

        if (needToRedrawLayout) {
            gnu::cls();
            // Poster;
            gnu::gotoY(4);
            style::setFg(ANTICINE_LOGO_COLOR);
            gnu::printRawCenter(gnu::anticineLogo);

        }
        // Actualizamos el estado de focus de cada input
        for (int i = 0; i < N_FIELDS; i++) {
            // Al cambiar el focus, se redibuja el input automáticamente
            registrationFields[i].setFocus(i == focus_index && menuOption == REGISTRO_OPCION_FORMULARIO);
            registrationFields[i].draw();
        }
        for (int i = 0; i < N_BUTTONS; i++) {
            buttons[i].setFocus(i == button_focus_i && menuOption == REGISTRO_OPCION_BOTONES);
            buttons[i].draw();
        }

        lastConsoleSize = gnu::getConsoleSize();
    }

    return "exit"; // luego del formulario, se debe redirigir al menú de compra
}
} // namespace gnu
