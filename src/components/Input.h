#pragma once

#include <string>
#include <style.hpp>
#include "Drawable.h"
#include "../consoleUtils.h"

namespace gnu {

class Input : public Drawable {
    size_t max_length;
    size_t cursor_index = 0;
    std::string value;
    bool focused = false;
    // bool isPassword = false; (TODO)
  public:

    struct handleInfo {
        std::string value = "";
        bool enter_pressed = false;
        bool escape_presed = false;
        bool tab_pressed = false;
        // el usuario presionó una tecla de ir al input anterior
        bool wanna_go_previous = false;
        // el usuario presionó una tecla de ir al input siguiente
        bool wanna_go_next = false;
    };

    Input(size_t length) : Drawable({ int(length + 2), 3 }) {
        this->max_length = length;
    }

    std::string getValue() { return this->value; }

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
        handleInfo result;
        this->handleInput(key, result);
        return result;
    }

    void drawContent() {
        // Es importante redibujar todo el contenido hasta el final
        // para flushear inputs anteriores
        gnu::gotoXY(this->position.x + 2, this->position.y + 1);
        gnu::print(this->value);
        for (size_t i = 0; i < this->size.x - this->value.size() - 1; i++) {
            gnu::print(" ");
        }

        // El cursor que se dibuja es una simulación de cursor usando style::inverse
        // TODO: color de cursor personalizable (tal vez innecesario)
        if (!this->focused) return;
        style::inverse();
        gnu::gotoXY(this->position.x + this->cursor_index + 2, this->position.y + 1);
        // Si el cursor está al final, solo dibujamos un espacio
        if (this->cursor_index == this->value.size()) {
            gnu::print(" ");
        }
        else {
            // Reescribimos el caracter que está debajo del cursor
            gnu::print(this->value.at(this->cursor_index));
        }
        style::reset();
    }

    void drawBorder() {
        gnu::gotoXY(this->position.x, this->position.y);
        gnu::print("┌");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print("─");
        }
        gnu::print("┐");

        gnu::gotoXY(this->position.x, this->position.y + 1);
        gnu::print("│");
        gnu::gotoXY(this->position.x + this->size.x + 1, this->position.y + 1);
        gnu::print("│");

        gnu::gotoXY(this->position.x, this->position.y + 2);
        gnu::print("└");
        for (int i = 0; i < this->size.x; i++) {
            gnu::print("─");
        }
        gnu::print("┘");
    }

  private:

    void handleNormalInput(int key) {
        // Manejando el input de "control"
        LOG_FILE("key: " << key << std::endl);
        // Borrar caracter
        if (key == gnu::key::Backspace) {
            if (this->cursor_index == 0) return;
            if (this->value.empty()) return;
            this->value.erase(this->cursor_index - 1, 1);
            this->cursor_index--;
            return;
        }
        // Suprimir caracter
        if (key == gnu::key::Supr) {
            if (this->cursor_index == this->value.size()) return;
            this->value.erase(this->cursor_index, 1);
            return;
        }
        // Navegación
        if (key == gnu::key::Left) {
            if (this->cursor_index == 0) return;
            this->cursor_index--;
            return;
        }
        if (key == gnu::key::Right && this->cursor_index < this->value.size()) {
            this->cursor_index++;
            return;
        }
        // Inicio/Fin
        if (key == gnu::key::Home) {
            this->cursor_index = 0;
            return;
        }
        if (key == gnu::key::End) {
            this->cursor_index = this->value.size();
            return;
        }
        // El último caso es que se haya presionado un caracter dibujable
        if (this->value.size() >= this->max_length) return;
        if (!isPrintableAscii(key)) return;


        this->value.insert(this->cursor_index, 1, (char)key);
        this->cursor_index++;
    }

    void handleInput(int key, handleInfo& result) {
        // Ninguna de estas teclas modifica el focus, es responsabilidad del
        // programador manejar el focus según el `handleResult`
        if (key == gnu::key::Enter) {
            result.enter_pressed = true;
            result.wanna_go_next = true;
            return;
        }
        if (key == gnu::key::ExitKey) {
            result.escape_presed = true;
            return;
        }
        if (key == gnu::key::Tab) {
            result.tab_pressed = true;
            result.wanna_go_next = true;
            return;
        }
        if (key == gnu::key::Down) {
            result.wanna_go_next = true;
            return;
        }
        if (key == gnu::key::Up) {
            result.wanna_go_previous = true;
            return;
        }
        this->handleNormalInput(key);
    }

    // Nota: Esta función no se comporta como debería en UNIX para key > 127
    bool isPrintableAscii(int key) {
        return key >= 32 && key <= 255;
    }
};

} // namespace gnu

