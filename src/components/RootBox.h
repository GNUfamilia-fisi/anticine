#pragma once
#include "Box.h"

namespace gnu {

class RootBox : public Box {

  public:
    // El tamaño del body es toda la consola, por defecto
    RootBox() : Box(gnu::getConsoleSize()) {
    }
    void drawTree() {
        std::cout << "I ended, chill bro";
    }
};

} // namespace gnu
