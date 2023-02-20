#pragma once

#include <string>
#include <cmath>
#include <fstream>
#include <json.hpp>
#include <cstdlib>
#include <ctime>

#include "../consoleUtils.h"
#include "../services.h"

using json = nlohmann::json;

namespace gnu {

std::string fechaSeleccionada; // variable global (no usada)

// Prototipos
void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize);

std::string menuCarteleraFinal();
std::string chooseCinemaScreen();
std::string menuDetalles();
std::string menuDiaAux();
std::string asientos();


// Variables globales
std::string cineID;
std::string movieID;
std::string selectedDate = "";


void menuSelector() {
    json menuRawData = gnu::apifetch("/cines/cercanos");
    cineID = menuRawData["nearest_id"].get<std::string>();

    std::string menuID = menuCarteleraFinal();

    while (true) {
        if (menuID == "selectorCineAux") {
            menuID = chooseCinemaScreen();
        }
        if (menuID == "selectorPelicula") {
            menuID = menuCarteleraFinal();
        }
        if (menuID == "menuDetalles") {
            menuID = menuDetalles();
        }
        if (menuID == "menuDiaAux") {
            menuID = menuDiaAux();
        }
        if(menuID=="Asientos"){
            menuID= asientos();
        }
    }
}


//para capturar el color o la posicion de los asientos
std::string codigos [5][15]={{"1A","1B","1C","1D","1E","1F","1G","1H","1I","1J","1K","1L","1M","1N","1P"},
                            {"2A","2B","2C","2D","2E","2F","2G","2H","2I","2J","2k","2L","2M","2N","2P"},
                            {"3A","3B","3C","3D","3E","3F","3G","3H","3I","3J","3k","3L","3M","3N","3P"},
                            {"4A","4B","4C","4D","4E","4F","4G","4H","4I","4J","4k","4L","4M","4N","4P"},
                            {"5A","5B","5C","5D","5E","5F","5G","5H","5I","5J","5k","5L","5M","5N","5P"}} ;   //wstring,
//forma de la butaca
std::string butaca[3] ={"┌──────┐","│      │","└─'  '─┘"};  

std::string asientos(){
    gnu::cls();
    //pantalla
    gnu::Box pantalla({135,2});
    pantalla.setBoxColor({ 20, 156, 178});
    //sin palabras en la pantalla que se ve feazo

    //hacer fondo mas grande para que parezcan pasillos
    pantalla.position={30,1};
    pantalla.showBorder=false;
    pantalla.draw();
    //borde para decorar los asientos
    gnu::Box caja({ 146, 25 });
    caja.setBoxColor({ 104,19,1 });
    caja.setFontColor({ 255, 138, 208 });
    caja.position = { 23, 7 };//30,5
    caja.showBorder = true;
    caja.transparent=true;
    caja.draw();

    //boton de seleccione el asiento
    gnu::Box boton({ 30, 3 });
    boton.setFontColor({ 255, 138, 208 });
    //boton.setBoxColor({184, 155, 231});
    boton.content="Seccione su asiento";
    boton.position = { 80, 34 };
    boton.showBorder = true;
    boton.draw();
    //caja para seleccionar asientos
    //dependiendo de esto hago esta vaina con colores sino sale
    
    //seleccionar las cajas
    gnu::Box seleccion ({7,3});
    seleccion.setBoxColor({  0, 29, 158 });
    seleccion.transparent=false;
    seleccion.showBorder=true;
    seleccion.position ={30,7};
    

    //no puedo hugo, ya quedo zzz
    int coordx=30;//ni idea por que esta wbd si pasa de 30 se loquea
    int coordy=7;//5
    int a=0,b=0;
    
    //avanzar de nuevo
    //azul = disponible
    //rojo ocupado
    //verde=seleccionado
    //crear nueva caja para seleccionar
    gnu:: Box opciones ({7,3});
    opciones.setBoxColor({  0, 29, 158 });
    opciones.setFontColor({255,255,255});

    for(short k=coordy;k<25+coordy;k+=5){//5 espacios
        for(short j=coordx;j<135+coordx;j+=9){//9 espacios
            opciones.position= {j,k};
            opciones.content=codigos[b][a];
            opciones.draw();
            //gotoXY(j,k);
            /*
            for(int i=0;i<sup;i++){
                gotoXY(j,i+k);
                gnu::print(butaca[i]);
            } */
         a++;      
        }
        a=0;
    b++;
    }
    b=0;


/*
    for(int k=coordy;k<25+coordy;k+=5){//5 espacios
        for(int j=coordx;j<135+coordx;j+=9){//9 espacios
            gotoXY(j+3,k+2);
            gnu::print(codigos[b][a]);//ya tenemos posicion en la que deben estar
                a++;
                Sleep(10);    
            }
            a=0;
            b++;
        }
        Sleep(10);
        b=0;
        a=0;
        */
    short c=30;
    short d=7;
    int input;
    while (true){
        seleccion.position={c,d};
        seleccion.setBoxColor({  0, 29, 158 });
        seleccion.draw();
        gnu::sleep(100);
        seleccion.setBoxColor({   28, 209, 61 });
        seleccion.draw();
        gnu::sleep(100);
        
        input = gnu::getch();
        if (input) {
            switch (input) {
            case gnu::key::Right:
                c+=9;
                break;
            case gnu::key::Left:
                c-=9;
                break;
            case gnu::key::Up:
                d-=5;
                break;
            case gnu::key::Down:
                d+=5;
                break;
            }
        }
    }

}

