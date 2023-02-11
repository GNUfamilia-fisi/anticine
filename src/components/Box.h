#pragma once

#include <string>
#include <vector>
#include "../consoleUtils.h"
#include "utf8.hpp"

namespace gnu {

class Canva {
  public:
    // std::vector<std::vector<gnu::color>> pixels;
    gnu::color pixels[256][256];
    gnu::vec2d size;
    void setPixel(gnu::vec2d px, gnu::color color) {
        pixels[px.x][px.y] = color;
    }
};

class Box {

  public:
    std::string id;
    std::string content;

    Canva canva;
    gnu::color color;
    gnu::vec2d size;

    Box(gnu::vec2d size) :  Box() {
        this->size = size;
    }
    Box() : id("nanoid") {
        this->size = { 1, 1 };
    }
    void draw() {
        Canva canva;
        canva.size = this->size;

        short last_space = 0;
        short last_break = 0;
        short i = 0;
        std::vector<std::vector<std::string>> lines;
        size_t total_len = (short)utf8::str_length(this->content);
        std::cout << "total_len: " << total_len << "\n";
        std::vector<std::string> utf_8str = utf8::iterate(this->content);
        // print the utf8 string to debug
        // for (auto &ch : utf_8str) {
        //     std::cout << ch << "\n";
        // }
        // for (const auto &ch : this->content) {
        for (const auto &ch : utf_8str) {
            // std::cout << "i: " << i << "\n";
            if (ch == " ") {
                last_space = i;
            }
            if (i + 1 == total_len) {
                // we are at the end, so push the rest
                // std::cout << "last line: " << i << "\n";
                std::vector<std::string> line;
                for (int j = last_break; j < i + 1; j++) {
                    line.push_back(utf_8str[j]);
                }
                lines.push_back(line);
            }
            else if (i - last_break >= this->size.x) {
                // std::cout << "last_break: " << last_break << "\n";
                if (last_space <= last_break) {
                    // Si esto se cumple es porque no hubo espacios desde el ultimo salto de linea
                    // std::cout << "no spaces\n";
                    i++;
                    continue;
                }
                // push from last_break to last_space (ignoring the first space)
                // last space = 4
                // last break = 4
                std::vector<std::string> line;
                for (int j = last_break; j < last_space; j++) {
                    line.push_back(utf_8str[j]);
                }
                lines.push_back(line);
                last_break = last_space + 1;
            }
            i++;
        }

        // print the lines to debug
        // for (auto &line : lines) {
        //     for (auto &ch : line) {
        //         std::cout << ch;
        //     }
        //     std::cout << std::endl;
        // }
        

        std::string row;
        for (int i = 0; i < this->size.x; i++) {
            row += "█";
        }

        short start_col = (this->size.y - lines.size()) / 2;
        if (start_col < 0) start_col = 0;
        for (int j = 0; j < this->size.y; j++) {
            if (j == start_col) {
                for (auto &line : lines) {
                    int start_row = (this->size.x - line.size()) / 2;
                    gnu::gotoXY(0, j++);
                    // gnu::setConsoleColor(this->color);

                    std::cout << "\x1B[38;2;" << 209 << ";" << 167 << ";" << 77 << "m";
                    for (int i = 0; i < start_row; i++) {
                        std::cout << "█";
                    }
                    std::cout << "\x1B[48;2;" << 209 << ";" << 167 << ";" << 77 << "m";
                    std::cout << "\x1B[38;2;" << 0 << ";" << 0 << ";" << 0 << "m"; // fg black
                    std::string line_str;
                    for (auto &ch : line) {
                        line_str += ch;
                    }
                    fputs(line_str.c_str(), stdout);
                    std::cout << "\x1B[38;2;" << 209 << ";" << 167 << ";" << 77 << "m";
                    for (int i = 0; i < start_row; i++) {
                        std::cout << "█";
                    }
                    std::cout << "█";
                }
                j--;
                continue;
            }
            gnu::gotoXY(0, j);
            std::cout << "\x1B[38;2;" << 209 << ";" << 167 << ";" << 77 << "m";
            fputs(row.c_str(), stdout);
        }
    }
};

} // namespace gnu

/* ANSI NOTES:

// E2 80 8E F0 9F 98 83
// fwrite("█", 1, 1, stdout);
// const char utf8_string[] = { '\xE2', '\x96', '\x88', '\0' };
// fwrite(utf8_string, 1, 3, stdout);
// const char utf8_string[] = { '\xF0', '\x9F', '\x98', '\x83', '\0' };
// std::cout << "█";

Definitiva:
fputs("█", stdout);
*/
