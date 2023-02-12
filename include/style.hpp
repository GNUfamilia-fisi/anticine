#pragma once

#include <string>
#include <Windows.h>

namespace style {

HANDLE colorConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void print(std::string str) {
    WriteConsole(colorConsoleHandle, str.c_str(), str.length(), NULL, NULL);
}

// Para acceder al código de un color escribe
//  color::BLACK, por ejemplo
// Estos pueden ser usados en funciones como
enum color : unsigned short {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHTGRAY = 7,
    GRAY = 8,
    LIGHTBLUE = 9,
    LIGHTGREEN = 10,
    LIGHTCYAN = 11,
    LIGHTRED = 12,
    LIGHTMAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

struct rgb {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
};

/**
 * Solo colores predefinidos por Windows.h
 * 
 * @example
 * setColor(style::color::RED);
 * setColor(style::color::LIGHTRED);
*/

void setColor(style::color color) {
    SetConsoleTextAttribute(colorConsoleHandle, color);
}
/**
 * Colores RGB para los caracteres
 * 
 * @example
 * style::setFg({255, 0, 0}); // Rojo
*/
void setFg(style::rgb color) {
    std::string txt = "\x1B[38;2;" +
		std::to_string(color.r) + ";" +
		std::to_string(color.g) + ";" +
		std::to_string(color.b) + "m";
    print(txt.c_str());
}
/**
 * Colores RGB para el fondo de los caracteres
 * 
 * @example
 * style::setBg({255, 0, 0}); // Rojo
 * style::setBg({ 127, 215, 84 }); // Verde
*/
void setBg(style::rgb color) {
    std::string txt = "\x1B[48;2;" +
		std::to_string(color.r) + ";" +
		std::to_string(color.g) + ";" +
		std::to_string(color.b) + "m";
    print(txt.c_str());
}

// Para definir una serie de códigos ANSI

#define ANSI_ESCAPE_CODE(name, code) \
	void name() \
	{ \
		print("\x1B[" #code "m"); \
	} \

// ANSI scape codes
// https://github.com/mvorbrodt/blog/blob/master/src/ansi_escape_code.hpp

// Para usarlos, solo deben llamarse por su nombre definido
// style::bold();
// style::underline();
// style::blink();
// etc.

ANSI_ESCAPE_CODE(reset, 0)
ANSI_ESCAPE_CODE(bold, 1)
// ANSI_ESCAPE_CODE(faint, 2) // font weight (no soportado, usar bold)
ANSI_ESCAPE_CODE(italic, 3)
ANSI_ESCAPE_CODE(underline, 4)
ANSI_ESCAPE_CODE(blink, 5)
// ANSI_ESCAPE_CODE(fast_blink, 6)
ANSI_ESCAPE_CODE(inverse, 7)
// ANSI_ESCAPE_CODE(conceal, 8)
// ANSI_ESCAPE_CODE(crossed_out, 9)
ANSI_ESCAPE_CODE(underline_off, 24)
ANSI_ESCAPE_CODE(blink_off, 25)
ANSI_ESCAPE_CODE(reset_fg, 39)
ANSI_ESCAPE_CODE(reset_bg, 49)

} // namespace colors