std::string menuDetalles() {
    gnu::cls();

    json movieRawData = gnu::apifetch("/cines/" + cineID + "/cartelera/" + movieID);
    json movieData = movieRawData["movie"];

    std::vector<json> castRaw = movieData["cast"].get<std::vector<json>>();
    std::vector<json> daysRaw = movieRawData["days"].get<std::vector<json>>();
    
    //all the session and versions data from the first (current) day
    json currentDayRaw = daysRaw[0];

    for (json obj : daysRaw) {
        if (obj["date"].get<std::string>() == selectedDate) currentDayRaw = obj;
    }
    if (selectedDate == "") currentDayRaw = daysRaw[0];

    std::string director;
    std::vector<std::string> cast;
    std::string title = movieData["title"].get<std::string>();
    std::string trailer_url = movieData["trailer_url"].get<std::string>();

    for (json obj : castRaw){
        if (obj["role"].get<std::string>() == "Director") director = obj["fullname"].get<std::string>();
        else cast.push_back(obj["fullname"].get<std::string>());
    }

    gnu::Box header({gnu::getConsoleSize().x , 10});
    header.showBorder = false;
    header.setBoxColor({ 184, 155, 231 }); //RGB(184, 155, 231)
    header.draw();

    //======== titulo ============
    gnu::gotoXY({0, 7});
    style::setBg({184, 155, 231});
    style::setFg({0, 0, 0});
    style::bold();
    gnu::printRawOffset(R"(
 /||\ |~|~  |\/| /||\ |   /||\ ||\  ~|~|_|[~  | | | /|(`|)  /\| | /|~|~| ||\/| /||\ || /|
/-|| \| |   |  |/-|| \|  /-|| \||/   | | |[_   \|\|/-|_)|   \X|_|/-| | |_||  |/-|| \||/-|
)", 33);
    style::reset_bg();
    style::setDefaultFg();

    //====== director ======
    gnu::gotoXY({33, 10});
    style::bold();
    style::italic();
    gnu::print("Dirigido por " + director);
    style::setDefaultFg();

    //======= poster ======
    gnu::Box poster({30, 20});
    poster.setBoxColor({255, 255, 102});
    poster.position = gnu::vec2d({1, 8});
    poster.draw();

    //====== cast ==========
    gnu::gotoXY({1, 30});
    style::bold();
    style::underline();
    gnu::print("REPARTO");
    style::setDefaultFg();

    //TODO: make it print in line until touching the border of the poster
    short i = 0;
    for (std::string actor : cast) {
        gnu::gotoXY({1, 32 + i});
        gnu::print(actor + "\n");
        i++;
    }

    //=========== synopsis =============== || TODO: put this shit somewhere lol
    gnu::Card synopsis(movieData, {gnu::getConsoleSize().x - 31, 5}, 1);
    synopsis.position = gnu::vec2d({32, 10});
    synopsis.showBorder = false;
    //synopsis.draw();

    //======= date selector =========
    gnu::Box date({14, 1});
    date.position = gnu::vec2d({short(getConsoleSize().x - 15) , 8});
    date.content = currentDayRaw["date"].get<std::string>();
    date.showBorder = false;
    date.flushBorders();
    date.transparent = true;
    date.draw();

    //====== versions ========= || HO00005111 = real 3D dob || HO00005115 = 2D
    std::vector<json> versions = currentDayRaw["movie_versions"].get<std::vector<json>>();
    std::string currentDay = currentDayRaw["date"].get<std::string>();
    
    gnu::Box versionBoxTemplate({
        gnu::getConsoleSize().x - 35,
        3
    });

    std::vector<gnu::Box> versionBoxes(versions.size(), versionBoxTemplate);
    std::string versionTag;
    std::string langTag;

    //===== type of movie label =====
    gnu::Box typeSubBox({15, 1});

    size_t maxLabelSize = 10;
    
    //========= drawing the version boxes with their type labels =======
    for (int i = 0; i < versionBoxes.size(); i++){
        if (i == 0) versionBoxes[i].setBorderColor({230,50, 50});
        else versionBoxes[i].defaultBorderColor = true;

        versionBoxes[i].position = gnu::vec2d({ 34, (14 + i * (versionBoxTemplate.size.y + 2)) });
        versionBoxes[i].draw();

        //redrawing the type label
        versionTag = versions[i]["version_tags"].get<std::string>();
        langTag = versions[i]["language_tags"].get<std::string>();

        typeSubBox.content = versionTag + " | " + langTag;
        typeSubBox.position = versionBoxes[i].position;
        typeSubBox.draw();
    }

    //======= hours =======
    std::vector<std::string> hours;
    std::vector<size_t> hourSizes; // we need the sizes to limit the input

    //=========== sessions are presented as small selectable hour labes ==========
    gnu::Box sessionTemplate({10, 1});
    std::vector<gnu::Box> sessionBoxes(3, sessionTemplate);

    sessionBoxes[0].setFontColor({ 80, 80, 80 });
    sessionBoxes[2].setFontColor({ 80, 80, 80 });

    int middleRelative = versionBoxes[0].position.x + (versionBoxes[0].size.x / 2);

    for (int j = 0; j < versionBoxes.size(); j++){    
        
        //first we want to store the hours so we can use them later on (including both the epmty ones for scrolling)
        hours.insert(hours.begin(), "");
        for (json session : versions[j]["sessions"]) {
            hours.push_back(session["hour"].get<std::string>());
        }
        hours.insert(hours.end(), "");

        hourSizes.push_back(hours.size());
        
        for (int i = 0; i < sessionBoxes.size(); i++){
            sessionBoxes[i].content = hours[i];
            sessionBoxes[i].showBorder = false;
            sessionBoxes[i].position.x = middleRelative - 25 + (20 * i);
            sessionBoxes[i].position.y = versionBoxes[j].position.y + 2;
            sessionBoxes[i].draw();
        }

        //we clean the hour vector for every box, this way we wont need vectors of vectors for the hours
        hours.clear();
    }


    int input;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();

    size_t hour_i = 0;
    size_t session_i = 0;
    size_t day_i = 0;
    
    unsigned char globalOpt = 2;
    unsigned char opt = 0;

    unsigned char lastGlobalOpt = 2;
    unsigned char lastOpt = 0;

    //======== main loop ==============
    while(true) {
        input = gnu::getch();

        //======= input switch ======
        switch (input) {
        case gnu::key::Right:
            if (hour_i < hourSizes[opt] - 3 && globalOpt == 2) hour_i++;
            break;
        case gnu::key::Left:
            if (hour_i > 0 && globalOpt == 2) hour_i--;
            break;
        case gnu::key::Up:
            if (opt > 0 && globalOpt == 2) {
                opt--;
                hour_i = 0;
                break;
            }

            if (opt == 0 && globalOpt > 0) globalOpt--;
            break;
        case gnu::key::Down:
            if (opt < versionBoxes.size() - 1 && globalOpt == 2) {
                opt++;
                hour_i = 0;
            }

            if (opt == 0 && globalOpt < 2) globalOpt++;
            break;
        case gnu::key::Enter:
            if (globalOpt == 1) return "menuDiaAux";
            if (globalOpt == 0) system(std::string("start " + trailer_url).c_str());

            if (globalOpt == 2) {
                return "Asientos";
            }
        }

        for (int j = 0; j < versionBoxes.size(); j++){    
            
            //first we want to store the hours so we can use them later on (including both the epmty ones for scrolling)
            hours.insert(hours.begin(), "");
            for (json session : versions[j]["sessions"]) {
                hours.push_back(session["hour"].get<std::string>());
            }
            hours.insert(hours.end(), "");
            
            for (int i = 0; i < sessionBoxes.size(); i++){
                if (opt == j) sessionBoxes[i].content = hours[i + hour_i];
                else sessionBoxes[i].content = hours[i];
                
                sessionBoxes[i].showBorder = false;
                sessionBoxes[i].position.x = middleRelative - 25 + (20 * i);
                sessionBoxes[i].position.y = versionBoxes[j].position.y + 2;
                sessionBoxes[i].draw();
            }

            //we clean the hour vector for every box, this way we wont need vectors of vectors for the hours
            hours.clear();
        }


        if (lastConsoleSize != gnu::getConsoleSize() || lastOpt != opt || lastGlobalOpt != globalOpt){
            if (lastOpt == opt) gnu::cls();

            //======= drawing every session box on resize ===========
            for (int i = 0; i < versionBoxes.size(); i++){
                versionBoxes[i].position = gnu::vec2d({ 34, (14 + i * (versionBoxTemplate.size.y + 2)) });

                if (i == opt && globalOpt == 2) versionBoxes[i].setBorderColor({230,50, 50});
                else versionBoxes[i].defaultBorderColor = true;

                versionBoxes[i].draw();

                //redrawing the type label
                versionTag = versions[i]["version_tags"].get<std::string>();
                langTag = versions[i]["language_tags"].get<std::string>();

                typeSubBox.content = versionTag + " | " + langTag;
                typeSubBox.position = versionBoxes[i].position;
                typeSubBox.draw();
            }

            //===== redrawing =====
            header.size = gnu::vec2d({gnu::getConsoleSize().x , 10});
            header.draw();
            
            if (globalOpt == 1){
                date.showBorder = true;
                date.setBorderColor({230, 50, 50});
            }

            else {
                date.showBorder = false;
                date.flushBorders();
            }

            date.draw();
            
            if (globalOpt == 0) poster.setBoxColor({230, 50, 50});
            else poster.setBoxColor({255, 255, 102});

            poster.draw();

            //====== director ======
            gnu::gotoXY({33, 10});
            style::bold();
            style::italic();
            gnu::print("Dirigido por " + director);
            style::setDefaultFg();

            //====== cast ==========
            gnu::gotoXY({1, 30});
            style::bold();
            style::underline();
            gnu::print("REPARTO");
            style::setDefaultFg();
        
            //TODO: make it print in line until touching the border of the poster
            short i = 0;
            for (std::string actor : cast) {
                gnu::gotoXY({1, 32 + i});
                gnu::print(actor + "\n");
                i++;
            }

            //======== titulo ============
            gnu::gotoXY({0, 7});
            style::setBg({184, 155, 231});
            style::setFg({0, 0, 0});
            style::bold();
            gnu::printRawOffset(R"(
 /||\ |~|~  |\/| /||\ |   /||\ ||\  ~|~|_|[~  | | | /|(`|)  /\| | /|~|~| ||\/| /||\ || /|
/-|| \| |   |  |/-|| \|  /-|| \||/   | | |[_   \|\|/-|_)|   \X|_|/-| | |_||  |/-|| \||/-|
)", 33);
            style::reset_bg();
            style::setDefaultFg();
        }

        lastConsoleSize = gnu::getConsoleSize();
        lastOpt = opt;
        lastGlobalOpt = globalOpt;

        gnu::sleep(5);
    }

    return "hola";
}

