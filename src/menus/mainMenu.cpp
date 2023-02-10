#include "../consoleUtils.h"
#include "../services.h"
#include <json.hpp>
#include "mainMenu.h"

using json = nlohmann::json;

//==================== globales ==================

std::string cinemaID = "2706";

void gnu::carteleraFecha() {
    system("cls");
    gnu::displayLogo2D(gnu::color::RED);

    json billboardDays = gnu::apifetch("/cines/" + cinemaID + "/cartelera");

    std::vector<json> billboardsByDate;
    json currentBillboard;
    std::vector<std::string> availableDates;

    for (auto billboard : billboardDays["days"].get<std::vector<json>>()) {
        availableDates.push_back(billboard["date"].get<std::string>());
    }

    bool lock = true;
    size_t currentDay = 0;
    short optSelection = 0;

    json currentMovie;
    std::string currentMovieName;
    size_t currentMovie_i = 0;

    while (lock) {
        // guarda la película actual
        billboardsByDate = billboardDays["days"].get<std::vector<json>>();
        currentBillboard = billboardsByDate[currentDay].get<json>();
        currentMovie = currentBillboard["movies"][currentMovie_i].get<json>();
        currentMovieName = currentMovie["title"].get<std::string>();

        gnu::gotoXY(0, 15);
        gnu::cleanLine();
        gnu::displayDate(availableDates[currentDay], optSelection);

        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (currentMovieName.size() / 2) - 2, 15
        );

        if (optSelection == 0) {
            SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::WHITE);
        }
        if (optSelection == 1) {
            SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::LIGHTGREEN);
        }
        std::cout << "> " << currentMovieName << " <";

        gnu::gotoXY(0, 16);
        gnu::cleanLine();

        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) + (currentMovieName.size() / 2), 16
        );
        std::cout << currentMovie_i + 1 << "/" << currentBillboard["movies"].size();

        SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::WHITE);

        // Check input
        if (_kbhit()) {
            char hit = _getch();
            switch (hit) {
            case gnu::key::Up: {
                if (optSelection > 0) optSelection--;
                break;
            }
            case gnu::key::Down: {
                if (optSelection < 1) {
                    optSelection++;
                }
                break;
            }
            case gnu::key::Left: {
                if (optSelection == 0) {
                    if (currentDay > 0) {
                        currentDay--;
                    }
                    currentMovie_i = 0;
                }
                if (optSelection == 1) {
                    if (currentMovie_i > 0) currentMovie_i--;
                }
                break;
            }
            case gnu::key::Right: {
                if (optSelection == 0) {
                    if (currentDay + 1 < availableDates.size()) {
                        currentDay++;
                    }
                    currentMovie_i = 0; // reiniciarlo a la opción 0
                }
                if (optSelection == 1) {
                    if (currentMovie_i + 1 < currentBillboard["movies"].size()) {
                        currentMovie_i++;
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        gnu::sleep(5);
    }
}

void gnu::displayDate(std::string fulldate, short opt) {
    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) - 5, 12
    );
    std::cout << fulldate;

    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) + 3, 13
    );

    if (opt == 0) {
        SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::LIGHTGREEN);
        std::cout << "^^";
        SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::WHITE);
    }
    else {
        gnu::cleanLine();
    }
}

void gnu::chooseCinema() {
    system("cls");

    // Retorna una lista de cines ordenados según la geolocalización
    // del usuario, el primero siempre es el más cercano.
    // Dependiendo de 
    json nearCinemasData = gnu::apifetch("/cines/cercanos");
    size_t currentCine = 0;
    int nameCursorPos = 0;
    int listFixedSize = 5;
    unsigned short color = gnu::color::BLUE;
    bool lock = true;

    // main menu loop
    while (lock) {
        gnu::gotoXY(gnu::getConsoleSize().x / 2, 0);
        // Display the logo
        gnu::logoDisplay3D((gnu::color)color);
        color %= 15;
        color++;

        std::cout << std::endl;

        if (_kbhit()) {
            char hit = _getch();
            switch (hit) {
            case gnu::key::Down:
                if (currentCine < nearCinemasData["cinemas"].size() - 1) {
                    currentCine++;
                }
                if (nameCursorPos < listFixedSize - 1) {
                    nameCursorPos++;
                }
                else {
                    nameCursorPos = listFixedSize - 1;
                }
                break;
            case gnu::key::Up:
                if (currentCine > 0) {
                    currentCine--;
                }
                break;
            case gnu::key::Enter:
                cinemaID = nearCinemasData["cinemas"][currentCine]["cinema_id"].get<std::string>();
                lock = false;
                break;
            default:
                break;
            }
        }
        if (GetAsyncKeyState(VK_SPACE)) {
            cinemaID = nearCinemasData["cinemas"][currentCine]["cinema_id"].get<std::string>();
            lock = false;
        }

        gnu::cinemaListDisplay(nearCinemasData["cinemas"], currentCine, nameCursorPos, listFixedSize);
        gnu::sleep(5);
    }
}

