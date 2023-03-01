#pragma once

#include "../components/Input.h"
#include "../components/Button.h"
#include "../consoleUtils.h"
#include "../menus/menuDetallePelicula.h"
#include "../menus/menuMensajeAnticine.h"

#include "globales.h"

namespace gnu {

#define MENUCOMPRA_OPCION_FORMULARIO 0
#define MENUCOMPRA_OPCION_BOTONES 1

#define MENUCOMPRA_HEADER_OFFSET_Y 14
#define MENUCOMPRA_THUMBNAIL_OFFSET_X 20

using json = nlohmann::json;

std::string menuCompraEntradas() {
    gnu::cls();

    // Fields creadas en un array para acceder a posiciones dinámicamente
    int focus_index = 0;
    constexpr int N_FIELDS = 4;
    gnu::Input registrationFields[] = {
        gnu::Input("Nombre en tarjeta", 39),
        gnu::Input("Numero de tarjeta", 16 + 3), // 19 + 6 + 3 + (padding: 6) = 30
        gnu::Input("Vence", 5 + 1),  // formato MM/AA
        gnu::Input("CCV", 3)
    };
    constexpr int N_BUTTONS = 2;
    gnu::Button buttons[] = {
        gnu::Button("Confirmar compra", { 30, 1 }),
        gnu::Button("Cancelar", { 30, 1 })
    };

    enum Buttons : int {
        ConfirmarCompra,
        Cancelar,
    };
    enum Fields : int {
        NombreEnTarjeta,
        NumeroDeTarjeta,
        FechaVencimiento,
        CCV
    };

    // Posicionamos los inputs relativamente
    registrationFields[NombreEnTarjeta].position.y = MENUCOMPRA_HEADER_OFFSET_Y;
    registrationFields[NombreEnTarjeta].centerHorizontal();
    registrationFields[NombreEnTarjeta].position.x += MENUCOMPRA_THUMBNAIL_OFFSET_X;
    registrationFields[NumeroDeTarjeta].setPosition({ registrationFields[0].position.x, registrationFields[0].position.y + 5 });
    registrationFields[FechaVencimiento].setPosition({ registrationFields[0].position.x + 25, registrationFields[1].position.y });
    registrationFields[CCV].setPosition({ registrationFields[2].position.x + 11, registrationFields[1].position.y });
    buttons[ConfirmarCompra].position.y = registrationFields[1].position.y + 5;
    buttons[Cancelar].position.y = buttons[Buttons::ConfirmarCompra].position.y + 3;
    buttons[ConfirmarCompra].centerHorizontal();
    buttons[ConfirmarCompra].position.x += MENUCOMPRA_THUMBNAIL_OFFSET_X;
    buttons[Cancelar].centerHorizontal();
    buttons[Cancelar].position.x += MENUCOMPRA_THUMBNAIL_OFFSET_X;

    int menuOption = MENUCOMPRA_OPCION_FORMULARIO;
    int button_focus_i = 0;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    bool firstDraw = true;
    while (true) {
        int input = gnu::getch();
        if (input) {
            if (menuOption == MENUCOMPRA_OPCION_FORMULARIO) {
                gnu::Input::handleInfo result = registrationFields[focus_index].handle(input);
                if (result.wanna_go_previous) {
                    if (focus_index > 0) {
                        focus_index--;
                    }
                    else if (menuOption == MENUCOMPRA_OPCION_BOTONES) {
                        menuOption = MENUCOMPRA_OPCION_FORMULARIO;
                    }
                }
                if (result.wanna_go_next) {
                    if (focus_index + 1 < N_FIELDS) {
                        focus_index++;
                    }
                    else {
                        menuOption = MENUCOMPRA_OPCION_BOTONES;
                    }
                }
            }
            else switch (input) {
                case gnu::key::Up: {
                    if (button_focus_i > 0) button_focus_i--;
                    else menuOption = MENUCOMPRA_OPCION_FORMULARIO; // volver al formulario
                    break;
                }
                case gnu::key::Down: {
                    if (button_focus_i + 1 < N_BUTTONS) button_focus_i++;
                    break;
                }
                case gnu::key::Enter: {
                    if (button_focus_i == Buttons::ConfirmarCompra) {
                        // Verificar los datos en la db y redirigir a menú de compra
                        /*
                            interface TicketPurchaseBody {
                                email: string,
                                session_id: string,
                                is_guest: boolean,
                                seats: SeatPosition[],
                                credit_card: CreditCardInformation
                            }

                            type SeatPosition = {
                                row_number: number,
                                col_number: number
                            }

                            type CreditCardInformation = {
                                number: string,
                                holder: string,
                                expiration_date: string,
                                cvv: string
                            }
                        */
                       LOG_FILE("user data: " << g_userData.dump(4) << std::endl);
                        json purchaseData = {
                            { "email", !g_usuarioInvitado ? g_userData["email"].get<std::string>() : "" },
                            { "session_id", g_sessionID },
                            { "is_guest", g_usuarioInvitado },
                            { "seats", g_selectedSeats },
                            { "credit_card", {
                                { "name", registrationFields[NombreEnTarjeta].text },
                                { "number", registrationFields[NumeroDeTarjeta].text },
                                { "expiration_date", registrationFields[FechaVencimiento].text },
                                { "cvv", registrationFields[CCV].text }
                            } }
                        };

                        LOG_FILE("compra: purchaseData: " << purchaseData.dump(4) << std::endl);

                        std::string ticketsURI = "/sessions/" + g_sessionID + "/reserve";
                        json response = gnu::apipost(ticketsURI, purchaseData);

                        int code = response["code"].get<int>();

                        if (code == 200) {
                            return gnu::menuMensajeAnticine("menuCompraEntradas", "Gracias por su compra!");
                        }
                        else {
                            return gnu::menuMensajeAnticine("menuCompraEntradas", "Ups! Algo salió mal. Por favor intente de nuevo.");
                        }

                        return "exit";
                    }
                    else if (button_focus_i == Buttons::Cancelar) {
                        // Redirigir al menu detalle de la pelicula
                        return "menuDetalles";
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
            registrationFields[i].setFocus(i == focus_index && menuOption == MENUCOMPRA_OPCION_FORMULARIO);
            registrationFields[i].draw();
        }
        for (int i = 0; i < N_BUTTONS; i++) {
            buttons[i].setFocus(i == button_focus_i && menuOption == MENUCOMPRA_OPCION_BOTONES);
            buttons[i].draw();
        }

        lastConsoleSize = gnu::getConsoleSize();
    }

    return "exit"; // luego del formulario, se debe redirigir al menú de compra
}
} // namespace gnu