std::string menuDiaAux() {
    gnu::cls();
    std::string rawMessage = R"(                      
         _             _
 ___ ___| |___ ___ ___|_|___ ___ ___
|_ -| -_| | -_|  _|  _| | . |   | .'|
|___|___|_|___|_______|_|_____|_|__,|
                ____        _
 _ _ ___ ___   |  __|__ ___| |_ ___
| | |   | .'|  |  _| -_|  _|   | .'|
|___|_|_|__,|  |_| |___|___|_|_|__,|
)";

    gnu::printRawCenter(rawMessage);
    gnu::gotoY(15);

    json wholeRaw = gnu::apifetch("/cines/2705/cartelera/91074");
    std::vector<json> daysRaw = wholeRaw["days"].get<std::vector<json>>();
    
    std::vector<std::string> dates;

    for (json obj : daysRaw) dates.push_back(obj["date"].get<std::string>());
    dates.insert(dates.begin(), "");
    dates.insert(dates.end(), "");

    gnu::Box dateContainer({gnu::getConsoleSize().x - 4, 1});
    
    std::vector<gnu::Box> genericDateContainers(3, dateContainer);

    for (int i = 0; i < genericDateContainers.size(); i++) {
        genericDateContainers[i].content = dates[i];
        genericDateContainers[i].position = gnu::vec2d({0, 15 + 3 * i});
        genericDateContainers[i].showBorder = false;
        genericDateContainers[i].draw();
    }

    genericDateContainers[1].setFontColor({ 230, 50, 50 });
    genericDateContainers[1].showBorder = true;

    int input;
    unsigned char opt = 0;

    while (true) {
        input = gnu::getch();

        switch (input) {
        case gnu::key::Up:
            if (opt > 0) opt--;
            break;
        case gnu::key::Down:
            if (opt < dates.size() - 3) opt++;
            break;
        case gnu::key::Enter:
            selectedDate = dates[1 + opt];
            return "menuDetalles";
        }

        for (int i = 0; i < genericDateContainers.size(); i++) {
            genericDateContainers[i].content = dates[i + opt];
            genericDateContainers[i].position = gnu::vec2d({0, 15 + 3 * i});
            genericDateContainers[i].showBorder = false;
            genericDateContainers[i].draw();
        }
    }
}


