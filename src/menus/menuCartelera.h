#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../services.h"
#include "../components/MovieCard.h"
#include "../components/Button.h"

#include "menuDetallePelicula.h" // getSynopsis()

#include "globales.h"

namespace gnu {

std::string menuCartelera() {
    gnu::cls();

    json rawCarteleraData = gnu::apifetch("/cines/" + g_cineID + "/cartelera");
    std::vector<json> billboard = rawCarteleraData["movies"].get<std::vector<json>>();

    json emptyMovie = json::parse(R"({
        "title": "",
        "thumbnail_url": "",
        "average_thumbnail_color": {
            "r": 0,
            "g": 0,
            "b": 0
        },
        "duration": 0,
        "rating": "",
        "corporate_film_id": "",
        "trailer_url": "",
        "synopsis": ""
    })");
    billboard.insert(billboard.begin(), emptyMovie);
    billboard.insert(billboard.end(), emptyMovie);

    gnu::MovieCard card1(billboard[0], { 18, 15 });
    gnu::MovieCard card2(billboard[1], { 20, 17 });
    gnu::MovieCard card3(billboard[2], { 18, 15 });
    card1.position.y = 14;
    card2.position.y = 13;
    card3.position.y = 14;

    //============== descripcion ===================
    gnu::Box descriptionCard({ 86, 11 });
    descriptionCard.content = getSynopsis(billboard[1]);
    descriptionCard.defaultFontColor = true;
    descriptionCard.position.y = 38;
    descriptionCard.transparent = true;
    descriptionCard.showBorder = false;

    //====== box label for the current cinema we are in ==========
    std::string cinemaName;
    json cinemasListRaw = gnu::apifetch("/cines/cercanos");

    std::vector<json> cinemasList = cinemasListRaw["cinemas"].get<std::vector<json>>();

    for (int i = 0; i < cinemasList; i++){
        if (cinemasList[i]["cinema_id"].get<std::string>() == g_cineID) {
            cinemaName = cinemasList[i]["name"].get<std::string>();
            break;
        }
    }

    std::string cinemaLabel = "Estas en " + cinemaName;
    gnu::Button cinemaLabelBox(cinemaLabel);
    cinemaLabelBox.position.y = 9;
    cinemaLabelBox.size.y = 1;

    //=========== button indicators =================
    gnu::Box buttonLeft({ 6, gnu::getConsoleSize().y });
    buttonLeft.setBoxColor({ 35, 35, 35 });
    buttonLeft.position.x = 0;
    buttonLeft.content = "<-";
    buttonLeft.showBorder = false;
    buttonLeft.setFontColor({ 255,255,255 });

    gnu::Box buttonRight({ 6, gnu::getConsoleSize().y });
    buttonRight.setBoxColor({ 35, 35, 35 });
    buttonRight.position.x = gnu::getConsoleSize().x - 8;
    buttonRight.content = "->";
    buttonRight.showBorder = false;
    buttonRight.setFontColor({ 255,255,255 });

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    std::string titleLabel = billboard[1]["title"].get<std::string>();
    std::string lastLabel = titleLabel;
    std::string titlePadding = "";

    //====== additionals ========
    std::string duration = std::to_string(billboard[1]["duration"].get<int>());
    std::string rating = billboard[1]["rating"].get<std::string>();
    std::string additionals = duration + " | " + rating;
    std::string lastAdditionals = additionals;
    std::string additionalsPadding = "";

    char opt = 1;
    size_t panel_i = 1;
    int input;

