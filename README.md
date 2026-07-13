# chesse

> Motor de ajedrez en C++17. [Un motor de ajedrez](https://www.chessprogramming.org/) es un programa que sabe jugar ajedrez, analiza variantes e implementa una serie de movimientos que pueda considerar más fuertes.

Bernd Besser, computerzeitalter:

![Bernd Besser, Computerzeitalter](https://www.schaakkunst.nl/images/images_bernd/computerzeitalter100x120cm.jpg)

## Compilación

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

Genera el binario `chess`. Por defecto corre el protocolo UCI; con `-i` modo texto contra el motor.

