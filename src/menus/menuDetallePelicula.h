#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../services.h"
#include "../components/MovieCard.h"

#include "globales.h"

namespace gnu {

// Submenús
std::string submenuSeleccionarFecha();

#define MAX_SYNOPSIS_LENGTH 500
std::string getSynopsis(json movieData);

std::string menuDetalles() {
    gnu::cls();

    json movieRawData = gnu::apifetch("/cines/" + g_cineID + "/cartelera/" + g_movieID);
    json movieData = movieRawData["movie"];

    std::vector<json> castRaw = movieData["cast"].get<std::vector<json>>();
    std::vector<json> daysRaw = movieRawData["days"].get<std::vector<json>>();
    
    //all the session and versions data from the first (current) day
    json currentDayRaw = daysRaw[0];

    for (json obj : daysRaw) {
        if (obj["date"].get<std::string>() == g_selectedDate) {
            currentDayRaw = obj;
        }
    }
    if (g_selectedDate == "") {
        currentDayRaw = daysRaw[0];
    }

    std::string director;
    std::vector<std::string> cast;
    std::string title = movieData["title"].get<std::string>();
    std::string trailer_url = movieData["trailer_url"].get<std::string>();

    for (const json &obj : castRaw){
        if (obj["role"].get<std::string>() == "Director") {
            director = obj["fullname"].get<std::string>();
        }
        else {
            cast.push_back(obj["fullname"].get<std::string>());
        }
    }

    gnu::Box header({ gnu::getConsoleSize().x , 10 });
    header.showBorder = false;
    header.setBoxColor({ 184, 155, 231 });
    header.draw();

    // ======== titulo ============
    gnu::gotoXY({0, 7});
    style::setBg({184, 155, 231});
    style::setFg({0, 0, 0});
    style::bold();

    gnu::printRawOffset(R"(
 /||\ |~|~  |\/| /||\ |   /||\ ||\  ~|~|_|[~  | | | /|(`|)  /\| | /|~|~| ||\/| /||\ || /|
/-|| \| |   |  |/-|| \|  /-|| \||/   | | |[_   \|\|/-|_)|   \X|_|/-| | |_||  |/-|| \||/-|
)", 33);

    style::reset_bg();
    style::setDefaultFg();

    // ====== director ======
    gnu::gotoXY({33, 10});
    style::bold();
    style::italic();
    gnu::print("Dirigido por " + director);
    style::setDefaultFg();

    // ======= poster ======
    gnu::Box poster({30, 20});
    poster.setBoxColor({255, 255, 102});
    poster.position = gnu::vec2d({1, 8});
    poster.draw();

    // ====== cast ==========
    gnu::gotoXY({1, 30});
    style::bold();
    style::underline();
    gnu::print("REPARTO");
    style::setDefaultFg();

    // TODO: make it print in line until touching the border of the poster
    short i = 0;
    for (const std::string &actor : cast) {
        gnu::gotoXY({ 1, 32 + i });
        gnu::print(actor + "\n");
        i++;
    }

    // =========== synopsis =============== || TODO: colocarlo "en otro sitio"
    gnu::Box synopsis({ gnu::getConsoleSize().x - 31, 5 });
    synopsis.position = { 32, 10 };
    synopsis.showBorder = false;
    synopsis.content = getSynopsis(movieData);

    // ======= date selector =========
    gnu::Box date({14, 1});
    date.position = { short(getConsoleSize().x - 15), 8 };
    date.content = currentDayRaw["date"].get<std::string>();
    date.showBorder = false;
    date.flushBorders();
    date.transparent = true;
    date.draw();

    // ====== versions ======
    std::vector<json> versions = currentDayRaw["movie_versions"].get<std::vector<json>>();
    std::string currentDay = currentDayRaw["date"].get<std::string>();
    
    gnu::Box versionBoxTemplate({
        gnu::getConsoleSize().x - 35,
        3
    });

    std::vector<gnu::Box> versionBoxes(versions.size(), versionBoxTemplate);
    std::string versionTag;
    std::string langTag;

    //===== type of movie label =====
    gnu::Box typeSubBox({15, 1});

    //========= drawing the version boxes with their type labels =======
    for (size_t i = 0; i < versionBoxes.size(); i++){
        if (i == 0) versionBoxes[i].setBorderColor({230,50, 50});
        else versionBoxes[i].defaultBorderColor = true;

        versionBoxes[i].position = {
            34,
            int(14 + i * (versionBoxTemplate.size.y + 2))
        };
        versionBoxes[i].draw();

        //redrawing the type label
        versionTag = versions[i]["version_tags"].get<std::string>();
        langTag = versions[i]["language_tags"].get<std::string>();

        typeSubBox.content = versionTag + " | " + langTag;
        typeSubBox.position = versionBoxes[i].position;
        typeSubBox.draw();
    }

    //======= hours =======
    std::vector<std::string> hours;
    std::vector<size_t> hourSizes; // we need the sizes to limit the input

    // > sessions are presented as small selectable hour labes
    gnu::Box sessionTemplate({10, 1});
    std::vector<gnu::Box> sessionBoxes(3, sessionTemplate);

    sessionBoxes[0].setFontColor({ 80, 80, 80 });
    sessionBoxes[2].setFontColor({ 80, 80, 80 });

    int middleRelative = versionBoxes[0].position.x + (versionBoxes[0].size.x / 2);

    for (size_t j = 0; j < versionBoxes.size(); j++){    
        
        // first we want to store the hours so we can use them later on (including both the epmty ones for scrolling)
        hours.insert(hours.begin(), "");
        for (json session : versions[j]["sessions"]) {
            hours.push_back(session["hour"].get<std::string>());
        }
        hours.insert(hours.end(), "");

        hourSizes.push_back(hours.size());
        
        for (size_t i = 0; i < sessionBoxes.size(); i++){
            sessionBoxes[i].content = hours[i];
            sessionBoxes[i].showBorder = false;
            sessionBoxes[i].position.x = middleRelative - 25 + (20 * i);
            sessionBoxes[i].position.y = versionBoxes[j].position.y + 2;
            sessionBoxes[i].draw();
        }

        // we clean the hour vector for every box, this way we wont need vectors of vectors for the hours
        hours.clear();
    }


    int input;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    size_t hour_i = 0;

    unsigned char globalOpt = 2;
    unsigned char opt = 0;

    unsigned char lastGlobalOpt = 2;
    unsigned char lastOpt = 0;

    //======== main loop ==============
    while(true) {
        input = gnu::getch();

        //======= input switch ======
        switch (input) {
        case gnu::key::Right:
            if (hour_i < hourSizes[opt] - 3 && globalOpt == 2) hour_i++;
            break;
        case gnu::key::Left:
            if (hour_i > 0 && globalOpt == 2) hour_i--;
            break;
        case gnu::key::Up:
            if (opt > 0 && globalOpt == 2) {
                opt--;
                hour_i = 0;
                break;
            }

            if (opt == 0 && globalOpt > 0) globalOpt--;
            break;
        case gnu::key::Down:
            if (opt < versionBoxes.size() - 1 && globalOpt == 2) {
                opt++;
                hour_i = 0;
            }

            if (opt == 0 && globalOpt < 2) globalOpt++;
            break;
        case gnu::key::Enter:
            if (globalOpt == 1) return "submenuSeleccionarFecha";
            if (globalOpt == 0) system(std::string("start " + trailer_url).c_str());

            if (globalOpt == 2) {
                return "Asientos";
            }
        }

        for (size_t j = 0; j < versionBoxes.size(); j++){    
            
            //first we want to store the hours so we can use them later on (including both the epmty ones for scrolling)
            hours.insert(hours.begin(), "");
            for (json session : versions[j]["sessions"]) {
                hours.push_back(session["hour"].get<std::string>());
            }
            hours.insert(hours.end(), "");
            
            for (size_t i = 0; i < sessionBoxes.size(); i++){
                if (opt == j) sessionBoxes[i].content = hours[i + hour_i];
                else sessionBoxes[i].content = hours[i];
                
                sessionBoxes[i].showBorder = false;
                sessionBoxes[i].position.x = middleRelative - 25 + (20 * i);
                sessionBoxes[i].position.y = versionBoxes[j].position.y + 2;
                sessionBoxes[i].draw();
            }

            //we clean the hour vector for every box, this way we wont need vectors of vectors for the hours
            hours.clear();
        }


        if (lastConsoleSize != gnu::getConsoleSize() || lastOpt != opt || lastGlobalOpt != globalOpt){
            if (lastOpt == opt) gnu::cls();

            //======= drawing every session box on resize ===========
            for (size_t i = 0; i < versionBoxes.size(); i++){
                versionBoxes[i].position = gnu::vec2d({
                    34,
                    int(14 + i * (versionBoxTemplate.size.y + 2))
                });

                if (i == opt && globalOpt == 2) versionBoxes[i].setBorderColor({230,50, 50});
                else versionBoxes[i].defaultBorderColor = true;

                versionBoxes[i].draw();

                //redrawing the type label
                versionTag = versions[i]["version_tags"].get<std::string>();
                langTag = versions[i]["language_tags"].get<std::string>();

                typeSubBox.content = versionTag + " | " + langTag;
                typeSubBox.position = versionBoxes[i].position;
                typeSubBox.draw();
            }

            //===== redrawing =====
            header.size = gnu::vec2d({gnu::getConsoleSize().x , 10});
            header.draw();
            
            if (globalOpt == 1){
                date.showBorder = true;
                date.setBorderColor({230, 50, 50});
            }

            else {
                date.showBorder = false;
                date.flushBorders();
            }

            date.draw();
            
            if (globalOpt == 0) poster.setBoxColor({230, 50, 50});
            else poster.setBoxColor({255, 255, 102});

            poster.draw();

            //====== director ======
            gnu::gotoXY({33, 10});
            style::bold();
            style::italic();
            gnu::print("Dirigido por " + director);
            style::setDefaultFg();

            //====== cast ==========
            gnu::gotoXY({1, 30});
            style::bold();
            style::underline();
            gnu::print("REPARTO");
            style::setDefaultFg();
        
            //TODO: make it print in line until touching the border of the poster
            short i = 0;
            for (std::string actor : cast) {
                gnu::gotoXY({1, 32 + i});
                gnu::print(actor + "\n");
                i++;
            }

            //======== titulo ============
            gnu::gotoXY({0, 7});
            style::setBg({184, 155, 231});
            style::setFg({0, 0, 0});
            style::bold();
            gnu::printRawOffset(R"(
 /||\ |~|~  |\/| /||\ |   /||\ ||\  ~|~|_|[~  | | | /|(`|)  /\| | /|~|~| ||\/| /||\ || /|
/-|| \| |   |  |/-|| \|  /-|| \||/   | | |[_   \|\|/-|_)|   \X|_|/-| | |_||  |/-|| \||/-|
)", 33);
            style::reset_bg();
            style::setDefaultFg();
        }

        lastConsoleSize = gnu::getConsoleSize();
        lastOpt = opt;
        lastGlobalOpt = globalOpt;

        gnu::sleep(5);
    }

