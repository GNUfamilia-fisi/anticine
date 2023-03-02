#pragma once

#include <iomanip>
#include <sstream>
#include <utf8.hpp>
#include <style.hpp>

#include "../components/Input.h"
#include "../components/Button.h"
#include "../consoleUtils.h"
#include "../menus/menuDetallePelicula.h"
#include "../menus/menuMensajeAnticine.h"

#include "globales.h"

namespace gnu {

#define MENUCOMPRA_OPCION_FORMULARIO 0
#define MENUCOMPRA_OPCION_BOTONES 1

#define MENUCOMPRA_HEADER_OFFSET_Y 24

#define PRECIO_ENTRADA 11.99f // (PEN) solo para pruebas

#define COLOR_SECUNDARIO_TEXTO style::rgb({ 111, 107, 138 })

using json = nlohmann::json;

std::string mapNumberToChar(int num);
std::string convertSeatsVecToStr(const std::vector<json> &seats);

std::string menuCompraEntradas() {
    gnu::cls();
    // hardcoded thumbnail size:
    constexpr int thumbnailWidth = 45;
    constexpr int thumbnailHeight = 30;

    // ======== Títulos imprimibles ========
    std::string movieMultilineTitle = parseStringToASCIIArtText(g_movieData["title"].get<std::string>());
    std::string completaTuCompraTitle = parseStringToASCIIArtText("Completa tu compra!");

    // Thumbnail de la película como colored ANSI art
    std::string movieRawThumbnail = g_movieData["raw_thumbnail_image"].get<std::string>();
    style::rgb averageThumbnailColor = {
        g_movieData["average_thumbnail_color"]["r"].get<unsigned char>(),
        g_movieData["average_thumbnail_color"]["g"].get<unsigned char>(),
        g_movieData["average_thumbnail_color"]["b"].get<unsigned char>()
    };
    style::rgb complementaryThumbnailColor = style::getComplementaryColor(averageThumbnailColor);

    // ========= Header =========
    gnu::Box header({ gnu::getConsoleSize().x , DETAIL_HEADER_HEIGHT });
    header.showBorder = false;
    header.setBoxColor(averageThumbnailColor);

    // == Variables para guardar datos NO DINÁMICOS de la compra ==
    std::string butacaField = g_selectedSeats.size() == 1 ? " butaca 🎟️  🪑 : " : " butacas 🎟️  🪑 : ";
    std::string sillasCompradasStr = "X " + std::to_string(g_selectedSeats.size()) + butacaField + convertSeatsVecToStr(g_selectedSeats);

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
                            Structura de la API:

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
                        std::string ticketsURI = "/sessions/" + g_sessionID + "/reserve";
                        json response = gnu::apipost(ticketsURI, purchaseData);

                        int code = response["code"].get<int>();

                        if (code == 200) {
                            return gnu::menuMensajeAnticine("menuCartelera", "Gracias por su compra 😻 !");
                        }
                        else if (code == 410) {
                            std::string message = response["message"].get<std::string>();
                            return gnu::menuMensajeAnticine("menuCompraEntradas", "Lo sentimos, " + message + " 😿");
                        }
                        else {
                            return gnu::menuMensajeAnticine("menuCompraEntradas", "Ups! Algo salió mal 😿. Por favor intente de nuevo más tarde.");
                        }

                        return "exit";
                    }
                    else if (button_focus_i == Buttons::Cancelar) {
                        // Redirigir al menu detalle de la pelicula
                        return "menuCartelera";
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
            // ======= Dibujando el header =======
            header.size = gnu::vec2d({gnu::getConsoleSize().x , 10});
            header.draw();

            // Poster
            gnu::gotoXY({ 0, MOVIE_TITLE_OFFSET_Y - 1 });
            gnu::printRawOffset(movieRawThumbnail, 2);

            registrationFields[NombreEnTarjeta].position.y = MENUCOMPRA_HEADER_OFFSET_Y;
            registrationFields[NombreEnTarjeta].centerHorizontal();
            registrationFields[NombreEnTarjeta].position.x += thumbnailWidth / 2;
            registrationFields[NumeroDeTarjeta].setPosition({ registrationFields[0].position.x, registrationFields[0].position.y + 5 });
            registrationFields[FechaVencimiento].setPosition({ registrationFields[0].position.x + 25, registrationFields[1].position.y });
            registrationFields[CCV].setPosition({ registrationFields[2].position.x + 11, registrationFields[1].position.y });
            buttons[ConfirmarCompra].position.y = registrationFields[1].position.y + 7;
            buttons[Cancelar].position.y = buttons[Buttons::ConfirmarCompra].position.y + 3;
            buttons[ConfirmarCompra].centerHorizontal();
            buttons[ConfirmarCompra].position.x += thumbnailWidth / 2;
            buttons[Cancelar].centerHorizontal();
            buttons[Cancelar].position.x += thumbnailWidth / 2;

            gnu::gotoXY({ 0, MOVIE_TITLE_OFFSET_Y });
            style::setBg(averageThumbnailColor);
            style::setFg(complementaryThumbnailColor);
            style::bold();
            // Ascii art
            gnu::printRawOffset(movieMultilineTitle, thumbnailWidth + 5);
            style::reset_bg();
            style::setDefaultFg();

            #define GET_OFFSET_X(field) (thumbnailWidth + ((gnu::getConsoleSize().x - thumbnailWidth) / 2) - (utf8::str_length(field) / 2))
            #define GET_MULTILINE_OFFSET_X(field) (thumbnailWidth + ((gnu::getConsoleSize().x - thumbnailWidth) / 2) - ((utf8::str_length(field) - 2) / 4))

            // print subtitle "Completa tu compra!"
            gnu::gotoXY({ 0, MOVIE_TITLE_OFFSET_Y + 6 });
            style::setFg(COLOR_SECUNDARIO_TEXTO);
            style::bold();
            gnu::printRawOffset(completaTuCompraTitle, GET_MULTILINE_OFFSET_X(completaTuCompraTitle));
            style::reset();

            // Print sillas seleccionadas
            gnu::print("\n\n");
            float total_price = (float)g_selectedSeats.size() * PRECIO_ENTRADA;
            std::stringstream price_str;
            price_str << "S/ " << std::fixed << std::setprecision(3) << total_price;
            std::string priceASCIIArt = gnu::parseStringToASCIIArtText(price_str.str());
            gnu::printRawOffset(priceASCIIArt, GET_MULTILINE_OFFSET_X(priceASCIIArt));

            // == Lista de sillas seleccionadas ==
            gnu::print("\n\n");
            style::bold();
            gnu::printRawOffset(sillasCompradasStr, GET_OFFSET_X(sillasCompradasStr));
            style::bold_off();

            // === Detalles adicionales de la compra ===
            gnu::gotoXY({ 3, thumbnailHeight + DETAIL_HEADER_HEIGHT - 3 });
            style::bold();
            style::underline();
            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("Detalles de la compra\n\n");
            style::setDefaultFg();
            style::underline_off();
            style::bold_off();

            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("   Cine y dirección:\n");
            style::setDefaultFg();
            gnu::print("   " + g_cineData["name"].get<std::string>() + "\n");
            gnu::print("   " + g_cineData["address"].get<std::string>() + "\n\n");

            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("   Película: ");
            style::setDefaultFg();
            gnu::print(g_movieData["title"].get<std::string>() + "\n");
            gnu::print("             " + g_movieVersion["version_tags"].get<std::string>() + " | "
                                       + g_movieVersion["language_tags"].get<std::string>() + " | "
                                       + g_movieData["rating"].get<std::string>() + "\n\n"
            );

            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("   Tickets generales: ");
            style::setDefaultFg();
            gnu::print(std::to_string(g_selectedSeats.size()) + " 🎟️\n\n");

            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("   Fecha: ");
            style::setDefaultFg();
            gnu::print(g_sessionData["day"].get<std::string>() + "\n");
            style::setFg(COLOR_SECUNDARIO_TEXTO);
            gnu::print("   Hora:  ");
            style::setDefaultFg();
            gnu::print(g_sessionData["hour"].get<std::string>() + "\n\n");
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

std::string mapNumberToChar(int num) {
    switch (num) {
        case 0: return "A";
        case 1: return "B";
        case 2: return "C";
        case 3: return "D";
        case 4: return "E";
        case 5: return "F";
        case 6: return "G";
        case 7: return "H";
        case 8: return "I";
        case 9: return "J";
        case 10: return "K";
        case 11: return "L";
        case 12: return "M";
        case 13: return "N";
        default: return " ";
    }
}

#define MAXIMA_SILLAS 20

std::string convertSeatsVecToStr(const std::vector<json> &seats) {
    std::string to_return = "[ ";
    size_t len = seats.size() > MAXIMA_SILLAS ? MAXIMA_SILLAS : seats.size();

    for (size_t i = 0; i < len; i++) {
        json seat = seats[i];
        std::string name = mapNumberToChar(seat["row_number"].get<int>()) + std::to_string(seat["col_number"].get<int>());
        to_return += name + (i < len - 1 ? ", " : "");
    }
    if (seats.size() > MAXIMA_SILLAS) {
        to_return += " ...";
    }
    return to_return + " ]";
}

} // namespace gnu
