#include "utilsDisplay.h"
#include "json.hpp"

using json = nlohmann::json;

void logoDisplay3D(DWORD color);

void logoDisplay2D(DWORD color);    

void cinemaListDisplay(json closeData, int current, int namePos, int showSize);

void colorLine(DWORD color, int y);

void displayDate(std::string day, std::string month, std::string year, short opt);


//================= menu prototypes ====================
void chooseCinema();
void carteleraFecha();




//==================== globals ==================
HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

std::string cinemaID = "740";

std::string baseApi = "https://27a0-181-66-156-128.sa.ngrok.io";




//refer to utisDisplay.h for the complete list of foreground colors


int main(int argc, char const *argv[])
{
    ShowConsoleCursor(false);

    chooseCinema();
    carteleraFecha();

    return 0;
}


void carteleraFecha(){

    system("cls");

    logoDisplay2D(WHITE);    

    json carteleraData = fetch(baseApi + "/cines/" + cinemaID + "/cartelera");


    std::time_t raw = std::time(nullptr);
    std::tm *date = localtime(&raw);
    std::string day;
    std::string month;

    day = (date->tm_mday < 10) ? ("0" + std::to_string(date->tm_mday)) : std::to_string(date->tm_mday);
    month = (date->tm_mon < 9) ? ("0" + std::to_string(date->tm_mon + 1)) : std::to_string(date->tm_mon + 1);
    
    std::string year = std::to_string(1900 + date->tm_year);
    std::string format = year + "-" + month + "-" + day;

    json moviesList;

    for(auto cinemaDay : carteleraData["days"]){    
        if (cinemaDay["date"] == format){
            moviesList = cinemaDay["movies"];
        }
    }

    bool lock = true;

    int currentDay = date->tm_mday;
    int currentPos = 0;

    short optSelection = 0;

    std::string currentString;

    while(lock){

        moviesList = json::parse(R"(
        [
        ]
        )");    
    
        for(auto cinemaDay : carteleraData["days"]){    
            if (cinemaDay["date"] == year + "-" + month + "-" + day){
                moviesList = cinemaDay["movies"];
            }
        }
    
        displayDate(day, month, year, optSelection);

        day = (currentDay < 10) ? ("0" + std::to_string(currentDay)) : std::to_string(currentDay);

        if(GetAsyncKeyState(VK_DOWN) && optSelection < 1){
            optSelection++;
        }
        
        if(GetAsyncKeyState(VK_UP) && optSelection > 0){
            optSelection--;
        }
        
        if(GetAsyncKeyState(VK_RIGHT)){
            if(std::stoi(day) < 31 && optSelection == 0){
                currentDay++;
                currentPos = 0;
            }

            if (moviesList.size() != 0 && currentPos < moviesList.size() - 1 && optSelection == 1){
                currentPos++;
            }
        }

        if(GetAsyncKeyState(VK_LEFT)){
            if(std::stoi(day) > 1 && optSelection == 0){
                currentDay--;
                currentPos = 0;
            }

            if(currentPos > 0 && optSelection == 1){
                currentPos--;
            }
        }

        if (moviesList.size() != 0){
            currentString = moviesList[currentPos]["title"].get<std::string>();
        }
        else{
            currentString = "No movies";
        }

        gotoXY(0,15);
        cleanLine();

        gotoXY(
            (getConsoleRectSize().x / 2) - (currentString.length() / 2),
            15
        );

        if(optSelection == 0){
            SetConsoleTextAttribute(consoleHandle, WHITE);            
        }

        if(optSelection == 1){
            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);            
        }

        std::cout << "> " << currentString << " <";

        gotoXY(0,16);
        cleanLine();

        gotoXY(
            (getConsoleRectSize().x / 2) + (currentString.length() / 2),
            16
        );
        std::cout << currentPos + 1 << "/" << moviesList.size();

        SetConsoleTextAttribute(consoleHandle, WHITE);


        //std::cout << "\n\nopt es: " << optSelection << "\n\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


}


void displayDate(std::string day, std::string month, std::string year, short opt){
    gotoXY(
        (getConsoleRectSize().x / 2) - 5,
        12
    );

    std::cout << year + "-" + month + "-" << day;

    gotoXY(
        (getConsoleRectSize().x / 2) + 3,
        13
    );

    if(opt == 0){    
        SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);


        std::cout << "^^";

        SetConsoleTextAttribute(consoleHandle, WHITE);
    }
    else{
        cleanLine();
    }

}

void chooseCinema(){
    
    json closeCineData = fetch(baseApi + "/cines/cercanos");
    
    int currentCine = 0;
    
    int nameCursorPos = 0;

    int listSize = 5;

    DWORD color = 1;

    bool lock = true;
    
    system("cls");

    //main menu loop
    while(lock){

        gotoXY(getConsoleRectSize().x / 2,0);
        
        //Display the logo
        logoDisplay3D(color);

        color++;

        if(color == 15){
            color = 1;
        }

        std::cout << std::endl;

        if(GetAsyncKeyState(VK_DOWN)){
            if(currentCine < closeCineData["cinemas"].size() - 1){
                currentCine++;    
            }

            if (nameCursorPos < listSize - 1){
                nameCursorPos++;
            }

            else{
                nameCursorPos = listSize - 1;
            }
        }
        
        if(GetAsyncKeyState(VK_UP)){
            if(currentCine > 0){
                currentCine--;
            }
        }

        if(GetAsyncKeyState(VK_SPACE)){
            cinemaID = closeCineData["cinemas"][currentCine]["cinema_id"].get<std::string>();
            lock = false;
        }

        cinemaListDisplay(closeCineData["cinemas"], currentCine, nameCursorPos, listSize);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void cinemaListDisplay(json closeData, int current, int namePos, int showSize){
    SetConsoleTextAttribute(consoleHandle, WHITE);

    int indic = namePos;


    if(closeData.size() < showSize){
        showSize = closeData.size();
    }


    if(current < showSize){
        indic = current;
    }

    std::string names[showSize];
    
    if (current < showSize){
        for(int i = 0; i < showSize; i++){
            names[i] = closeData[i]["name"].get<std::string>();
        }
    }

    else{
        for(int i = 0; i < showSize; i++){
            names[i] = closeData[i + current - showSize + 1]["name"].get<std::string>();
        }
    }

    //cleaner
    for (int i = 0; i < showSize; i++){
        gotoXY(0, 12 + i);
        cleanLine();
    }


    for (int i = 0; i < showSize; i++){

        if (i == indic){
            gotoXY(
                (getConsoleRectSize().x / 2) - names[i].length() - 1,
                12 + i
            );

            SetConsoleTextAttribute(consoleHandle, LIGHTGREEN);

            std::cout << ">>";
        }

        else{
            SetConsoleTextAttribute(consoleHandle, WHITE);
        }

        gotoXY(
            (getConsoleRectSize().x / 2) - (names[i].length() / 2),
            12 + i
        );

        std::cout << names[i];
    }
}