void gnu::cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize) {
    SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::WHITE);

    size_t indic = namePos;

    if (closeData.size() < showSize) {
        showSize = closeData.size();
    }
    if (current < showSize) {
        indic = current;
    }

    // std::string names[showSize];
    std::string names[128];

    if (current < showSize) {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i]["name"].get<std::string>();
        }
    }
    else {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i + current - showSize + 1]["name"].get<std::string>();
        }
    }
    // cleaner
    for (size_t i = 0; i < showSize; i++) {
        gnu::gotoXY(0, 12 + i);
        gnu::cleanLine();
    }
    for (size_t i = 0; i < showSize; i++) {

        if (i == indic) {
            gnu::gotoXY(
                (gnu::getConsoleSize().x / 2) - names[i].length() - 1, 12 + i
            );

            SetConsoleTextAttribute(gnu::consoleHandle, gnu::LIGHTGREEN);

            std::cout << ">>";
        }
        else {
            SetConsoleTextAttribute(gnu::consoleHandle, gnu::WHITE);
        }
        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (names[i].length() / 2), 12 + i
        );
        std::cout << names[i];
    }
}


void gnu::logoDisplay3D(gnu::color color) {
    SetConsoleTextAttribute(gnu::consoleHandle, color);
    std::cout << R"(
         $$$$$$\  $$\ $$\   $$\ $$$$$$$$\ $$\   $$\ $$$$$$$$\
        $$  __$$\ \__|$$$\  $$ |$$  _____|$$ |  $$ |\__$$  __|
        $$ /  \__|$$\ $$$$\ $$ |$$ |      \$$\ $$  |   $$ |   
        $$ |      $$ |$$ $$\$$ |$$$$$\     \$$$$  /    $$ |   
        $$ |      $$ |$$ \$$$$ |$$  __|    $$  $$<     $$ |   
        $$ |  $$\ $$ |$$ |\$$$ |$$ |      $$  /\$$\    $$ |   
        \$$$$$$  |$$ |$$ | \$$ |$$$$$$$$\ $$ /  $$ |   $$ |   
         \______/ \__|\__|  \__|\________|\__|  \__|   \__|   
    )" << std::endl;
}

void gnu::displayLogo2D(gnu::color color) {
    SetConsoleTextAttribute(gnu::consoleHandle, color);
    std::cout << R"(
       _______ _   _________  ________
      / ____(_/ | / / ____| |/ /_  __/
     / /   / /  |/ / __/  |   / / /   
    / /___/ / /|  / /___ /   | / /    
    \____/_/_/ |_/_____//_/|_|/_/     
                                  
    )" << std::endl;
}

void gnu::cargandoDisplay() {
    SetConsoleTextAttribute(gnu::consoleHandle, gnu::color::LIGHTCYAN);
    char cargando = '#';
    char inicio = '|';
    std::cout<< R"(
                                                 .___
      ____ _____ _______  _________    ____    __| _/____  
    _/ ___\\__  \\_  __ \/ ___\__  \  /    \  / __ |/  _ \
    \  \___ / __ \|  | \/ /_/  > __ \|   |  \/ /_/ (  <_> )
     \___  >____  /__|  \___  (____  /___|  /\____ |\____/ 
         \/     \/     /_____/     \/     \/      \/       
    )"<<"\n";
    
    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) - (33 / 2),
        15
    );
    
    std::cout << inicio;
    
    for (int i = 0 ; i < 32 ; i++){
        std::cout << cargando;
        gnu::sleep(20);
    }
    
    std::cout << inicio;
    
    gnu::sleep(400);

    system("cls");
}
