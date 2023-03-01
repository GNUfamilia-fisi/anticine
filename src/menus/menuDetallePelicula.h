#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../services.h"
#include "../components/MovieCard.h"

#include "globales.h"

namespace gnu {

#define DETAIL_HEADER_HEIGHT 10
#define MOVIE_TITLE_OFFSET_Y 7

#define OPTION_VIEW_MOVIE_TRAILER 0
#define OPTION_SELECT_OTHER_DATE 1
#define OPTION_SELECT_SESSION 2

// Submenús
std::string submenuSeleccionarFecha();
std::string formatTime(std::string minutes);
std::string parseStringToASCIIArtText(std::string text);

#define MAX_SYNOPSIS_LENGTH 500
std::string getSynopsis(json movieData);

std::string menuDetalles() {
    gnu::cls();
    // hardcoded thumbnail size:
    int thumbnailWidth = 45;
    int thumbnailHeight = 30;

    // Obtenemos datos de la película
    json movieResponse = gnu::apifetch("/cines/" + g_cineID + "/cartelera/" + g_movieID);
    std::vector<json> movieDaysData = movieResponse["days"].get<std::vector<json>>();
    json movieData                  = movieResponse["movie"];

    // Reparto de la película
    std::vector<json> castData = movieData["cast"].get<std::vector<json>>();
    // Título de la película como ASCII art
    std::string movieMultilineTitle = parseStringToASCIIArtText(movieData["title"].get<std::string>());
    // Thumbnail de la película como colored ANSI art
    std::string movieRawThumbnail = movieData["raw_thumbnail_image"].get<std::string>();
    style::rgb averageThumbnailColor = {
        movieData["average_thumbnail_color"]["r"].get<unsigned char>(),
        movieData["average_thumbnail_color"]["g"].get<unsigned char>(),
        movieData["average_thumbnail_color"]["b"].get<unsigned char>()
    };
    style::rgb complementaryThumbnailColor = getComplementaryColor(averageThumbnailColor);

    // Las sesiones están separadas por días. Por defecto se toma el primer día
    json currentDayData = movieDaysData[0];
    for (json day : movieDaysData) {
        if (day["date"].get<std::string>() == g_selectedDate) {
            currentDayData = day;
            break;
        }
    }

    // Variables para guardar datos NO DINÁMICOS de la película
    std::string movieDirector;
    std::vector<std::string> movieCast;
    std::string title = movieData["title"].get<std::string>();
    std::string trailer_url = movieData["trailer_url"].get<std::string>();
    std::string movieRating = movieData["rating"].get<std::string>();
    int rawDuration = movieData["duration"].get<int>();
    std::string movieDuration = formatTime(std::to_string(rawDuration));

    for (const json &obj : castData){
        if (obj["role"].get<std::string>() == "Director") {
            movieDirector = obj["fullname"].get<std::string>();
        }
        else {
            movieCast.push_back(obj["fullname"].get<std::string>());
        }
    }

    gnu::Box header({ gnu::getConsoleSize().x , DETAIL_HEADER_HEIGHT });
    header.showBorder = false;
    header.setBoxColor(averageThumbnailColor);

    style::reset_bg();
    style::setDefaultFg();

    // =========== synopsis =============== || TODO: colocarlo
    gnu::Box synopsis({ gnu::getConsoleSize().x - 31, 5 });
    synopsis.position = { 32, 10 };
    synopsis.showBorder = false;
    synopsis.content = getSynopsis(movieData);

    // ======= date selector =========
    gnu::Box date({ 14, 1 });
    date.position = { short(getConsoleSize().x - 16), DETAIL_HEADER_HEIGHT - 3 };
    date.content = currentDayData["date"].get<std::string>();
    date.showBorder = false;
    date.setBoxColor(averageThumbnailColor);
    date.setBorderColor(averageThumbnailColor);
    date.draw();

    // ====== versions ======
    std::vector<json> movieVersions = currentDayData["movie_versions"].get<std::vector<json>>();
    std::string currentDay = date.content;

    // Las version boxes van al lado del thumbnail, y necesitan algo de padding
    gnu::Box versionBoxTemplate({
        gnu::getConsoleSize().x - (thumbnailWidth + 6 + 2), // 6 de padding, 2 de borde
        3
    });
    // Creamos un vector de cajas para las versiones
    std::vector<gnu::Box> movieVersionBoxes(movieVersions.size(), versionBoxTemplate);
    std::string versionTag;
    std::string langTag;

    // ===== type of movie label =====
    gnu::Box typeSubBox({ 15, 1 });

    // Populamos el vector de versionBoxes
    for (size_t i = 0; i < movieVersionBoxes.size(); i++) {
        movieVersionBoxes[i].defaultBorderColor = true;
        movieVersionBoxes[i].position = {
            int(thumbnailWidth + 5),
            int((DETAIL_HEADER_HEIGHT + 5) + i * (versionBoxTemplate.size.y + 2))
        };
        //redrawing the type label
        versionTag = movieVersions[i]["version_tags"].get<std::string>();
        langTag = movieVersions[i]["language_tags"].get<std::string>();

        typeSubBox.content = versionTag + " | " + langTag;
        typeSubBox.position = movieVersionBoxes[i].position;
    }

    // ======= hours =======
    std::vector<size_t> hourSizes; // we need the sizes to limit the input
    for (size_t j = 0; j < movieVersionBoxes.size(); j++) {
        hourSizes.push_back(movieVersions[j]["sessions"].size() + 2);
    }
    int middleRelative = movieVersionBoxes[0].position.x + (movieVersionBoxes[0].size.x / 2);

    // > sessions are presented as small selectable hour labes
    std::vector<gnu::Box> sessionBoxes(3, gnu::Box({ 10, 1 }));

    sessionBoxes[0].setFontColor({ 80, 80, 80 });
    sessionBoxes[2].setFontColor({ 80, 80, 80 });

    int input;
    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    size_t session_i = 0;

    unsigned char globalOpt = 2;
    unsigned char movie_version_i = 0;
    bool firstDraw = true;

    // ======== main loop ==============
    while (true) {
        input = gnu::getch();

        if (input) switch (input) {
            case gnu::key::Right: {
                if (session_i < hourSizes[movie_version_i] - 3 && globalOpt == OPTION_SELECT_SESSION) {
                    session_i++;
                }
                break;
            }
            case gnu::key::Left: {
                if (session_i > 0 && globalOpt == OPTION_SELECT_SESSION) {
                    session_i--;
                }
                break;
            }
            case gnu::key::Up: {
                if (movie_version_i > 0 && globalOpt == OPTION_SELECT_SESSION) {
                    movie_version_i--;
                    session_i = 0;
                    break;
                }
                if (movie_version_i == 0 && globalOpt > OPTION_VIEW_MOVIE_TRAILER) {
                    globalOpt--;
                }
                break;
            }
            case gnu::key::Down: {
                if (movie_version_i < movieVersionBoxes.size() - 1 && globalOpt == OPTION_SELECT_SESSION) {
                    movie_version_i++;
                    session_i = 0;
                }

                if (movie_version_i == 0 && globalOpt < OPTION_SELECT_SESSION) {
                    globalOpt++;
                }
                break;
            }
            case gnu::key::Enter: {
                if (globalOpt == OPTION_VIEW_MOVIE_TRAILER) {
                    gnu::openBrowser(trailer_url);
                }
                if (globalOpt == OPTION_SELECT_OTHER_DATE) {
                    return "submenuSeleccionarFecha";
                }
                if (globalOpt == OPTION_SELECT_SESSION) {
                    // guardamos el id de la sesión seleccionada
                    g_sessionID = movieVersions[movie_version_i]["sessions"][session_i]["session_id"].get<std::string>();
                    return "Asientos";
                }
                break;
            }
            case gnu::key::ExitKey: {
                return "menuCartelera";
            }
        }

        // first we want to store the schedules hours so we can use them later on
        // Recorremos todas las versiones
        for (size_t i = 0; i < movieVersionBoxes.size(); i++) {
            std::vector<std::string> hours;
            // Creamos 2 vacías para el scroll
            hours.push_back("");
            for (const json &session : movieVersions[i]["sessions"]) {
                hours.push_back(session["hour"].get<std::string>());
            }
            hours.push_back("");

            // Nota: entiendo esta lógica, pero no sé si es neceasario un
            // sistema de scrolling, cuando podemos listar todas las horas
            for (size_t j = 0; j < sessionBoxes.size(); j++){
                if (movie_version_i == i) {
                    sessionBoxes[j].content = hours[j + session_i];
                }
                else {
                    sessionBoxes[j].content = hours[j];
                }
                sessionBoxes[j].showBorder = false;
                sessionBoxes[j].position.x = middleRelative - 25 + (20 * j);
                sessionBoxes[j].position.y = movieVersionBoxes[i].position.y + 2;
                sessionBoxes[j].draw();
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
            for (size_t i = 0; i < movieVersionBoxes.size(); i++){
                movieVersionBoxes[i].size.x = gnu::getConsoleSize().x - (thumbnailWidth + 6 + 2);
            }
            // ======= Dibujando el header =======
            header.size = gnu::vec2d({gnu::getConsoleSize().x , 10});
            header.draw();

            // Poster
            gnu::gotoXY({ 0, MOVIE_TITLE_OFFSET_Y - 1 });
            gnu::printRawOffset(movieRawThumbnail, 2);

            // ======= Información de la película =======
            // Se imprimen abajo del header, a la derecha del thumbnail
            gnu::gotoXY({ thumbnailWidth + 5, DETAIL_HEADER_HEIGHT + 1 });
            style::bold();
            gnu::print(movieDuration);
            gnu::print(" | " + movieRating + " | ");
            style::italic();
            gnu::print("Dirigido por " + movieDirector);
            style::setDefaultFg();

            // ======= Dibujando lista de actores =======
            // Se imprimen justo abajo del thumbnail
            if (movieCast.size() > 0) {
                gnu::gotoXY({ 3, thumbnailHeight + DETAIL_HEADER_HEIGHT - 3 });
                style::bold();
                style::underline();
                gnu::print("Reparto\n\n");
                style::setDefaultFg();
                for (std::string actor : movieCast) {
                    gnu::print("   • " + actor + "\n\n");
                }
            }

            // ======== titulo de la película ============
            gnu::gotoXY({ 0, MOVIE_TITLE_OFFSET_Y });
            style::setBg(averageThumbnailColor);
            style::setFg(complementaryThumbnailColor);
            style::bold();
            // Ascii art
            gnu::printRawOffset(movieMultilineTitle, thumbnailWidth + 5);
            style::reset_bg();
            style::setDefaultFg();
        }
        if (needToRedrawLayout || input) {
            // ======= drawing every session box on resize ===========
            for (size_t i = 0; i < movieVersionBoxes.size(); i++){
                if (i == movie_version_i && globalOpt == OPTION_SELECT_SESSION) {
                    movieVersionBoxes[i].setBorderColor({ 230, 50, 50 });
                }
                else {
                    movieVersionBoxes[i].defaultBorderColor = true;
                }
                movieVersionBoxes[i].draw();

                //redrawing the type label
                versionTag = movieVersions[i]["version_tags"].get<std::string>();
                langTag = movieVersions[i]["language_tags"].get<std::string>();

                typeSubBox.content = versionTag + " | " + langTag;
                typeSubBox.position = movieVersionBoxes[i].position;
                typeSubBox.draw();
            }

            if (globalOpt == OPTION_SELECT_OTHER_DATE){
                date.showBorder = true;
            }
            else {
                date.showBorder = false;
                date.flushBorders();
            }
            date.position = { short(getConsoleSize().x - 16), DETAIL_HEADER_HEIGHT - 3 };
            date.draw();
        }
        lastConsoleSize = gnu::getConsoleSize();
        gnu::sleep(5);
    }
    return "exit"; // no se debería llegar aquí
}

/* --- Submenús --- */

std::string submenuSeleccionarFecha() {
    gnu::cls();
    std::string seleccionaFechaTitulo = gnu::parseStringToASCIIArtText("Selecciona una fecha");

    gnu::gotoXY({ 0, 0 });
    style::setFg({ 219, 119, 126 });
    gnu::print("\n\n\n\n\n");
    gnu::printRawCenter(seleccionaFechaTitulo);
    style::setFg({ 222, 196, 198 });
    gnu::print("\n\n");
    gnu::printLineCentered("Selecciona tu fecha favorita para ver \"" + g_movieData["title"].get<std::string>() + "\"");

    gnu::gotoY(15);

    json wholeRaw = gnu::apifetch("/cines/" + g_cineID + "/cartelera/" + g_movieID);
    std::vector<json> daysRaw = wholeRaw["days"].get<std::vector<json>>();

    std::vector<std::string> dates;

    for (json obj : daysRaw) dates.push_back(obj["date"].get<std::string>());
    dates.insert(dates.begin(), "");
    dates.insert(dates.end(), "");

    std::vector<gnu::Box> dateContainers(3, gnu::Box(
        {gnu::getConsoleSize().x - 4, 1}
    ));

    for (size_t i = 0; i < dateContainers.size(); i++) {
        dateContainers[i].content = dates[i];
        dateContainers[i].position = gnu::vec2d({ 0, int(15 + 3 * i)});
        dateContainers[i].showBorder = false;
        dateContainers[i].draw();
    }

    dateContainers[1].setFontColor({ 230, 50, 50 });
    dateContainers[1].showBorder = true;

    int input;
    unsigned char fecha_i = 0;

    while (true) {
        input = gnu::getch();

        switch (input) {
        case gnu::key::Up:
            if (fecha_i > 0) fecha_i--;
            break;
        case gnu::key::Down:
            if (fecha_i < dates.size() - 3) fecha_i++;
            break;
        case gnu::key::Enter:
            g_selectedDate = dates[1 + fecha_i];
            return "menuDetalles";
        }

        for (size_t i = 0; i < dateContainers.size(); i++) {
            dateContainers[i].content = dates[i + fecha_i];
            dateContainers[i].position = gnu::vec2d({ 0, int(15 + 3 * i)});
            dateContainers[i].showBorder = false;
            dateContainers[i].draw();
        }
    }
}

/* --- Utilidades --- */

// Obtiene la sinopsis de la película y la acorta si es necesario
std::string getSynopsis(json movieData) {
    std::string synopsis = movieData["synopsis"].get<std::string>();
    if (synopsis.length() > MAX_SYNOPSIS_LENGTH) {
        synopsis = synopsis.substr(0, MAX_SYNOPSIS_LENGTH);
        synopsis += "...";
    }
    return synopsis;
}

std::string formatTime(std::string minutes) {
    int hours = std::stoi(minutes) / 60;
    int minutesInt = std::stoi(minutes) % 60;
    std::string minutesStr = std::to_string(minutesInt);
    if (minutesInt < 10) {
        minutesStr = "0" + minutesStr;
    }
    return std::to_string(hours) + "h " + minutesStr + "min";
}

#include <unordered_map>

/*
  https://fsymbols.com/generators/carty/

 {"▄▀█", {"█▄▄", {"█▀▀", {"█▀▄", {"█▀▀", {"█▀▀", {"█▀▀", {"█░█", {"█", {"░░█", {"█▄▀", {"█░░", {"█▀▄▀█", {"█▄░█", {"█▄░█", {"█▀█", {"█▀█", {"█▀█", {"█▀█", {"█▀", {"▀█▀", {"█░█", {"█░█", {"█░█░█", {"▀▄▀", {"█▄█", {"▀█", {"▀", {"▄▄", {"░", {"░",
  "█▀█"}  "█▄█"}  "█▄▄"}  "█▄▀"}  "██▄"}  "█▀░"}  "█▄█"}  "█▀█"}  "█"}  "█▄█"}  "█░█"}  "█▄▄"}  "█░▀░█"}  "█░▀█"}  "█░▀█"}  "█▄█"}  "█▀▀"}  "▀▀█"}  "█▀▄"}  "▄█"}  "░█░"}  "█▄█"}  "▀▄▀"}  "▀▄▀▄▀"}  "█░█"}  "░█░"}  "█▄"}  "▄"}  "░░"}  "█"}  "▄"}

  {"█", {"▀█",
   "▄"}  "░▄"}

  {"▄█", {"▀█", {"█░█", {"█▀", {"█▄▄", {"▀▀█", {"█▀█", {"█▀█",
   "░█"}  "█▄"}  "▀▀█"}  "▄█"}  "█▄█"}  "░░█"}  "▀▀█"}  "█▄█"}
*/

std::unordered_map<std::string, std::pair<std::string, std::string>> asciiMap = {
    { "a", {"▄▀█"  ,
            "█▀█"  } },
    { "b", {"█▄▄"  ,
            "█▄█"  } },
    { "c", {"█▀▀"  ,
            "█▄▄"  } },
    { "d", {"█▀▄"  ,
            "█▄▀"  } },
    { "e", {"█▀▀"  ,
            "██▄"  } },
    { "f", {"█▀▀"  ,
            "█▀░"  } },
    { "g", {"█▀▀"  ,
            "█▄█"  } },
    { "h", {"█░█"  ,
            "█▀█"  } },
    { "i", {"█"    ,
            "█"    } },
    { "j", {"░░█"  ,
            "█▄█"  } },
    { "k", {"█▄▀"  ,
            "█░█"  } },
    { "l", {"█░░"  ,
            "█▄▄"  } },
    { "m", {"█▀▄▀█",
            "█░▀░█"} },
    { "n", {"█▄░█" ,
            "█░▀█" } },
    { "o", {"█▀█"  ,
            "█▄█"  } },
    { "p", {"█▀█"  ,
            "█▀▀"  } },
    { "q", {"█▀█"  ,
            "▀▀█"  } },
    { "r", {"█▀█"  ,
            "█▀▄"  } },
    { "s", {"█▀"   ,
            "▄█"   } },
    { "t", {"▀█▀"  ,
            "░█░"  } },
    { "u", {"█░█"  ,
            "█▄█"  } },
    { "v", {"█░█"  ,
            "▀▄▀"  } },
    { "w", {"█░█░█",
            "▀▄▀▄▀"} },
    { "x", { "▀▄▀" ,
             "█░█" } },
    { "y", {"█▄█"  ,
            "░█░"  } },
    { "z", {"▀█"   ,
            "█▄"   } },
    { ":", {"▀"    ,
            "▄"    } },
    { "-", {"▄▄"   ,
            "░░"   } },
    { ",", {"░"    ,
            "█"    } },
    { ".", {"░"    ,
            "▄"    } },
    { "?", {"▀█"   ,
            "░▄"   } },
    { "!", {"█"    ,
            "▄"    } },
    { "¿", {"▀░"   ,
            "█▄"   } },
    { "¡", {"▀"    ,
            "█"    } },
    { "1", {"▄█"   ,
            "░█"   } },
    { "2", {"▀█"   ,
            "█▄"   } },
    { "3", {"▀█▀"   ,
            "▄▄█"  } },
    { "4", {"█░█"  ,
            "▀▀█"  } },
    { "5", {"█▀"   ,
            "▄█"   } },
    { "6", {"█▄▄"  ,
            "█▄█"  } },
    { "7", {"▀▀█"   ,
            "░░█"   } },
    { "8", {"█▀█"   ,
            "███"   } },
    { "9", {"█▀█"   ,
            "▀▀█"   } },
    { "0", {"█▀█"  ,
            "█▄█"  } }
};

#include <locale> // std::locale, std::tolower

std::string parseStringToASCIIArtText(std::string text) {

    std::string asciiArtTop = "";
    std::string asciiArtBottom = "";
    std::string toLowerText = "";

    for (char& c : text) {
        toLowerText += std::tolower(c, std::locale());
    }

    for (const utf8::utf8_char_t &ch : utf8::iterate(toLowerText)) {
        if (ch == " ") {
            asciiArtTop += "  ";
            asciiArtBottom += "  ";
            continue;
        }
        if (asciiMap.find(ch) == asciiMap.end()) continue;
        asciiArtTop += asciiMap[ch].first + " ";
        asciiArtBottom += asciiMap[ch].second + " ";
    }

    return asciiArtTop + "\n" + asciiArtBottom;
}

} // namespace gnu
