#pragma once

#include <Windows.h>
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

std::string fecha;

// Prototipos
void carteleraFechaScreen();
void displayDate(std::string fulldate, short opt);
void cinemaListDisplay(json closeData, size_t current, size_t namePos, size_t showSize);
void logoDisplay3D(style::color color);
void displayLogo2D(style::color color);
void loadingScreen();

std::string menuCarteleraFinal();
std::string chooseCinemaScreen();
std::string menuDetalles();
std::string asientos();


// Variables globales
std::string cineID;
std::string movieID;


void menuSelector() {
    json menuRawData = gnu::apifetch("/cines/cercanos");
    cineID = menuRawData["nearest_id"].get<std::string>();

    std::string menuID = menuCarteleraFinal();

    while (true) {
        if (menuID == "selectorCineAux") {
            menuID = chooseCinemaScreen();//elejir cines
        }
        if (menuID == "selectorPelicula") {
            menuID = menuCarteleraFinal();
        }
        if (menuID == "menuDetalles") {
            menuID = menuDetalles();//seleccionar horario
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
    system("cls");
    gnu::Box caja({ 135, 25 });
    caja.setFontColor({ 255, 138, 208 });
    caja.position = { 30, 5 };
    caja.showBorder = true;
    caja.draw();

    int coordx=30;//ni idea por que esta wbd si pasa de 30 se loquea
    int coordy=5;//5
    int a=0,b=0;
    
    int sup=3;
    for(int k=coordy;k<25+coordy;k+=5){//5 espacios
        for(int j=coordx;j<135+coordx;j+=9){//9 espacios
            gotoXY(j,k);
            for(int i=0;i<sup;i++){
                gotoXY(j,i+k);
                std::cout<<butaca[i];
            }    
        }

    }


    while (true){

    for(int k=coordy;k<25+coordy;k+=5){//5 espacios
        for(int j=coordx;j<135+coordx;j+=9){//9 espacios
            gotoXY(j+3,k+2);
            std::cout<<codigos[b][a];//ya tenemos posicion en la que deben estar
                a++;
                Sleep(10);    
            }
            a=0;
            b++;
        }
        Sleep(10);
        b=0;
        a=0;

        for(int k=coordy;k<25+coordy;k+=5){//5 espacios
            for(int j=coordx;j<135+coordx;j+=9){//9 espacios
                gotoXY(j+3,k+2);
                std::cout<<codigos[b][a];//ya tenemos posicion en la que deben estar
                a++;
                Sleep(100);   
            }
            a=0;
            b++;
        }
        a=0;b=0;
    }

}



std::string menuDetalles() {
    gnu::cls();
    gnu::Box header({gnu::getConsoleSize().x , 7});
    header.showBorder = false;
    header.setBoxColor({ 184, 155, 231 }); //RGB(220, 10, 180)
    header.draw();

    gnu::Box date({14, 1});
    date.position = gnu::vec2d({getConsoleSize().x - 14 , 6});
    date.content = "▶ 2023-05-12";
    date.showBorder = false;
    date.transparent = true;
    date.draw();

    gnu::Box poster({35, 20});
    poster.setBoxColor({150, 180, 235});
    poster.position = gnu::vec2d({1, 6});
    poster.draw();

    std::vector<std::string> footerDates = {"09:00am", "12:00am", "01:00pm", "02:00pm", "01:00pm","03:45pm","05:30pm","07:00pm"};
    
    gnu::Box footerTemplate({9, 1});
    
    std::vector<gnu::Box> footers(footerDates.size(), footerTemplate);

    for (int i = 0; i < footers.size(); i++) {
        int index = ceil((double) (230 / footers.size()) * i);
        footers[i].content = footerDates[i];
        footers[i].transparent = true;
        footers[i].setFontColor({ 230 - index, 230 - index, 230 - index });
        footers[i].position = gnu::vec2d({
            (getConsoleSize().x / 2) - 4 + (9 * i),
            gnu::getConsoleSize().y - 2
        });
        footers[i].showBorder = false;
    }

    gnu::Box emptyFooter({9, 1});
    emptyFooter.content = "       ";
    emptyFooter.setFontColor({ 30, 30, 30 });
    emptyFooter.transparent = true;
    emptyFooter.position = gnu::vec2d({
        (getConsoleSize().x / 2) - 13,
        gnu::getConsoleSize().y - 2
    });
    emptyFooter.showBorder = false;
    footers.insert(footers.begin(), emptyFooter);

    int input = '\0';
    size_t hour_i = 0;
    while(true) {
        input = '\0';
        if (_kbhit()) input = _getch();

        switch (input) {
        case gnu::key::Right:
            if (hour_i < footers.size() - 2) hour_i++;
            break;
        case gnu::key::Left:
            if (hour_i > 0) hour_i--;
            break;
        case gnu::key::Enter:
            fecha = footerDates[hour_i];
            return "Asientos";
            break;
        }
        
        if (1 <= hour_i) footers[0].content = footerDates[hour_i - 1];
        else footers[0].content = "       ";
        footers[0].draw();

        for (int i = 1; i < footers.size(); i++) {
            if (i + hour_i - 1 < footerDates.size()) footers[i].content = footerDates[i + hour_i - 1];
            else footers[i].content = "       ";

            footers[i].draw();
        }

        std::cout << std::endl;

        gnu::printLineCentered("▲");

        gnu::sleep(50);
    };

    return "hola";
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
    gnu::Box cinemaLabelBox({ cinemaLabel.size() + 4, 1 });
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

    char input = '\0';
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
    
    while (true) {
        // ---- Receive input ----
        char input = '\0';
        if (_kbhit()) input = _getch();

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

        card1.reconsider(billboard[0 + panel_i]);
        card2.reconsider(billboard[1 + panel_i]);
        card3.reconsider(billboard[2 + panel_i]);

        descriptionCard.reconsider(billboard[1 + panel_i]);
        descriptionCard.centerHorizontal();

        // ---- Update ----
        style::setFg({ 128, 186, 209 });
        gnu::printRawCenter(rawLogo);
        style::setFg({ 222, 197, 153 });

        card2.centerHorizontal();
        card1.setPosition( card2.position + gnu::vec2d{ (short)(-card1.size.x - 10), 1 } );
        card3.setPosition( card2.position + gnu::vec2d{ (short)(card2.size.x + 10), 1 } );

        card1.draw();
        card2.draw();
        card3.draw();

        gnu::print("\n\n\n");
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
        gnu::printLineCentered("😎 🤑 🙄");
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


void carteleraFechaScreen() {
    system("cls");
    gnu::displayLogo2D(style::color::RED);

    json billboardDays = gnu::apifetch("/cines/" + cineID + "/cartelera");

    std::vector<json> billboardsByDate;
    json currentBillboard;
    std::vector<std::string> availableDates;

    for (auto billboard : billboardDays["days"].get<std::vector<json>>()) {
        availableDates.push_back(billboard["date"].get<std::string>());
    }

    bool lock = true;
    size_t currentDay = 0;
    short optSelection = 0;

    json currentMovie;
    std::string currentMovieName;
    size_t currentMovie_i = 0;

    while (lock) {
        // guarda la película actual
        billboardsByDate = billboardDays["days"].get<std::vector<json>>();
        currentBillboard = billboardsByDate[currentDay].get<json>();
        currentMovie = currentBillboard["movies"][currentMovie_i].get<json>();
        currentMovieName = currentMovie["title"].get<std::string>();

        gnu::gotoXY(0, 15);
        gnu::displayDate(availableDates[currentDay], optSelection);

        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (currentMovieName.size() / 2) - 2, 15
        );

        if (optSelection == 0) {
            style::setColor(style::color::WHITE);
        }
        if (optSelection == 1) {
            style::setColor(style::color::LIGHTGREEN);
        }
        std::cout << "> " << currentMovieName << " <";

        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) + (currentMovieName.size() / 2), 16
        );
        std::cout << currentMovie_i + 1 << "/" << currentBillboard["movies"].size();

        style::setColor(style::color::WHITE);

        // Check input
        if (_kbhit()) {
            char hit = _getch();
            
            gnu::cleanLine(15);
            gnu::cleanLine(16);

            switch (hit) {
            case gnu::key::Up: {
                if (optSelection > 0){
                    optSelection--;
                }

                break;
            }
            case gnu::key::Down: {
                if (optSelection < 1) {
                    optSelection++;
                }
                break;
            }
            case gnu::key::Left: {
                if (optSelection == 0) {
                    if (currentDay > 0) {
                        currentDay--;
                    }
                    currentMovie_i = 0;
                }
                if (optSelection == 1) {
                    if (currentMovie_i > 0) currentMovie_i--;
                }
                break;
            }
            case gnu::key::Right: {
                if (optSelection == 0) {
                    if (currentDay + 1 < availableDates.size()) {
                        currentDay++;
                    }
                    currentMovie_i = 0; // reiniciarlo a la opción 0
                }
                if (optSelection == 1) {
                    if (currentMovie_i + 1 < currentBillboard["movies"].size()) {
                        currentMovie_i++;
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        gnu::sleep(5);
    }
}

void displayDate(std::string fulldate, short opt) {
    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) - 5, 12
    );
    std::cout << fulldate;

    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) + 3, 13
    );

    if (opt == 0) {
        style::setColor(style::color::LIGHTGREEN);
        std::cout << "^^";
        style::setColor(style::color::WHITE);
    }
    else {
        gnu::cleanLine(13);
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
       _  _    _          
  ___ | |(_)  (_)  __ _
 / _ \| || |  | | / _` |
|  __/| || |  | || (_| |
 \___||_||_| _/ | \__,_|
            |__/     
         _   _  _ __ 
        | | | || '_ \
        | |_| || | | |
         \__,_||_| |_|
 _                      _ 
| |  ___    ___   __ _ | |
| | / _ \  / __| / _` || |
| || (_) || (__ | (_| || |
|_| \___/  \___| \__,_||_|
)";
    
    gnu::printRawCenter(elijaLocalLabel);

    // main menu loop
    while (lock) {
        gnu::gotoXY(0, 1);
        std::cout << std::endl;

        if (_kbhit()) {
            char hit = _getch();
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

            std::cout << ">>";
        }
        else {
            style::setColor(style::WHITE);
        }
        gnu::gotoXY(
            (gnu::getConsoleSize().x / 2) - (names[i].length() / 2),
            20 + i
        );

        std::cout << names[i];
    }
}


void logoDisplay3D(style::color color) {
    style::setColor(color);
    std::string cinext1 = R"(
 $$$$$$\  $$\ $$\   $$\ $$$$$$$$\ $$\   $$\ $$$$$$$$\
$$  __$$\ \__|$$$\  $$ |$$  _____|$$ |  $$ |\__$$  __|
$$ /  \__|$$\ $$$$\ $$ |$$ |      \$$\ $$  |   $$ |   
$$ |      $$ |$$ $$\$$ |$$$$$\     \$$$$  /    $$ |   
$$ |      $$ |$$ \$$$$ |$$  __|    $$  $$<     $$ |   
$$ |  $$\ $$ |$$ |\$$$ |$$ |      $$  /\$$\    $$ |   
\$$$$$$  |$$ |$$ | \$$ |$$$$$$$$\ $$ /  $$ |   $$ |   
 \______/ \__|\__|  \__|\________|\__|  \__|   \__|   
)";

    gnu::printRawCenter(cinext1);
}

void displayLogo2D(style::color color) {
    style::setColor(color);
    std::string cinext2 = R"(
   _______ _   _________  ________
  / ____(_/ | / / ____| |/ /_  __/
 / /   / /  |/ / __/  |   / / /   
/ /___/ / /|  / /___ /   | / /    
\____/_/_/ |_/_____//_/|_|/_/     
)";

    gnu::printRawCenter(cinext2);
}

void loadingScreen() {
    style::setColor(style::color::LIGHTCYAN);
    char cargando = '#';
    char inicio = '|';
    std::cout<< R"(
                                                 .___
      ____ _____ _______  _________    ____    __| _/____  
    _/ ___\\__  \\_  __ \/ ___\__  \  /    \  / __ |/  _ \
    \  \___ / __ \|  | \/ /_/  > __ \|   |  \/ /_/ (  <_> )
     \___  >____  /__|  \___  (____  /___|  /\____ |\____/ 
         \/     \/     /_____/     \/     \/      \/       
    )"<<"\n";
    
    gnu::gotoXY(
        (gnu::getConsoleSize().x / 2) - (33 / 2),
        15
    );
    
    std::cout << inicio;
    
    for (int i = 0 ; i < 32 ; i++){
        std::cout << cargando;
        gnu::sleep(20);
    }
    
    std::cout << inicio;
    
    gnu::sleep(400);

    system("cls");
}

} // namespace gnu
