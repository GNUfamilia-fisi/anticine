#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>

#include "utilsDisplay.h"
#include "json.hpp"

void logoDisplay(DWORD color);


HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void cinemaDisplay(json closeData);

using json = nlohmann::json;



//refer to utisDisplay.h for the complete list of foreground colors



int main(){

    short currentCine = 0;

    json closeCineData = fetch("https://a195-181-66-156-128.sa.ngrok.io/cines/cercanos");
    
    system("cls");

    DWORD color = 7;

    //main menu loop
    while(1){

        gotoXY(getConsoleRectSize().x / 2,0);
        
        logoDisplay(color);

        color++;

        if(color == 15){
            color = 7;
        }

        std::cout << std::endl;

        if (GetAsyncKeyState(VK_RIGHT)){
            //TODO
        }

        else if(GetAsyncKeyState(VK_LEFT)){
            //TODO
        }

        else{
            //TODO
        }

        cinemaDisplay(closeCineData["cinemas"][currentCine]["name"]);

        gotoXY(getConsoleRectSize().x / 3 + 3, 20);

        std::this_thread::sleep_for(std::chrono::seconds(1));
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

    gotoXY(getConsoleRectSize().x / 3, 16);

    std::cout << "< " << to_string(closeData) << " >";

    std::cout << std::endl;

    std::cout << std::endl;
}