# Fase 1 — Reconocimiento de números

**Integrante:** Grecia Mamani

Primera versión del analizador léxico de LP, desarrollada en C++.

## Funcionamiento

Lee el archivo `prueba.lp` y reconoce secuencias numéricas:

- `NUM_INT`: números sin punto.
- `NUM_DEC`: secuencias que contienen un punto.

Esta versión todavía no valida números mal formados como `12.3.5`.

## Ejecución

Compilar y ejecutar desde la carpeta Fase 1 en Windows (PowerShell), con `g++` instalado:

```powershell
g++ main.cpp -o LexLP.exe
.\LexLP.exe
```

## Archivos

- `main.cpp`: código fuente.
- `prueba.lp`: entradas de prueba.
