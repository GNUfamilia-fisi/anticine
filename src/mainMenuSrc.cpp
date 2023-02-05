#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>

#include "utilsDisplay.h"

void logoDisplay(DWORD color);


HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

//refer to utisDisplay.h for the complete list of foreground colors



int main(){
    system("cls");

    DWORD color = 1;

    std::string load = "          ";

    short pos = 0;

    //main menu loop
    while(1){

        load[pos] = '>';

        gotoXY(0,0);
        
        logoDisplay(color);

        color++;

        if(color == 15){
            color = 0;
        }

        std::cout << "\n\n" << load << std::endl;

        pos++;


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
