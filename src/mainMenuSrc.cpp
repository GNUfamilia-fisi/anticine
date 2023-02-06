#include "utilsDisplay.h"
#include "json.hpp"

using json = nlohmann::json;

void logoDisplay(DWORD color);

void cinemaDisplay(json closeData, int current, int namePos);

void colorLine(DWORD color, int y);

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

std::string cinemaID;

json closeCineData = fetch("https://27a0-181-66-156-128.sa.ngrok.io/cines/cercanos");

//refer to utisDisplay.h for the complete list of foreground colors


int main(){

    int currentCine = 0;
    int nameCursorPos = 0;

    DWORD color = 0;

    char lock = 0;
    
    system("cls");
    ShowConsoleCursor(false);

    //main menu loop
    while(1){

        gotoXY(getConsoleRectSize().x / 2,0);
        
        logoDisplay(color);

        color++;

        if(color == 15){
            color = 0;
        }

        std::cout << std::endl;

        if(GetAsyncKeyState(VK_DOWN)){
            if(currentCine < closeCineData["cinemas"].size() - 1){
                currentCine++;    
            }

            if (nameCursorPos < 4){
                nameCursorPos++;
            }

            else{
                nameCursorPos = 4;
            }
        }
        
        if(GetAsyncKeyState(VK_UP)){
            if(currentCine > 0){
                currentCine--;
            }
        }

        if(GetAsyncKeyState(VK_RETURN)){
            cinemaID = closeCineData["cinemas"][currentCine]["cinema_id"].get<std::string>();
        }

        cinemaDisplay(closeCineData["cinemas"], currentCine, nameCursorPos);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}


void logoDisplay(DWORD color){
    SetConsoleTextAttribute(consoleHandle, color);

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


void cinemaDisplay(json closeData, int current, int namePos){
    SetConsoleTextAttribute(consoleHandle, WHITE);

    int showSize = 5;
    int indic = namePos;

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