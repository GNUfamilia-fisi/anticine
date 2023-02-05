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

using json = nlohmann::json;


COORD pos;
HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);


void gotoXY(int x, int y){
    pos = {x,y};
    SetConsoleCursorPosition(consoleWinHandle, pos);
}

std::string exec(const char* cmd);


auto fetch(){

    std::string fetchedString = exec("curl -s https://726b-181-66-156-128.sa.ngrok.io");

    json data = json::parse(fetchedString);

    return data;
}


std::string exec(const char* cmd) {
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
