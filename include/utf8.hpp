#pragma once

#include <string>
#include <vector>

namespace utf8 {

// std::string por defecto usa ASCII encoding
// UTF-8 es retrocompatible con ASCII, pero solo para
// el primer byte (1 code unit)

// Aprovecharemos esta retrocompatibilidad para
// poder usar std::string como si fuera UTF-8

// Lamentablemente, UTF-8 al ser un "variable length encoding",
// métodos como std::string::length() no funcionan correctamente.

// Además, si intentamos iterar sobre un std::string
// como si fuera un array de caracteres UTF-8, no funcionará,
// pues irá de 1 en 1 byte, y no necesariamente de 1 en 1 caracter.

// Para entender mejor UTF-8, ver:
// https://stackoverflow.com/a/50407375

// Esta pequeña librería escrita por la GNU-familia
// solo tiene dos funciones, una para obtener la longitud
// de un string UTF-8, y otra para iterar sobre sus caracteres.

// Eso sí, no podemos seguir utilizando "char" para representar
// un caracter, pues tiene un rango limitado.

// Aunque suene poco intuitivo, usaremos un std::string
// para representar un caracter UTF-8
typedef std::string utf8_char_t;

// Y por lo tanto, un string UTF-8 será un array
// de caracteres UTF-8
typedef std::vector<utf8_char_t> utf8_string_t;

// Esto nos será útil para poder iterar entre
// los code units de un caracter UTF-8
typedef unsigned char byte;

/**
 * Esta función devuelve la longitud de un std::string
 * que contiene caracteres codificados en UTF-8
 * 
 * @example
 * std::string str = "áéíóú 🙀👌👌";
 * str.length(); // 23         (incorrecto)
 * utf8::str_length(str); // 9 (correcto)
*/
std::size_t str_length(std::string &str) {

#if defined(UTF8_STRICT_COUNT)
    size_t character_count = 0;
    short i = 0;
    // Iterando sobre los code units (de 1 en 1 byte)
    while (i < str.length()) {
        short char_length = 0;
        byte c = str[i];
        // single-byte character
        if ((c & 0x80) == 0) char_length = 1;
        // two-byte character
        else if ((c & 0xe0) == 0xc0) char_length = 2;
        // three-byte character
        else if ((c & 0xf0) == 0xe0) char_length = 3;
        // four-byte character
        else if ((c & 0xf8) == 0xf0) char_length = 4;

        i += char_length;
        character_count++;
    }
    return character_count;
#else
    std::size_t len = 0;
    // std::string -> char*
    char* s = (char*)str.data();

    while (*s) len += (*s++ & 0xc0) != 0x80;
    return len;
#endif
}


std::vector<utf8_char_t> iterate(std::string str) {
    std::vector<utf8_char_t> utf8_string;
    for (std::string::const_iterator it = str.begin(); it != str.end();) {
        // single-byte code unit
        // La misma idea que la función str_length, pero
        // en vez de contar, vamos guardando los caracteres
        byte c = *it;
        short char_len = 0;
        if ((c & 0b10000000) == 0b00000000) {
            char_len = 1;
        }
        else if ((c & 0b11100000) == 0b11000000) {
            char_len = 2;
        }
        else if ((c & 0b11110000) == 0b11100000) {
            char_len = 3;
        }
        else if ((c & 0b11111000) == 0b11110000) {
            char_len = 4;
        }

        // Ahora que sabemos la longitud del caracter,
        // podemos guardar los code units
        utf8_char_t utf8_char;
        for (short i = 0; i < char_len; i++) {
            utf8_char += *it++; // empujamos el code unit
        }

        // Y por último, guardamos el caracter utf8
        utf8_string.push_back(utf8_char);
    }
    return utf8_string;
}

} // namespace utf8
