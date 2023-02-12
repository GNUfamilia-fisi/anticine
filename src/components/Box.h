#pragma once

#include <string>
#include <vector>
#include <utf8.hpp>
#include <colors.hpp>
#include "../consoleUtils.h"

namespace gnu {

class Box {
  public:
    std::string content;
    bool transparent = true;

    gnu::rgb rgb_color;
    gnu::rgb text_rgb_color = { 0, 0, 0 };
    gnu::vec2d size;
    gnu::vec2d position;

    Box(gnu::vec2d size) {
        this->size = size;
        this->position = { 0, 0 };
    }
    Box() {
        this->size = { 5, 5 };
        this->position = { 0, 0 };
    }
    void setPosition(gnu::vec2d position) {
        this->position = position;
    }
    void setColor(gnu::rgb rgb_color) {
        this->rgb_color = rgb_color;
        this->transparent = false;
    }
    void setFontColor(gnu::rgb font_color) {
        this->text_rgb_color = font_color;
    }
    void draw() {
        short last_space = 0;
        short last_break = 0;
        short i = 0;
        // std::vector<utf8::utf8_string_t> lines;
        std::vector<std::string> lines;
        size_t total_len = utf8::str_length(this->content);
        utf8::utf8_string_t utf_8str = utf8::iterate(this->content);

        for (const auto &ch : utf_8str) {
            // hacemos seguimiento de los espacios
            if (ch == " ") {
                last_space = i;
            }
            
            if (i - last_break >= this->size.x) {
                // Si esto se cumple es porque llegamos al límite
                // pero no hemos encontrado un espacio en donde cortar
                if (last_space <= last_break) {
                    // simplemente continuamos, esto provocará overflow
                    i++; continue;
                }
                std::string line;
                for (int j = last_break; j < last_space; j++) {
                    line += utf_8str[j];
                }
                lines.push_back(line);
                last_break = last_space + 1;
            }
            // Si llegamos al final de todo el contenido,
            // pusheamos el resto de caracteres
            if (i + 1 == total_len) {
                std::string line;
                for (int j = last_break; j < i + 1; j++) {
                    line += utf_8str[j];
                }
                lines.push_back(line);
            }
            i++;
        }

        std::string emptyRow;
        for (int i = 0; i < this->size.x; i++) {
            emptyRow += "█";
        }

        short start_col = (this->size.y - lines.size()) / 2;
        if (start_col < 0) start_col = 0;

        gnu::setFgRGBColor(this->rgb_color);
        for (short y = 0; y < this->size.y; ) {
            if (y != start_col) {
                gnu::gotoXY(position.x, (short)(position.y + y));
                gnu::print(emptyRow);
                y++;
                continue;
            }
            for (auto &line : lines) {
                size_t len = utf8::str_length(line);
                int start_row = (this->size.x - len) / 2;

                gnu::gotoXY(position.x, (short)(position.y + y++));
                // padding left
                gnu::print(gnu::repeat("█", start_row));

                gnu::setBgRGBColor(this->rgb_color);
                gnu::setFgRGBColor(this->text_rgb_color);
                gnu::print(line);

                // padding right
                gnu::setFgRGBColor(this->rgb_color);
                gnu::print(gnu::repeat("█", this->size.x - start_row - len));
            }
        }
        gnu::resetColor();
    }
};

} // namespace gnu
