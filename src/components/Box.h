#pragma once

#include <string>
#include <vector>
#include <utf8.hpp>
#include <style.hpp>
#include "../consoleUtils.h"

namespace gnu {

class Box {
  public:
    std::string content;
    bool transparent = true;
    bool showBorder = true;
    bool alreadyFlushed = false;
    bool defaultFontColor = true;
    bool defaultBorderColor = true;

    std::string fill = "█";

    style::rgb box_color = { 255, 255, 255 };
    style::rgb text_color = { 0, 0, 0 };
    style::rgb border_color = { 0, 0, 0 };

    gnu::vec2d size = { 5, 5 };
    gnu::vec2d position = { 0, 0 };
    
    short padding = 1;

    Box(gnu::vec2d size) {
        this->size = size;
    }
    void setPosition(gnu::vec2d position) {
        this->position = position;
    }
    void setBoxColor(style::rgb rgb_color) {
        this->box_color = rgb_color;
        this->transparent = false;
    }
    void setFontColor(style::rgb font_color) {
        this->text_color = font_color;
        this->defaultFontColor = false;
    }
    void setFontColor(int r, int g, int b) {
        this->text_color = { (unsigned char)r, (unsigned char)g, (unsigned char)b };
        this->defaultFontColor = false;
    }
    void setBorderColor(style::rgb border_color) {
        this->border_color = border_color;
        this->defaultBorderColor = false;
    }
    void drawBorder() {
        // Dibujamos el marco
        style::setFg(this->box_color);
        gnu::gotoXY(this->position.x - 1, this->position.y - 1);
        gnu::print("┌");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print("─");
        }
        gnu::print("┐");
        for (int i = 0; i < this->size.y; i++) {
            gnu::gotoXY( this->position.x - 1, this->position.y + i);
            gnu::print("│");
            gnu::gotoXY(this->position.x + this->size.x, this->position.y + i);
            gnu::print("│");
        }
        gnu::gotoXY(this->position.x - 1, this->position.y + this->size.y);
        gnu::print("└");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print("─");
        }
        gnu::print("┘");
        style::reset_fg();
        style::reset_bg();
    }

    void flushBorders (){
        style::setFg(this->box_color);
        gnu::gotoXY(this->position.x - 1, this->position.y - 1);
        gnu::print(" ");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print(" ");
        }
        gnu::print(" ");
        for (int i = 0; i < this->size.y; i++) {
            gnu::gotoXY( this->position.x - 1, this->position.y + i);
            gnu::print(" ");
            gnu::gotoXY(this->position.x + this->size.x, this->position.y + i);
            gnu::print(" ");
        }
        gnu::gotoXY(this->position.x - 1, this->position.y + this->size.y);
        gnu::print(" ");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print(" ");
        }
        gnu::print(" ");
        style::reset_fg();
        style::reset_bg();        
    }

    void setBordersVisible(bool flag) {
        if (!flag){
            if (this->alreadyFlushed) return;

            flushBorders();
            this->alreadyFlushed = true;
            this->showBorder = false;
        }
        else {
            this->alreadyFlushed = false;
            this->showBorder = true;
        }
    }

    void draw() {
        style::reset_fg();

        /**
         * Para dibujar, necesitamos romper el contenido de la caja en varias
         * líneas, para ello, usamos un algoritmo de word wrap.
         */

        // Variables de seguimiento:
        size_t last_space = 0;
        size_t last_break = 0;
        // Array de líneas
        std::vector<std::string> lines;
        // Para iterarlo apropiadamente, usamos las utilidades de utf8
        size_t total_len = utf8::str_length(this->content);
        utf8::utf8_string_t utf_8str = utf8::iterate(this->content);

        // Iteramos sobre cada caracter (utf8)
        size_t i = 0;
        for (const auto &ch : utf_8str) {
            if (ch == " ") {
                // hacemos seguimiento de los espacios
                last_space = i;
            }
            if (ch == "\n") {
                // Si encontramos un salto de línea, cortamos
                // hasta donde estemos (sin incluir el salto de línea)
                std::string line;
                for (size_t j = last_break; j < i; j++) {
                    line += utf_8str[j];
                }
                lines.push_back(line);
                last_break = i + 1; // +1 para saltar el salto de línea
            }
            if (i - last_break >= (size_t)this->size.x - this->padding * 2) {
                // Si esto se cumple es porque llegamos al límite
                // pero no hemos encontrado un espacio en donde cortar
                if (last_space <= last_break) {
                    // simplemente continuamos, esto provocará overflow
                    i++; continue;
                }
                std::string line;
                for (size_t j = last_break; j < last_space; j++) {
                    line += utf_8str[j];
                }
                lines.push_back(line);
                last_break = last_space + 1;
            }
            // Si llegamos al final de todo el contenido,
            // pusheamos el resto de caracteres
            if (i + 1 == total_len) {
                std::string line;
                for (size_t j = last_break; j < i + 1; j++) {
                    line += utf_8str[j];
                }
                lines.push_back(line);
            }
            i++;
        }

        std::string emptyRow;

        // Si es transparente, utilizamos relleno con espacios y color de fondo negro
        // (hacemos una copia para no modificar los valores originales)
        char* filling = (char*)(this->transparent ? " " : this->fill.c_str());
        style::rgb fill_color = this->transparent ? style::rgb({ 0, 0, 0 }) : this->box_color;

        for (int i = 0; i < this->size.x; i++) {
            emptyRow += filling;
        }

        short start_col = (this->size.y - lines.size()) / 2;
        if (start_col < 0) start_col = 0;

        style::setFg(fill_color);
        for (short y = 0; y < this->size.y; ) {
            if (y != start_col || lines.size() == 0) {
                gnu::gotoXY(this->position.x, this->position.y + y);
                gnu::print(emptyRow);
                y++;
                continue;
            }
            // Dibujamos las líneas
            for (auto &line : lines) {
                size_t len = utf8::str_length(line);
                int start_row = (this->size.x - len) / 2;
                gnu::gotoXY(this->position.x, this->position.y + y++);

                // padding left
                gnu::print(gnu::repeat(filling, start_row));

                // text
                if (this->transparent) {
                    if (defaultFontColor) style::setDefaultFg();
                    else style::setFg(this->text_color);
                }
                else {
                    style::setFg(this->text_color);
                    style::setBg(fill_color);
                }
                gnu::print(line);

                // padding right
                style::reset_bg();
                style::setFg(fill_color);
                gnu::print(gnu::repeat(filling, this->size.x - start_row - len));
            }
        }
        style::reset_fg();
        style::reset_bg();
        if (this->showBorder) {
            this->drawBorder();
        }
    }

    void centerHorizontal() {
        this->position.x = (gnu::getConsoleSize().x - this->size.x) / 2;
    }
    void centerVertical() {
        this->position.y = (gnu::getConsoleSize().y - this->size.y) / 2;
    }
};

} // namespace gnu
