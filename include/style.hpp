#pragma once

#include <string>
#include <cmath>
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
    BLACK =         30,
    GRAY =          30,
    RED =           31,
    GREEN =         32,
    YELLOW =        33,
    BLUE =          34,
    MAGENTA =       35,
    CYAN =          36,
    WHITE =         37
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
    std::string txt = "\x1B[" + std::to_string(color) + "m";
    print(txt.c_str());
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

// Devuelve una versión más clara u oscura del baseColor para
// usarse como complemento de luminosidad
// https://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-black-depending-on-background-color
#define LIGHT_MULTIPLIER 2.f
#define DARK_MULTIPLIER 0.45f
#define LIGHT_MAX 240

style::rgb getComplementaryColor(style::rgb baseColor) {
    float r = baseColor.r / 255.0f;
    float g = baseColor.g / 255.0f;
    float b = baseColor.b / 255.0f;

    if (r < 0.03928f) { r = r / 12.92f; }
    else { r = powf((r + 0.055f) / 1.055f, 2.4f); }

    if (g < 0.03928f) { g = g / 12.92f; }
    else { g = powf((g + 0.055f) / 1.055f, 2.4f); }

    if (b < 0.03928f) { b = b / 12.92f; }
    else { b = powf((b + 0.055f) / 1.055f, 2.4f); }

    float L = 0.2126f * r + 0.7152f * g + 0.0722f * b;

    if (L > 0.179f) {
        // insted of returning black, we return a darker version of the base color
        return {
            (unsigned char)(baseColor.r * DARK_MULTIPLIER),
            (unsigned char)(baseColor.g * DARK_MULTIPLIER),
            (unsigned char)(baseColor.b * DARK_MULTIPLIER)
        };
    }
    else {
        // insted of returning white, we return a lighter version of the base color
        style::rgb complementaryColor;
        if (static_cast<int>(baseColor.r) * LIGHT_MULTIPLIER > LIGHT_MAX) {
            complementaryColor.r = LIGHT_MAX;
        }
        else {
            complementaryColor.r = static_cast<int>(baseColor.r) * LIGHT_MULTIPLIER;
        }
        if (static_cast<int>(baseColor.g) * LIGHT_MULTIPLIER > LIGHT_MAX) {
            complementaryColor.g = LIGHT_MAX;
        }
        else {
            complementaryColor.g = static_cast<int>(baseColor.g) * LIGHT_MULTIPLIER;
        }
        if (static_cast<int>(baseColor.b) * LIGHT_MULTIPLIER > LIGHT_MAX) {
            complementaryColor.b = LIGHT_MAX;
        }
        else {
            complementaryColor.b = static_cast<int>(baseColor.b) * LIGHT_MULTIPLIER;
        }
        return complementaryColor;
    }
}

} // namespace colors
