#Analizador-Lexico  (Compiladores)
Analizador léxico de LP

Proyecto grupal del curso de **Compiladores — Fase 3**, desarrollado en **C++11**.

Reconoce los tokens del lenguaje LP, almacena identificadores en una tabla de símbolos sin duplicados y reporta errores léxicos con línea y columna.

## Integrantes

- Grecia Mamani 
- Angelica Huaman 
- Heydy Castillo

## Compilación y ejecución

Se necesita un compilador compatible con C++11, como `g++`. Estos comandos son para Windows (PowerShell). Si el archivo fuente tiene otro nombre, reemplazar `main.cpp`.

```powershell
g++ -std=c++11 main.cpp -o lexlp.exe
.\lexlp.exe
```

Por defecto, analiza el archivo `prueba.lp` ubicado en la carpeta de ejecución. Para usar otro archivo:

```powershell
.\lexlp.exe otro_archivo.lp
```

## Resultados

El programa muestra los resultados en consola y genera:

- **tokens.txt:** tokens reconocidos.
- **tabla_simbolos.txt:** identificadores y sus índices.
- **errores.txt:** errores encontrados.

Estos archivos se sobrescriben en cada ejecución.

## Pruebas

Probar entradas válidas, inválidas y casos límite, como un archivo vacío, identificadores repetidos, `12.`, `1.2.3` y textos sin cerrar. Incluir los archivos de prueba en el repositorio antes de entregar.
