# chesse

> [https://www.chessprogramming.org/](Motor de ajedrez en C++17). Un motor de ajedrez es un programa que sabe jugar ajedrez, analiza variantes e implementa una serie de movimientos que pueda considerar <más fuertes>. 

Bernd Besser, computerzeitalter:
[https://www.schaakkunst.nl/images/images_bernd/computerzeitalter100x120cm.jpg](Bernd Besser, computerzeitalter)

## Compilación

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

Genera el binario `chess`. Por defecto corre el protocolo UCI (para GUIs); con `--interactive` o `-i` corre en modo texto contra el motor.

