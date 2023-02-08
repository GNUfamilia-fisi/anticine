#include "utilsDisplay.h"
#include "json.hpp"

using json = nlohmann::json;

void logoDisplay3D(DWORD color);
void logoDisplay2D(DWORD color);
void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize);
void colorLine(DWORD color, int y); // no implementado aún
void displayDate(std::string fulldate, short opt);

//================= menu prototypes ====================

void chooseCinema();
void carteleraFecha();

//==================== globals ==================

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
std::string cinemaID = "740";
std::string baseApi = "https://api.cinext.up.railway.app";

// refer to utisDisplay.h for the complete list of foreground colors

int main(void) {
    system("cls");
    ShowConsoleCursor(false);

    cargandoDisplay();
    chooseCinema();
    carteleraFecha();

    return 0;
}

void carteleraFecha() {
    system("cls");
    logoDisplay2D(WHITE);

    json billboardDays = fetch(baseApi + "/cines/" + cinemaID + "/cartelera");

    // json moviesList;
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
    size_t currentMovie_i = 0;

    while (lock) {
        // guarda la película actual
        billboardsByDate = billboardDays["days"].get<std::vector<json>>();
        currentBillboard = billboardsByDate[currentDay].get<json>();
        currentMovie = currentBillboard["movies"][currentMovie_i].get<json>();

        gotoXY(0, 15);
        cleanLine();
        displayDate(availableDates[currentDay], optSelection);

        gotoXY(
            (getConsoleRectSize().x / 2) - (currentMovie["title"].size() / 2), 15
        );

        if (optSelection == 0) {
            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);
        }
        if (optSelection == 1) {
            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);
        }
        std::cout << "> " << currentMovie["title"] << " <";

        gotoXY(0, 16);
        cleanLine();

        gotoXY(
            (getConsoleRectSize().x / 2) + (currentMovie["title"].size() / 2), 16
        );
        std::cout << currentMovie_i + 1 << "/" << currentBillboard["movies"].size();

        SetConsoleTextAttribute(consoleHandle, WHITE);

        // Check input
        if (_kbhit()) {
            char hit = _getch();
            switch (hit) {
            case KeyCode::Up: {
                if (optSelection > 0) optSelection--;
                break;
            }
            case KeyCode::Down: {
                if (optSelection < 1) {
                    optSelection++;
                }
                break;
            }
            case KeyCode::Left: {
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
            case KeyCode::Right: {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void displayDate(std::string fulldate, short opt) {
    gotoXY(
        (getConsoleRectSize().x / 2) - 5, 12
    );
    std::cout << fulldate;

    gotoXY(
        (getConsoleRectSize().x / 2) + 3, 13
    );

    if (opt == 0) {
        SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);
        std::cout << "^^";
        SetConsoleTextAttribute(consoleHandle, WHITE);
    }
    else {
        cleanLine();
    }
}

void chooseCinema() {
    system("cls");

    // Retorna una lista de cines ordenados según la geolocalización
    // del usuario, el primero siempre es el más cercano.
    // Dependiendo de 
    json nearCinemasData = fetch(baseApi + "/cines/cercanos");
    size_t currentCine = 0;
    int nameCursorPos = 0;
    int listFixedSize = 5;
    DWORD color = 1;
    bool lock = true;

    // main menu loop
    while (lock) {
        gotoXY(getConsoleRectSize().x / 2, 0);
        // Display the logo
        logoDisplay3D(color);
        color %= 15;
        color++;

        std::cout << std::endl;

        if (_kbhit()) {
            char hit = _getch();
            switch (hit) {
            case KeyCode::Down:
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
            case KeyCode::Up:
                if (currentCine > 0) {
                    currentCine--;
                }
                break;
            case KeyCode::Enter:
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

        cinemaListDisplay(nearCinemasData["cinemas"], currentCine, nameCursorPos, listFixedSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize) {
    SetConsoleTextAttribute(consoleHandle, WHITE);

    size_t indic = namePos;

    if (closeData.size() < showSize) {
        showSize = closeData.size();
    }
    if (current < showSize) {
        indic = current;
    }

    std::string names[showSize];

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
        gotoXY(0, 12 + i);
        cleanLine();
    }
    for (size_t i = 0; i < showSize; i++) {

        if (i == indic) {
            gotoXY(
                (getConsoleRectSize().x / 2) - names[i].length() - 1, 12 + i
            );

            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);

            std::cout << ">>";
        }
        else {
            SetConsoleTextAttribute(consoleHandle, WHITE);
        }
        gotoXY(
            (getConsoleRectSize().x / 2) - (names[i].length() / 2), 12 + i
        );
        std::cout << names[i];
    }
}
