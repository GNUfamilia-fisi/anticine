<img src="./media/Anticine.png" title="" alt="" data-align="center">

Anticine™ was born as a school project for the "Algorithmics I" assignment. It implements a fully console-based UI to provide the user a whole virtual cinema experience.



## Documentation

### Compilación

Para compilar el proyecto, ejecuta cualquiera de estos dos comandos dependiendo de tu compilador (g++ es el deseado).

```bash
# con g++
g++ -Iinclude src/*.cpp src/menus/*.cpp

# conf clang
clang -Iinclude src/*.cpp src/menus/*.cpp
```

### Utilidades

###### consoleSize

```cpp
consoleSize
```

La estructura "consoleSize" posee  2 miembros, "x" e "y". Esta estructura es usada más adelante en la función `getConsoleRectSize()` para devolver el tamaño de la terminal.

```cpp
typedef struct {
    int x;
    int y;
} consoleSize;
```

###### getConsoleRectSize

```cpp
consoleSize getConsoleRectSize();
```

Esta función obtiene las coordenadas de la consola y las devuelve como un objeto "consoleSize", es decir, tiene un componente x (ancho) e y (largo).

```cpp
consoleSize getConsoleRectSize() {
    CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufferInfo);

    consoleSize obj = {
        BufferInfo.srWindow.Right - BufferInfo.srWindow.Left + 1,
        BufferInfo.srWindow.Bottom - BufferInfo.srWindow.Top + 1,
    };

    return obj;
}
```

 La función se encarga de crear una estructura  `CONSOLE_SCREEN_BUFFER_INFO` ,  esta estructura contiene información acerca de la ventana de la consola en la que corre el programa.

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

###### gotoXY

```cpp
void gotoXY(short x, short y);
```

Todo caracter que se imprime en pantalla se imprime desde un punto dado, este punto de inicio es denominado generalmente como "cursor", la función `gotoXY()` dirige el cursor a las coordenadas especificadas en el primer y segundo parámetro.

```cpp
COORD cursorPosition;    //structure container for X and Y coordinates

void gotoXY(short x, short y) {
    cursorPosition = { x, y };
    SetConsoleCursorPosition(consoleWinHandle, cursorPosition);
}
```

A partir de donde el cursor se haya posicionado se imprimirán los caracteres.

###### exec

```cpp
std::string exec(const char *cmd);
```

Esta funcion ejecuta un subproceso (o comando) que se especifica en el `cmd`, que tiene la forma de un string clásico de **C** y retorna la respuesta de este proceso como un string de **C++**.

```cpp
std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
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
    std::string fetchResult = exec(command.c_str());

    json data;
    try {
        data = json::parse(fetchResult);
    }
    catch (std::exception e) {
        throw std::runtime_error("Error de fetching a " + url);
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

###### HANDLES

```cpp
HANDLE consoleWinHandle = GetStdHandle(STD_OUTPUT_HANDLE);
```

El tipo `HANDLE` es un tipo de puntero representativo de ciertos elementos, para este caso se hace uso de `GetStdHandle()` para obtener un "HANDLE" a un elemento de "STD OUTPUT" o de "salida estándar", en este caso, nuestra terminal. Este elemento es usado más adelante como una "llave" para controlar ciertas funcionalidades de la terminal;

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




