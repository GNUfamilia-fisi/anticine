#pragma once

#include <json.hpp>
#include <style.hpp>
#include "../consoleUtils.h"
#include "../components/Box.h"

#include "globales.h"

namespace gnu {

//para capturar el color o la posicion de los asientos
std::string codigos [5][15]={{"1A","1B","1C","1D","1E","1F","1G","1H","1I","1J","1K","1L","1M","1N","1P"},
                            {"2A","2B","2C","2D","2E","2F","2G","2H","2I","2J","2k","2L","2M","2N","2P"},
                            {"3A","3B","3C","3D","3E","3F","3G","3H","3I","3J","3k","3L","3M","3N","3P"},
                            {"4A","4B","4C","4D","4E","4F","4G","4H","4I","4J","4k","4L","4M","4N","4P"},
                            {"5A","5B","5C","5D","5E","5F","5G","5H","5I","5J","5k","5L","5M","5N","5P"}} ;   //wstring,
//forma de la butaca
std::string butaca[3] ={"┌──────┐","│      │","└─'  '─┘"};  

std::string menuAsientos(){
    gnu::cls();
    //pantalla
    gnu::Box pantalla({ 135, 2 });
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

} // namespace gnu