    return "hola";
}

/* --- Submenús --- */

std::string submenuSeleccionarFecha() {
    gnu::cls();
    std::string rawMessage = R"(                      
         _             _
 ___ ___| |___ ___ ___|_|___ ___ ___
|_ -| -_| | -_|  _|  _| | . |   | .'|
|___|___|_|___|_______|_|_____|_|__,|
                ____        _
 _ _ ___ ___   |  __|__ ___| |_ ___
| | |   | .'|  |  _| -_|  _|   | .'|
|___|_|_|__,|  |_| |___|___|_|_|__,|
)";

    gnu::printRawCenter(rawMessage);
    gnu::gotoY(15);

    json wholeRaw = gnu::apifetch("/cines/2705/cartelera/91074");
    std::vector<json> daysRaw = wholeRaw["days"].get<std::vector<json>>();
    
    std::vector<std::string> dates;

    for (json obj : daysRaw) dates.push_back(obj["date"].get<std::string>());
    dates.insert(dates.begin(), "");
    dates.insert(dates.end(), "");

    gnu::Box dateContainer({gnu::getConsoleSize().x - 4, 1});
    
    std::vector<gnu::Box> genericDateContainers(3, dateContainer);

    for (size_t i = 0; i < genericDateContainers.size(); i++) {
        genericDateContainers[i].content = dates[i];
        genericDateContainers[i].position = gnu::vec2d({ 0, int(15 + 3 * i)});
        genericDateContainers[i].showBorder = false;
        genericDateContainers[i].draw();
    }

    genericDateContainers[1].setFontColor({ 230, 50, 50 });
    genericDateContainers[1].showBorder = true;

    int input;
    unsigned char opt = 0;

    while (true) {
        input = gnu::getch();

        switch (input) {
        case gnu::key::Up:
            if (opt > 0) opt--;
            break;
        case gnu::key::Down:
            if (opt < dates.size() - 3) opt++;
            break;
        case gnu::key::Enter:
            g_selectedDate = dates[1 + opt];
            return "menuDetalles";
        }

        for (size_t i = 0; i < genericDateContainers.size(); i++) {
            genericDateContainers[i].content = dates[i + opt];
            genericDateContainers[i].position = gnu::vec2d({ 0, int(15 + 3 * i)});
            genericDateContainers[i].showBorder = false;
            genericDateContainers[i].draw();
        }
    }
}

// Obtiene la sinopsis de la película y la acorta si es necesario
std::string getSynopsis(json movieData) {
    std::string synopsis = movieData["synopsis"].get<std::string>();
    if (synopsis.length() > MAX_SYNOPSIS_LENGTH) {
        synopsis = synopsis.substr(0, MAX_SYNOPSIS_LENGTH);
        synopsis += "...";
    }
    return synopsis;
}

} // namespace gnu
