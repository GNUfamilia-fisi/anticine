#pragma once

#include <string>
#include <vector>
#include <json.hpp>
#include "../consoleUtils.h"

using json = nlohmann::json;

// Variables globales compartidas por todos los menús
// (en un futuro intentaremos hacerlo más elegante, por ahora funciona)

std::string g_cineID = "";
/*
cineData: {
    cinema_id: "600",
    name: "Anticine Angamos",
    city: "Lima",
    address: "Av. Angamos Nº 1803 y/o Av. Tomas Marsano Nº 961- Surquillo"
}
*/
json g_cineData;

std::string g_movieID = "";

/*
interface CinemaMovieInformation {
  corporate_film_id: string;
  title: string;
  synopsis: string;
  emojis: string | null; // added
  version_tags: string; // las tags de todas las versiones
  trailer_url: string;
  thumbnail_url: string;
  average_thumbnail_color: RGBColor;
  raw_thumbnail_image: string;
  duration: number;
  rating: MovieRating;
  cast: MovieCast[];
}

interface MovieCast {
  fullname: string;
  role: 'Actor' | 'Director';
}

type MovieRating = (
  "APT (PG)" | // Apto para Todos
  "G"        | // General Exhibition
  "M"        | // Mature Audiences
  "M14"      | // Mayor a 14
  "M18"      | // Mayor a 18
  "PG"       | // Parental Guidance Recommended
  "R"        | // Restricted 16+ unless parent/guardn
  "R16"      | // Restricted To 16+
  "R18"      | // Restricted To 18+
  "TBC"        // To be confirmed
);
*/
json g_movieData;

// obtenida al seleccionar un horario
std::string g_sessionID = "";
// Usado en menuDetallePelicula.h -> std::string submenuSeleccionarFecha()
std::string g_selectedDate = "";
// solo disponible si está logueado
std::string g_userID = "";

/*
sessionData: {
    session_id: string,
    day: string,
    hour: string
}
*/
json g_sessionData;

/*
interface MovieVersion {
  corporate_film_id: string;
  movie_version_id: string; // film_HOPK
  title: string;
  version_tags: string;
  language_tags: string;
  seats_tags: string;
}
*/
json g_movieVersion;

/*
userData: {
    id: string,
    fullname: string,
    DNI: string,
    email: string,
    password: string
}
*/
json g_userData;
/*
Cada json tiene la forma: {
    "row_number": number,
    "col_number": number
}
*/
std::vector<json> g_selectedSeats; // guardadas en menuAsientos.h

// Variables modificadas en login y register
bool g_usuarioLogueado = false;
bool g_usuarioInvitado = false;