    while (true) {
        input = gnu::getch();

        if (input) switch(input){
            case gnu::key::Right: {
                if (panel_i < billboard.size() - 3 && opt == 1) panel_i++;
                break;
            }
            case gnu::key::Left: {
                if (panel_i > 0 && opt == 1) panel_i--;
                break;
            }
            case gnu::key::Up: {
                if (opt > 0) opt--;
                break;
            }
            case gnu::key::Down: {
                if (opt < 1) opt++;
                break;
            }
            case gnu::key::Enter: {
                if (opt == 1) {
                    // g_movieData ahora se guarda en menuDetallePelicula.h
                    // g_movieData = billboard[1 + panel_i];
                    g_movieID = billboard[1 + panel_i]["corporate_film_id"].get<std::string>();
                    return "menuDetalles";
                }
                else {
                    return "selectorCineAux";
                }
            }
        }

        card1.updateData(billboard[0 + panel_i]);
        card2.updateData(billboard[1 + panel_i]);
        card3.updateData(billboard[2 + panel_i]);

        descriptionCard.content = getSynopsis(billboard[1 + panel_i]);
        descriptionCard.centerHorizontal();

        // ---- Update ----
        style::setFg(ANTICINE_LOGO_COLOR);
        gnu::gotoY(2);
        style::bold();
        gnu::printRawCenter(gnu::anticineLogo);
        style::bold_off();
        style::setFg({ 222, 197, 153 });

        card2.centerHorizontal();
        card1.setPosition( card2.position + gnu::vec2d{ (short)(-card1.size.x - 10), 1 } );
        card3.setPosition( card2.position + gnu::vec2d{ (short)(card2.size.x + 10), 1 } );

        card1.showBorder = true;
        card2.showBorder = true;
        card3.showBorder = true;

        card1.cardDraw();
        card2.cardDraw();
        card3.cardDraw();

        gnu::gotoY(card2.position.y + card2.size.y + 2 + 1);
        titleLabel = billboard[1 + panel_i]["title"].get<std::string>();

        if (lastLabel.size() <= titleLabel.size()) {
            gnu::printLineCentered(titleLabel);
        }
        else {
            for (size_t i = 0; i < (lastLabel.size() - titleLabel.size() + 1) / 2; i++){
                titlePadding += " ";
            }
            gnu::printLineCentered(titlePadding + titleLabel + titlePadding);
        }
        lastLabel = titleLabel;

        gnu::print("\n\n");
        // emojis
        std::string emojisRaw = billboard[1 + panel_i]["emojis"].get<std::string>();

        utf8::utf8_string_t emojis = utf8::iterate(emojisRaw);

        gnu::gotoX(gnu::getConsoleSize().x / 2 - 5);

        // TODO: agregar soporte para emojis compuestos de varios codepoints
        // "emoji zwj sequences"
        // Referencia: https://unicode.org/emoji/charts/emoji-zwj-sequences.html
        for (std::string h : emojis) {
            // if (h == std::string("♂️")) continue;
            gnu::print(h);
        }

        gnu::print("\n\n");

        //additional data
        gnu::printLineCentered(additionals);

        duration = std::to_string(billboard[1 + panel_i]["duration"].get<int>());
        rating = billboard[1 + panel_i]["rating"].get<std::string>();
        additionals = duration + " | " + rating;

        if (lastAdditionals.size() <= additionals.size()) {
            gnu::printLineCentered(additionals);
        }
        else {
            for (size_t i = 0; i < (lastAdditionals.size() - additionals.size() + 1) / 2; i++){
                additionalsPadding += " ";
            }
            gnu::printLineCentered(additionalsPadding + additionals + additionalsPadding);
        }
        lastAdditionals = additionals;

        descriptionCard.draw();

        buttonLeft.size.y = gnu::getConsoleSize().y;
        buttonRight.size.y = gnu::getConsoleSize().y;
        buttonRight.position.x = gnu::getConsoleSize().x - 6;

        // TODO: no sé qué quiere panda diestro pero a cada rato para poniendo "TODO's" sin contexto alguno
        cinemaLabelBox.centerHorizontal();
        cinemaLabelBox.content = cinemaLabel;
        cinemaLabelBox.size.x = cinemaLabel.size() + 4;

        // Seleccionado el botón de cines
        if (opt == 0) {
            buttonRight.content = "";
            buttonLeft.content = "";
            buttonRight.transparent = true;
            buttonLeft.transparent = true;

            cinemaLabelBox.focus();
        }
        else {
            buttonRight.content = ">>";
            buttonLeft.content = "<<";
            buttonRight.transparent = false;
            buttonLeft.transparent = false;

            cinemaLabelBox.unfocus();
        }

        cinemaLabelBox.draw();
        buttonLeft.draw();
        buttonRight.draw();

        // ----- Clear ----
        gnu::gotoXY(0, 0);
        if (gnu::getConsoleSize() != lastConsoleSize) {
            gnu::cls();
        }

        lastConsoleSize = gnu::getConsoleSize();
        titlePadding = "";
        additionalsPadding = "";

        gnu::sleep(5);
    }
}

} // namespace gnu
