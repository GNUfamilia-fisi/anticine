#include <iostream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <windows.h>

#include "json.hpp"
#include "utilsDisplay.h"


using json = nlohmann::json;
using namespace nlohmann::literals;



COORD cursorPosition;


HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//
//
void gotoXY(int x, int y){
    cursorPosition = {x,y};
    SetConsoleCursorPosition(consoleWinHandle, cursorPosition);
}

std::string exec(const char* cmd){
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

json fetch(std::string f){

    std::string ToFetch = "curl -s " + f;

    std::string fetchedString = exec(ToFetch.c_str());


    json data = json::parse(fetchedString);

    return data;
}

consoleSize getConsoleRectSize(){
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    consoleSize obj = {
        BufferInfo.srWindow.Right - BufferInfo.srWindow.Left + 1,
        BufferInfo.srWindow.Bottom - BufferInfo.srWindow.Top + 1,
    };

    return obj;
}

void cleanLine(){
    for (int i = 0; i < getConsoleRectSize().x; i++){
        std::cout << " ";
    }
}