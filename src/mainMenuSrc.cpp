#include "utilsDisplay.h"
#include "json.hpp"

using json = nlohmann::json;

void logoDisplay(DWORD color);

void cinemaDisplay(json closeData);

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

std::string cinemaID;

json closeCineData = fetch("https://a195-181-66-156-128.sa.ngrok.io/cines/cercanos");

//refer to utisDisplay.h for the complete list of foreground colors


int main(){

    short currentCine = 0;

    DWORD color = 0;
    
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

        if (GetAsyncKeyState(VK_RIGHT)){
            if(currentCine < closeCineData["cinemas"].size() - 1){
                currentCine++;
            }
        }

        else if(GetAsyncKeyState(VK_LEFT) && currentCine > 0){
            currentCine--;
        }

        else if(GetAsyncKeyState(VK_RETURN)){
            cinemaID = closeCineData["cinemas"][currentCine]["cinema_id"].get<std::string>();
        }

        cinemaDisplay(closeCineData["cinemas"][currentCine]["name"]);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
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

    std::string displayed = "> " + closeData.get<std::string>() + " <";

    int Xpos = (getConsoleRectSize().x / 2) - (displayed.length() / 2);

    gotoXY(0, 12);
    cleanLine();

    gotoXY(Xpos, 12);
    std::cout << displayed;

    gotoXY(Xpos + 3, 12);
}