#include "utilsDisplay.h"
#include "json.hpp"

void logoDisplay(DWORD color);

void cinemaDisplay(json closeData);

using json = nlohmann::json;

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

int cinemaID;

//refer to utisDisplay.h for the complete list of foreground colors



int main(){

    short currentCine = 0;

    json closeCineData = fetch("https://a195-181-66-156-128.sa.ngrok.io/cines/cercanos");
    /*
    json closeCineData = json::parse(R"(
    {
        "city": "Lima",
        "cinemas": [
            {
                "cinema_id": "2705",
                "name": "CiNEXT Gamarra",
                "city": "Lima"
            },
            {
                "cinema_id": "2705",
                "name": "CiNEXT Gamarra 2",
                "city": "Lima"
            }
        ],
        "nearest_id": "2705",
        "code": 200,
        "error": null
    })");
    */

    DWORD color = 0;
    
    system("cls");

    //main menu loop
    while(1){

        gotoXY(getConsoleRectSize().x / 2,0);
        
        logoDisplay(color);

        color++;

        if(color == 15){
            color = 0;
        }

        std::cout << std::endl;

        if (GetAsyncKeyState(VK_RIGHT) && currentCine < closeCineData["cinemas"].size()){
            currentCine++;
        }

        else if(GetAsyncKeyState(VK_LEFT) && currentCine > 0){
            currentCine--;
        }

        else if(GetAsyncKeyState(VK_RETURN)){
            //cinemaID = stoi(closeCineData["cinemas"][currentCine]["cinema_id"]);
        }

        cinemaDisplay(closeCineData["cinemas"][currentCine]["name"]);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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


void cinemaDisplay(json closeData){
    SetConsoleTextAttribute(consoleHandle, WHITE);

    gotoXY(getConsoleRectSize().x / 3, 12);

    cleanLine();

    gotoXY(getConsoleRectSize().x / 3, 12);

    std::cout << "< " << nlohmann::to_string(closeData) << " >";

    gotoXY(getConsoleRectSize().x / 3 + 3, 12);
}