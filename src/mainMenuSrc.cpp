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
    ShowConsoleCursor(false);

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
    size_t currentMovie_i = 0;
    std::vector<std::string> availableDates;

    for (auto billboard : billboardDays["days"].get<std::vector<json>>()) {
        availableDates.push_back(billboard["date"].get<std::string>());
    }

    bool lock = true;
    int currentDay = 0;
    short optSelection = 0;

    std::string currentString;

    while (lock) {
        // guarda la película actual
        billboardsByDate = billboardDays["days"].get<std::vector<json>>();
        currentBillboard = billboardsByDate[currentDay].get<json>();
        currentString = currentBillboard["movies"][currentMovie_i]["title"].get<std::string>();

        gotoXY(0, 15);
        cleanLine();
        displayDate(availableDates[currentDay], optSelection);

        gotoXY(
            (getConsoleRectSize().x / 2) - (currentString.length() / 2), 15
        );

        if (optSelection == 0) {
            SetConsoleTextAttribute(consoleHandle, WHITE);
        }
        if (optSelection == 1) {
            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);
        }
        std::cout << "> " << currentString << " <";

        gotoXY(0, 16);
        cleanLine();

        gotoXY(
            (getConsoleRectSize().x / 2) + (currentString.length() / 2), 16
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
    json closeCineData = fetch(baseApi + "/cines/cercanos");
    size_t currentCine = 0;
    int nameCursorPos = 0;
    int listSize = 5;
    DWORD color = 1;
    bool lock = true;

    system("cls");

    // main menu loop
    while (lock) {
        gotoXY(getConsoleRectSize().x / 2, 0);
        // Display the logo
        logoDisplay3D(color);
        color %= 15;
        color ++;

        std::cout << std::endl;

        if (GetAsyncKeyState(VK_DOWN)) {
            if (currentCine < closeCineData["cinemas"].size() - 1) {
                currentCine++;
            }
            if (nameCursorPos < listSize - 1) {
                nameCursorPos++;
            }
            else {
                nameCursorPos = listSize - 1;
            }
        }
        if (GetAsyncKeyState(VK_UP)) {
            if (currentCine > 0) {
                currentCine--;
            }
        }
        if (GetAsyncKeyState(VK_SPACE)) {
            cinemaID = closeCineData["cinemas"][currentCine]["cinema_id"].get<std::string>();
            lock = false;
        }

        cinemaListDisplay(closeCineData["cinemas"], currentCine, nameCursorPos, listSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
