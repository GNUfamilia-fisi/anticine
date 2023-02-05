#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>

#include "utilsDisplay.h"
#include "json.hpp"

void logoDisplay(DWORD color);


HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void cinemaDisplay();

using json = nlohmann::json;



//refer to utisDisplay.h for the complete list of foreground colors



int main(){
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

        //gotoXY(getConsoleRectSize().x / 2, 10);

        cinemaDisplay();

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
    )";
}


void cinemaDisplay(){
    for (int i = 0; i < getConsoleRectSize().x / 3; i++){
        std::cout << "=";
    }
    std::cout << std::endl;
    for (int i = 0; i < getConsoleRectSize().x / 3; i++){
        if (i == 0 || i == (getConsoleRectSize().x / 3) - 1){
            std::cout << "|";
        }
        else{
            std::cout << " ";
        }
    }

    json data = fetch("https://726b-181-66-156-128.sa.ngrok.io/cines");

    std::cout << data["cines_en_tu_ciudad"];


    std::cout << std::endl;
}