std::string menuCarteleraFinal() {
    gnu::cls();
    std::string rawLogo = R"(
_______       __________      _____             
___    |________  /___(_)________(_)___________ 
__  /| |_  __ \  __/_  /_  ___/_  /__  __ \  _ \
_  ___ |  / / / /_ _  / / /__ _  / _  / / /  __/
/_/  |_/_/ /_/\__/ /_/  \___/ /_/  /_/ /_/\___/ 
)";
    
    srand(time(NULL));

    json rawCarteleraData = gnu::apifetch("/cines/" + cineID + "/cartelera");
    std::vector<json> billboard = rawCarteleraData["movies"].get<std::vector<json>>();    
    
    json emptyMovie = json::parse(R"({
        "title": "",
        "poster_url": "",
        "duration": 0,
        "rating": "",
        "corporate_film_id": "",
        "trailer_url": "",
        "synopsis": ""
    })");
    billboard.insert(billboard.begin(), emptyMovie);
    billboard.insert(billboard.end(), emptyMovie);

    gnu::Card card1(billboard[0], { 18, 15 }, 0);
    card1.position.y = 11;

    gnu::Card card2(billboard[1], { 20, 17 }, 0);
    card2.position.y = 10;
    

    gnu::Card card3(billboard[2], { 18, 15 }, 0);
    card3.position.y = 11;

    card1.setBoxColor({ 209, 167, 77 });
    card2.setBoxColor({ 102, 196, 127 });
    card3.setBoxColor({ 189, 121, 163 });

    //============== descripcion ===================
    gnu::Card descriptionCard(billboard[1], { 70, 11 }, 1);

    descriptionCard.defaultFontColor = true;
    descriptionCard.position.y = 33;
    descriptionCard.transparent = true;
    descriptionCard.showBorder = false;

    //====== box label for the current cinema we are in ==========
    std::string cinemaName;
    json cinemasListRaw = gnu::apifetch("/cines/cercanos");

    std::vector<json> cinemasList = cinemasListRaw["cinemas"].get<std::vector<json>>();

    for (int i = 0; i < cinemasList; i++){
        if (cinemasList[i]["cinema_id"].get<std::string>() == cineID) {
            cinemaName = cinemasList[i]["name"].get<std::string>();
            break;
        }
    }

    std::string cinemaLabel = "Estas en " + cinemaName;
    gnu::Box cinemaLabelBox({ short(cinemaLabel.size() + 4), 1 });
    cinemaLabelBox.content = cinemaLabel;
    cinemaLabelBox.transparent = true;
    cinemaLabelBox.showBorder = true;
    cinemaLabelBox.defaultFontColor = false;
    cinemaLabelBox.setFontColor({ 255, 138, 208 });
    cinemaLabelBox.position.y = 7;
    
    //=========== button indicators =================
    gnu::Box buttonLeft({ 6, gnu::getConsoleSize().y });
    buttonLeft.setBoxColor({ 35, 35, 35 });
    buttonLeft.position.x = 0;
    buttonLeft.content = "<-";
    buttonLeft.showBorder = false;
    buttonLeft.setFontColor({ 255,255,255 });

    gnu::Box buttonRight({ 6, gnu::getConsoleSize().y });
    buttonRight.setBoxColor({ 35, 35, 35 });
    buttonRight.position.x = gnu::getConsoleSize().x - 6;
    buttonRight.content = "->";
    buttonRight.showBorder = false;
    buttonRight.setFontColor({ 255,255,255 });

    int panel_i = 0;

    gnu::vec2d lastConsoleSize = gnu::getConsoleSize();
    std::string titleLabel = billboard[1]["title"].get<std::string>();
    std::string lastLabel = titleLabel;
    std::string titlePadding = "";

    //====== additionals ========
    std::string duration = std::to_string(billboard[1]["duration"].get<int>());
    std::string rating = billboard[1]["rating"].get<std::string>();
    std::string additionals = duration + " | " + rating;
    std::string lastAdditionals = additionals;
    std::string additionalsPadding = "";

    char opt = 0;
    
    int input;
    
    while (true) {
        // ---- Receive input ----
        input = gnu::getch();

        switch(input){
        case gnu::key::Right:
            if (panel_i < billboard.size() - 3 && opt == 1) panel_i++;
            break;
        case gnu::key::Left:
            if (panel_i > 0 && opt == 1) panel_i--;
            break;
        case gnu::key::Up:
            if (opt > 0) opt--;
            break;
        case gnu::key::Down:
            if (opt < 1) opt++;
            break;
        case gnu::key::Enter:
            if (opt == 1) {
                movieID = billboard[1 + panel_i]["corporate_film_id"].get<std::string>();
                return "menuDetalles";
            }
            else {
                return "selectorCineAux";
            }
            break;
        }

        card1.updateData(billboard[0 + panel_i]);
        card2.updateData(billboard[1 + panel_i]);
        card3.updateData(billboard[2 + panel_i]);

        descriptionCard.updateData(billboard[1 + panel_i]);
        descriptionCard.centerHorizontal();

        // ---- Update ----
        style::setFg({ 128, 186, 209 });
        gnu::printRawCenter(rawLogo);
        style::setFg({ 222, 197, 153 });

        card2.centerHorizontal();
        card1.setPosition( card2.position + gnu::vec2d{ (short)(-card1.size.x - 10), 1 } );
        card3.setPosition( card2.position + gnu::vec2d{ (short)(card2.size.x + 10), 1 } );

        card1.setBoxColor({ 209, 167, 77 });
        card2.setBoxColor({ 102, 196, 127 });
        card3.setBoxColor({ 189, 121, 163 });
 
        card1.showBorder = true;
        card2.showBorder = true;
        card3.showBorder = true;

        card1.cardDraw();
        card2.cardDraw();
        card3.cardDraw();

        gnu::gotoY(card2.position.y + card2.size.y + 2 + 1);
        titleLabel = billboard[1 + panel_i]["title"].get<std::string>();

        if (lastLabel.size() <= titleLabel.size()) {
            gnu::printLineCentered(titleLabel);            
        }
        else {
            for (int i = 0; i < (lastLabel.size() - titleLabel.size() + 1) / 2; i++){
                titlePadding += " ";
            }
            gnu::printLineCentered(titlePadding + titleLabel + titlePadding);
        }
        lastLabel = titleLabel;

        gnu::print("\n");
        //emojis
        std::string emojisRaw = billboard[1 + panel_i]["emojis"].get<std::string>();

        utf8::utf8_string_t emojis = utf8::iterate(emojisRaw);

        gnu::gotoX(gnu::getConsoleSize().x / 2 - 2);

        //TODO
        for (std::string h : emojis) {
            if (h == std::string("♂️")) continue;
            gnu::print(h);
        }

        gnu::print("\n");

        //additional data
        gnu::printLineCentered(additionals);
        
        duration = std::to_string(billboard[1 + panel_i]["duration"].get<int>());
        rating = billboard[1 + panel_i]["rating"].get<std::string>();
        additionals = duration + " | " + rating;

        if (lastAdditionals.size() <= additionals.size()) {
            gnu::printLineCentered(additionals);            
        }
        else {
            for (int i = 0; i < (lastAdditionals.size() - additionals.size() + 1) / 2; i++){
                additionalsPadding += " ";
            }
            gnu::printLineCentered(additionalsPadding + additionals + additionalsPadding);
        }
        lastAdditionals = additionals;


        descriptionCard.draw();

        buttonLeft.size.y = gnu::getConsoleSize().y;
        buttonRight.size.y = gnu::getConsoleSize().y;
        buttonRight.position.x = gnu::getConsoleSize().x - 7;

        //TODO: cinemaLabel = .....
        cinemaLabelBox.centerHorizontal();
        cinemaLabelBox.content = cinemaLabel;
        cinemaLabelBox.size.x = cinemaLabel.size() + 4;
        cinemaLabelBox.size.y = 1;

        if (opt == 0) {
            buttonRight.content = "";
            buttonLeft.content = "";
            buttonRight.transparent = true;
            buttonLeft.transparent = true;

            cinemaLabelBox.defaultFontColor = false;
            cinemaLabelBox.setFontColor({ 255, 138, 208 });
            cinemaLabelBox.setBordersVisible(true);
        }

        else {
            buttonRight.content = ">>";
            buttonLeft.content = "<<";
            buttonRight.transparent = false;
            buttonLeft.transparent = false;

            cinemaLabelBox.defaultFontColor = true;
            cinemaLabelBox.setBordersVisible(false);
        }
        cinemaLabelBox.draw();

        buttonLeft.draw();
        buttonRight.draw();


        // ----- Clear ----
        gnu::gotoXY(0, 0);
        gnu::vec2d size = gnu::getConsoleSize();
        if (gnu::getConsoleSize() != lastConsoleSize) {
            gnu::cls();
        }

        lastConsoleSize = gnu::getConsoleSize();
        titlePadding = "";
        additionalsPadding = "";

        gnu::sleep(5);
    }
}

