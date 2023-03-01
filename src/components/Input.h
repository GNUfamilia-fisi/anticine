#pragma once

#include <string>
#include <style.hpp>
#include "Drawable.h"
#include "../consoleUtils.h"

namespace gnu {

class Input : public Drawable {
    size_t max_length;
    size_t cursor_index = 0;
    bool focused = false;
    std::string label = "";
    // bool isPassword = false; (TODO)
  public:
    std::string text;

    struct handleInfo {
        std::string text = "";
        bool enter_pressed = false;
        bool escape_presed = false;
        bool tab_pressed = false;
        // el usuario presionó una tecla de ir al input anterior
        bool wanna_go_previous = false;
        // el usuario presionó una tecla de ir al input siguiente
        bool wanna_go_next = false;
    };

    Input(size_t length) : Drawable({ int(length + 4), 3 }) {
        this->max_length = length;
    }

    Input(std::string label, size_t length) : Drawable({ int(length + 4), 3 }) {
        this->max_length = length;
        this->label = label;
    }

    std::string getValue() { return this->text; }

    void setFocus(bool focus) {
        // Cada vez que cambia el focus, se redibuja el input
        // (por cuestiones del cursor)
        this->focused = focus;
        this->drawContent(); // no es necesario redibujar el borde
    }

    void draw() {
        this->drawBorder();
        this->drawContent();
    }

    /**
     * @brief Procesa un input cualquiera y cambia el estado del input según corresponda
     *
     * @param key input recibido, idealmente desde gnu::getch()
     * @return handleResult contiene información sobre el input procesado
     */
    handleInfo handle(int key) {
        handleInfo handle_result = {};
        if (key == 0) return handle_result;
        // Ninguna de estas teclas modifica el focus, es responsabilidad del
        // programador manejar el focus según el `handleResult`
        if (key == gnu::key::Enter) {
            handle_result.enter_pressed = true;
            handle_result.wanna_go_next = true;
            return handle_result;
        }
        if (key == gnu::key::ExitKey) {
            handle_result.escape_presed = true;
            return handle_result;
        }
        if (key == gnu::key::Tab) {
            handle_result.tab_pressed = true;
            handle_result.wanna_go_next = true;
            return handle_result;
        }
        if (key == gnu::key::Down) {
            handle_result.wanna_go_next = true;
            return handle_result;
        }
        if (key == gnu::key::Up) {
            handle_result.wanna_go_previous = true;
            return handle_result;
        }
        if (key == 224) {
            return handle_result;
        }
        this->handleNormalInput(key);
        return handle_result;
    }

    void drawContent() {
        int offset_y = 0;
        // Es importante redibujar todo el contenido hasta el final
        // para flushear inputs anteriores
        if (!this->label.empty()) {
            offset_y = 1;
        }
        gnu::gotoXY(this->position.x + 2, this->position.y + 1 + offset_y);
        gnu::print(this->text);
        for (size_t i = 0; i < this->size.x - this->text.size() - 3; i++) {
            gnu::print(" ");
        }

        // El cursor que se dibuja es una simulación de cursor usando style::inverse
        // TODO: color de cursor personalizable (tal vez innecesario)
        if (!this->focused) return;
        style::inverse();
        gnu::gotoXY(this->position.x + this->cursor_index + 2, this->position.y + 1 + offset_y);
        // Si el cursor está al final, solo dibujamos un espacio
        if (this->cursor_index == this->text.size()) {
            gnu::print(" ");
        }
        else {
            // Reescribimos el caracter que está debajo del cursor
            gnu::print(this->text.at(this->cursor_index));
        }
        style::reset();
    }

    void drawBorder() {
        int offset_y = 0;
        if (!this->label.empty()) {
            offset_y = 1;
            gnu::gotoXY(this->position.x, this->position.y);
            gnu::print(this->label);
        }
        gnu::gotoXY(this->position.x, this->position.y + offset_y);
        gnu::print("┌");
        for (int i = 0; i < this->size.x - 2; i++) {
            gnu::print("─");
        }
        gnu::print("┐");

        gnu::gotoXY(this->position.x, this->position.y + 1 + offset_y);
        gnu::print("│");
        gnu::gotoXY(this->position.x + this->size.x - 1, this->position.y + 1 + offset_y);
        gnu::print("│");

        gnu::gotoXY(this->position.x, this->position.y + 2 + offset_y);
        gnu::print("└");
        for (int i = 0; i < this->size.x - 2; i++) {
            gnu::print("─");
        }
        gnu::print("┘");
    }

  private:

    void handleNormalInput(int key) {
        // Manejando el input de "control"
        //LOG_FILE("key: " << key << "\n" << std::endl);
        // Borrar caracter
        if (key == gnu::key::Backspace) {
            if (this->cursor_index == 0) return;
            if (this->text.empty()) return;
            this->text.erase(this->cursor_index - 1, 1);
            this->cursor_index--;
            return;
        }
        // Suprimir caracter
        if (key == gnu::key::Supr) {
            if (this->cursor_index == this->text.size()) return;
            this->text.erase(this->cursor_index, 1);
            return;
        }
        // Navegación
        if (key == gnu::key::Left) {
            if (this->cursor_index == 0) return;
            this->cursor_index--;
            return;
        }
        if (key == gnu::key::Right) {
            if (this->cursor_index >= this->text.size()) return;
            this->cursor_index++;
            return;
        }
        // Inicio/Fin
        if (key == gnu::key::Home) {
            this->cursor_index = 0;
            return;
        }
        if (key == gnu::key::End) {
            this->cursor_index = this->text.size();
            return;
        }

        // El último caso es que se haya presionado un caracter dibujable
        if (this->text.size() >= this->max_length) return;
        if (!isPrintableAscii(key)) return;

        this->text.insert(this->cursor_index, 1, (char)key);
        this->cursor_index++;
    }

    // Nota: Esta función no se comporta como debería en UNIX para key > 127
    bool isPrintableAscii(int key) {
        return key >= 32 && key <= 255;
    }
};

} // namespace gnu

