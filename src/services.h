#pragma once

#include <string>
#include <json.hpp>
#include <utf8.hpp>
#include "consoleUtils.h"
#include "services.h"

using json = nlohmann::json;

namespace gnu {

// La URL de la API donde anticine obtiene sus datos
// Repositorio: https://github.com/GNUfamilia-fisi/anticine-server
std::string apiURL = "http://localhost:6969";

/**
 * Hace una petición a una URL y devuelve su respuesta.
 *
 * Por defecto solo funciona con JSONs, por ejemplo,
 * un `fetch("www.google.com")` terminaría en error, pues
 * Google te devuelve una página web en formato HTML,
 * no texto en formato JSON.
 *
 * Por detrás, esta función usa gnu::exec() para ejecutar
 * el comando "curl", que sirve para hacer peticiones HTTP.
 *
 * @example
 * json response = gnu::fetch("https://api.anticine.up.railway.app")
 */
json fetch(std::string url) {
    std::string command = "curl -s " + url;

    // ejecutamos el comando en la terminal
    std::string fetchResult = gnu::exec(command);

    std::cout << "result: " << fetchResult << std::endl;

    json data;

    try {
        // Intentamos transformar la repuesta del servidor a JSON
        data = json::parse(fetchResult);
    }
    catch (...) {
        throw std::runtime_error("Error al intentar hacer fetch a " + url);
    }
    return data;
}

/**
 * Usa a "fetch" con la url de nuestra API como base
 *
 * Por ejemplo, el siguiente código:
 * json response = apifetch("/cines/cercanos")
 *
 * Es equivalente a:
 * json response = fetch(apiURL + "/cines/cercanos")
 */
json apifetch(std::string endpoint) {
    // Usamos la apiURL como base

    // Por ejemplo, si apiURL es "http://localhost:6969"
    // y endpoint es "/cines/cercanos", entonces

    // url = "http://localhost:6969/cines/cercanos"
    return fetch(apiURL + endpoint);
}

json apipost(std::string endpoint, json data) {
    // Usamos la apiURL como base
    std::string url = apiURL + endpoint;
    std::string raw_data = "";

    // Agregamos un "\" antes de cada comilla antes de enviar el JSON
    // {"a": "b", "c": "d"} -> {\"a\": \"b\", \"c\": \"d\"}
    for (auto ch : utf8::iterate(data.dump())) {
        if (ch == "\"") {
            raw_data += "\\\"";
        }
        else {
            raw_data += ch;
        }
    }

    std::string command = "curl -s -X POST -H \"Content-Type: application/json\" -d \"" + raw_data + "\" " + url;
    LOG_FILE("Comando: " << command << std::endl);
    std::string fetchResult = gnu::exec(command);

    json response;

    try {
        // Intentamos transformar la repuesta del servidor a JSON para retornarlo
        response = json::parse(fetchResult);
    }
    catch (...) {
        throw std::runtime_error("Error al intentar hacer fetch a " + url);
    }
    return response;
}

} // namespace gnu