std::string chooseCinemaScreen() {
    system("cls");

    // Retorna una lista de cines ordenados según la geolocalización
    // del usuario, el primero siempre es el más cercano.
    // Dependiendo de 
    
    json nearCinemasData = gnu::apifetch("/cines/cercanos");

    size_t currentCine = 0;
    int nameCursorPos = 0;
    short listLength = 5;
    unsigned short color = style::color::BLUE;
    bool lock = true;

    std::string elijaLocalLabel = R"(
         _             _
 ___ ___| |___ ___ ___|_|___ ___ ___
|_ -| -_| | -_|  _|  _| | . |   | .'|
|___|___|_|___|___|___|_|___|_|_|__,|
               _             _
    _ _ ___   | |___ ___ ___| |
   | | |   |  | | . |  _| .'| |
   |___|_|_|  |_|___|___|__,|_|
)";
    
    gnu::printRawCenter(elijaLocalLabel);

    int hit;
    
    // main menu loop
    while (lock) {
        gnu::gotoXY(0, 1);
        std::cout << std::endl;

        hit = gnu::getch();

        switch (hit) {
        case gnu::key::Down:

            for (size_t i = 0; i < listLength; i++) {
                gnu::cleanLine(20+i);
            }

            if (currentCine < nearCinemasData["cinemas"].size() - 1) {
                currentCine++;
            }
            if (nameCursorPos < listLength - 1) {
                nameCursorPos++;
            }
            else {
                nameCursorPos = listLength - 1;
            }
            break;
        case gnu::key::Up:
            
            for (size_t i = 0; i < listLength; i++) {
                gnu::cleanLine(20+i);
            }

            if (currentCine > 0) {
                currentCine--;
            }
            break;
        // Si selecciona un cine, lo guardamos en la variable global cinemaID
        // que será usado por el siguiente menú
        case gnu::key::Enter:
            cineID = nearCinemasData["cinemas"][currentCine]["cinema_id"].get<std::string>();
            lock = false;
            break;
        default:
            break;
        }

        gnu::cinemaListDisplay(nearCinemasData["cinemas"], currentCine, nameCursorPos, listLength);
        gnu::sleep(5);
    }

    return "selectorPelicula";
}

void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize) {
    style::setColor(style::color::WHITE);

    size_t indic = namePos;

    std::string displayName;

    if (closeData.size() < showSize) {
        showSize = closeData.size();
    }
    if (current < showSize) {
        indic = current;
    }

    // std::string names[showSize];
    std::string names[128];

    if (current < showSize) {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i]["name"].get<std::string>();
        }
    }
    else {
        for (size_t i = 0; i < showSize; i++) {
            names[i] = closeData[i + current - showSize + 1]["name"].get<std::string>();
        }
    }

    for (size_t i = 0; i < showSize; i++) {

        if (i == indic) {
            gnu::gotoXY(
                (gnu::getConsoleSize().x / 2) - names[i].length() - 1,
                20 + i
            );

            style::setColor(style::LIGHTGREEN);
            gnu::print(">>");
        }
        else {
            style::setColor(style::WHITE);
        }
        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (names[i].length() / 2),
            20 + i
        );

        gnu::print(names[i]);
    }
}

} // namespace gnu
