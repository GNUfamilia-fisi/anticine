<img title="" src="./media/Anticine.png" alt="" data-align="center" width="618">

Anticine™ was born as a school project for the "Algorithmics I" assignment. It implements a fully console-based UI virtual cinema experience.

# Documentation

## Compilación

Para compilar el proyecto, ejecuta cualquiera de estos dos comandos dependiendo de tu compilador (g++ es el deseado).

```bash
# con g++
g++ -Iinclude main.cpp

# con clang
clang -Iinclude main.cpp
```

## Utilidades

##### Namespace

Las funciones y estrcuturas que se especifican aquí y  en las utilidades de los archivos, excluyendo las estructuras y tipos de la biblioteca `windows.h` se usan mediante el namespace único del proyecto "`gnu::`".

```cpp
//ejemplo

gnu::vec2d Hola = gnu::getConsoleSize();
```

##### Tipos y estructuras



###### vec2d

```cpp
gnu::vec2d
```

La estructura `vec2d` posee  2 miembros, "x" e "y". Esta estructura es usada más adelante en la función `getConsoleSize()` para devolver el ancho y largo en caracteres de la terminal.

```cpp
typedef struct {
    int x;
    int y;
} vec2d;
```

Esta estructura es útil para almecenar datos que requieren 2 valores como una coordenada en el plano o las medidas de una figura rectangular.

###### CONSOLE_SCREEN_BUFFER_INFO

```cpp
typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
  COORD      dwSize;                //size of the screen buffer
  COORD      dwCursorPosition;      //coordinates of the cursor
  WORD       wAttributes;
  SMALL_RECT srWindow;              //screen buffer extremes coordinates
  COORD      dwMaximumWindowSize;   //maximum size of the console window
} CONSOLE_SCREEN_BUFFER_INFO;

//retrieved from the learn.windows.com documentation
```

Esta estructura contiene información acerca de la ventana de la consola en la que corre el programa, puede servir para obtener la posicion del cursor con el miembro `dwCursorPosition` o el tamaño de la ventana con `dwSize` , por ejemplo.

###### HANDLE

```cpp
HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);
```

El tipo `HANDLE` es un tipo de puntero representativo de ciertos elementos, para este caso se hace uso de `GetStdHandle()` para obtener un "HANDLE" a un elemento de "STD OUTPUT" o de "salida estándar", en este caso, nuestra terminal. Este elemento es usado más adelante como una "llave" para controlar ciertas funcionalidades de la terminal.

En el caso de la variable global `consoleWinHandle`, esta no deberia ser usada realmente fuera de la cabecera donde fue declarada.

###### COORD

El tipo `COORD` de la biblioteca `windows.h` es una estrutura que, de forma similar vec2d, nos permite almacenar 2 valores (un "x" y un "y").

```cpp
typedef struct _COORD {
  SHORT X;
  SHORT Y;
} COORD, *PCOORD;
```

###### Colores

Estos valores representan los códigos de color que toman las funciones de `windows.h`, pueden ser usadas en funciones como `setColor()`, por ejemplo.

```cpp
enum color : unsigned short {
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
};
```

Para usarlas, solo se llama al namespace y luego a `color` y luego el nombre, es decir:

```cpp
setColor(gnu::color::YELLOW);
```

###### Keycodes

Las keycodes son identificadores que pueden ser comparados con los valores que devuelve `_getch()`, por ejemplo. Estos son útiles para ejecutar instrucciones cuando se presione una tecla.

```cpp
enum key {
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77,

    a = 97,
    w = 119,
    s = 115,
    d = 100,

    ExitKey = 27,
    Space = 32,
    Enter = 13
};
```



##### Funciones

###### getConsoleSize

```cpp
gnu::vec2d getConsoleSize();
```

Esta función obtiene las coordenadas de la consola y las devuelve como un objeto "consoleSize", es decir, tiene un componente x (ancho) e y (largo).

```cpp
gnu::vec2d getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    gnu::vec2d coords = { BufferInfo.dwSize.X, BufferInfo.dwSize.Y };
    return coords;
}
```

La función se encarga de crear una estructura  `CONSOLE_SCREEN_BUFFER_INFO` .

###### gotoXY

```cpp
void gotoXY(short x, short y);
```

Todo caracter que se imprime en pantalla se imprime desde un punto dado, este punto de inicio es denominado generalmente como "cursor", la función `gotoXY()` dirige el cursor a las coordenadas especificadas en el primer y segundo parámetro.

```cpp
void gotoXY(short x, short y) {
    COORD cursorPosition = { x, y };
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}
```

A partir de donde el cursor se haya posicionado se imprimirán los caracteres.

###### exec

```cpp
std::string exec(const char *cmd);
```

Esta funcion ejecuta un subproceso (o comando) que se especifica en el `cmd` y devuelve la respuesta de este proceso como un string.

```cpp
std::string exec(std::string command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}
```

Esta función es la que abre el proceso que se especifique en el cmd con `popen()`, luego va pasando el resultado de este proceso o la "respuesta" en bloques de 128 bytes cada uno al string `result`.

En otras palabras, permite ejecutar un comando que ejecutarias en tu consola y la respuesta que le da ese comando la devuelve como string.

###### fetch

```cpp
json fetch(std::string url);
```

De la misma forma que `exec` "ejecuta" un subproceso y devuelve la respuesta, fetch ejecuta una peticion web, esta básicamente funciona de la misma forma que un navegador, por ejemplo, abriendo una conexión a direccion web y devolviendo la respuesta, en este caso como json.

```cpp
json fetch(std::string url) {
    std::string command = "curl -s " + url;
    std::string fetchResult = gnu::exec(command);
    json data;

    try {
        data = json::parse(fetchResult);
    }
    catch (...) {
        throw std::runtime_error("Error al intentar hacer fetch a " + url);
    }
    return data;
}
```

El `url` que le pasas a la función como parámetro se mezcla con el otro string "curl -s", `curl` es un comando que existe nativamente en muchas computadoras actualmente, este permite justamente hacer peticiones web a traves de la terminal. Cuando le especificamos a "curl" el parámetro "-s" le estamos diciendo que solo devuelva la respuesta, no barras de progreso o errores, de este modo "limpiamos" la respuesta para guardarla en `data`.

###### cleanLine

```cpp
void cleanLine();
```

Esta funcion trabaja de forma simple, sólo toma la posición en la que te encuentres actualmentey sobreescribe una línea completa de espacios vacíos " " con el tamaño de la pantalla.

###### ShowConsoleCursor

```cpp
void ShowConsoleCursor(bool showFlag);
```

Como su nombre lo indica, esta funcion se encarga de cambiar la visibilidad del cursor del terminal en donde se llame, como se explicó anteriormente, un cursor es solo un indicador desde donde la consola imprime.

```cpp
void ShowConsoleCursor(bool showFlag) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleWinHandle, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(consoleWinHandle, &cursorInfo);
}
```

El booleano `showFlag` determina si el cursor será visible o no (**true** para mostrar y **false** para ocultar).

##### setColor

```cpp
void setColor(gnu::color color)
```

Esta función basicamente solo cambia el color de lo que se vaya a imprimir en consola, solo toma como parámetro un `gnu::color`.

```cpp
void setColor(gnu::color color) {
    SetConsoleTextAttribute(consoleHandle, color);
}
```

##### sleep

```cpp
void sleep(int ms);
```

Esta función hace básicamente lo mismo que 

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(ms);
```

Es decir, toma una cantidad en milisegundos y pausa toda la ejecución del bucle o programa donde haya sido llamada por ese tiempo.
