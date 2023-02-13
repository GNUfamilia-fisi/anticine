#include "Box.h"

namespace gnu {

class Card : public Box{
public:
    json insert;
    char type;

    Card(json data, gnu::vec2d size, char key) : Box({5, 5}){
        this->insert = data;
        
        if (key == 0){
            this->content = insert["title"].get<std::string>();
        }
        else if (key == 1){
            this->content = insert["synopsis"].get<std::string>();
        }
        else {
            this->content = "Unspecified";
        }
        this->size = size;
        this->type = key;
    }

    void reconsider(json pass){
        switch(type){
        case 0:
            this->content = pass["title"].get<std::string>();
            break;
        case 1:
            this->content = pass["synopsis"].get<std::string>();
            break;
        }
    }
};

/*
    std::string content;
    bool transparent = true;
    bool showBorder = true;

    style::rgb box_color = { 255, 255, 255 };
    style::rgb text_color = { 0, 0, 0 };
    gnu::vec2d size = { 5, 5 };
    gnu::vec2d position = { 0, 0 };
*/

}