#pragma once

#include <string>
#ifdef _WIN32
// En windows, usamos windows.h para los colores básicos definidos
#   include <Windows.h>
#endif

namespace style {

#ifdef _WIN32
    // Variable global para el handle de la consola proveída por Windows.h
    // Esta variable no debería ser usada directamente, sino a través de funciones
HANDLE colorConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void print(std::string str) {
    WriteConsole(colorConsoleHandle, str.c_str(), str.length(), NULL, NULL);
}
#else
void print(std::string str) {
    fputs(str.c_str(), stdout);
    fflush(stdout);
}
#endif

// Para acceder al código de un color escribe
//  color::BLACK, por ejemplo
// Estos pueden ser usados en funciones como
enum color : unsigned short {
#ifdef _WIN32
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
#else
    BLACK = 30,
    BLUE = 32,
    GREEN = 32,
    CYAN = 36,
    RED = 31,
    MAGENTA = 35,
    BROWN = 33,
    LIGHTGRAY = 37,
    GRAY = 30,
    LIGHTBLUE = 34,
    LIGHTGREEN = 32,
    LIGHTCYAN = 36,
    LIGHTRED = 31,
    LIGHTMAGENTA = 35,
    YELLOW = 33,
    WHITE = 37
#endif
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
#ifdef _WIN32
    SetConsoleTextAttribute(colorConsoleHandle, color);
#else
    std::string txt = "\x1B[" + std::to_string(color) + "m";
    print(txt.c_str());
#endif
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

void setDefaultFg() {
    std::string txt = "\x1B[0m";
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
