#pragma once

#include "../components/Input.h"
#include "../components/Button.h"
#include "../consoleUtils.h"
#include "../menus/menuDetallePelicula.h"
#include "../menus/menuMensajeAnticine.h"

#include "globales.h"

namespace gnu {

#define LOGIN_OPCION_FORMULARIO 0
#define LOGIN_OPCION_BOTONES 1
#define INICIO_SESION_HEADER_OFFSET_Y 17

using json = nlohmann::json;

std::string menuFormularioLogin() {
    gnu::cls();

    // Fields creadas en un array para acceder a posiciones dinámicamente
    int focus_index = 0;
    constexpr int N_FIELDS = 2;
    gnu::Input registrationFields[] = {
        gnu::Input("Correo electrónico", 30),
        gnu::Input("Contraseña", 30)
    };
    constexpr int N_BUTTONS = 3;
    gnu::Button buttons[] = {
        gnu::Button("Ingresar", { 30, 1 }),
        gnu::Button("No tengo una cuenta", { 30, 1 }),
        gnu::Button("Continuar como invitado", { 30, 1 })
    };

    enum Buttons : int {
        Ingresar,
        NoTengoUnaCuenta,
        ContinuarComoInvitado
    };

    // Posicionamos los inputs relativamente
    registrationFields[0].position.y = INICIO_SESION_HEADER_OFFSET_Y;
    registrationFields[0].centerHorizontal();
    registrationFields[1].setPosition({ registrationFields[0].position.x, registrationFields[0].position.y + 5 });
    buttons[Ingresar].position.y = registrationFields[1].position.y + 5;
    buttons[NoTengoUnaCuenta].position.y = buttons[Buttons::Ingresar].position.y + 3;
    buttons[ContinuarComoInvitado].position.y = buttons[Buttons::NoTengoUnaCuenta].position.y + 3;
    buttons[Ingresar].centerHorizontal();
    buttons[NoTengoUnaCuenta].centerHorizontal();
    buttons[ContinuarComoInvitado].centerHorizontal();

    int menuOption = LOGIN_OPCION_FORMULARIO;
    int button_focus_i = 0;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    bool firstDraw = true;
    while (true) {
        int input = gnu::getch();
        if (input) {
            if (menuOption == LOGIN_OPCION_FORMULARIO) {
                gnu::Input::handleInfo result = registrationFields[focus_index].handle(input);
                if (result.wanna_go_previous) {
                    if (focus_index > 0) {
                        focus_index--;
                    }
                    else if (menuOption == LOGIN_OPCION_BOTONES) {
                        menuOption = LOGIN_OPCION_FORMULARIO;
                    }
                }
                if (result.wanna_go_next) {
                    if (focus_index + 1 < N_FIELDS) {
                        focus_index++;
                    }
                    else {
                        menuOption = LOGIN_OPCION_BOTONES;
                    }
                }
            }
            else switch (input) {
                case gnu::key::Up: {
                    if (button_focus_i > 0) button_focus_i--;
                    else menuOption = LOGIN_OPCION_FORMULARIO; // volver al formulario
                    break;
                }
                case gnu::key::Down: {
                    if (button_focus_i + 1 < N_BUTTONS) button_focus_i++;
                    break;
                }
                case gnu::key::Enter: {
                    if (button_focus_i == Buttons::Ingresar) {
                        // No implementado
                        // Debería verificar los datos en la db y redirigir a menú de compra
                        json loginData = {
                            {"email", registrationFields[0].text},
                            {"password", registrationFields[1].text}
                        };

                        json response = gnu::apipost("/login", loginData);

                        int code = response["code"].get<int>();

                        if (code == 200) {
                            g_usuarioLogueado = true;
                            g_usuarioInvitado = false;
                            g_userData = response["user"];
                            return "menuCompraEntradas";
                        }
                        else {
                            return gnu::menuMensajeAnticine("menuFormularioLogin", "Usuario o contraseña incorrectos");
                        }
                    }
                    else if (button_focus_i == Buttons::NoTengoUnaCuenta) {
                        // Redirigir a formulario de registro
                        return "menuFormularioRegistro";
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
            gnu::gotoY(3);
            style::bold();
            style::setFg(ANTICINE_LOGO_COLOR);
            gnu::printRawCenter(gnu::anticineLogo);
            gnu::print("\n\n");
            style::bold_off();
            gnu::printRawCenter(gnu::parseStringToASCIIArtText("Inicia sesion en ANTICINE"));

        }
        // Actualizamos el estado de focus de cada input
        for (int i = 0; i < N_FIELDS; i++) {
            // Al cambiar el focus, se redibuja el input automáticamente
            registrationFields[i].setFocus(i == focus_index && menuOption == LOGIN_OPCION_FORMULARIO);
            registrationFields[i].draw();
        }
        for (int i = 0; i < N_BUTTONS; i++) {
            buttons[i].setFocus(i == button_focus_i && menuOption == LOGIN_OPCION_BOTONES);
            buttons[i].draw();
        }

        lastConsoleSize = gnu::getConsoleSize();
    }

    return "exit"; // luego del formulario, se debe redirigir al menú de compra
}
} // namespace gnu
