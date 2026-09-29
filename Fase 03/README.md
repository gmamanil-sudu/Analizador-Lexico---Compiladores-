# Fase 03 - Analizador Léxico

## Descripción

En esta fase se implementó un analizador léxico para identificar y clasificar los diferentes elementos de un archivo de código fuente.

El analizador lee un archivo `.lp`, reconoce tokens, registra los identificadores en una tabla de símbolos y detecta errores léxicos.

## Estructura del proyecto

```text
Fase 03/
├── src/
│   ├── main.cpp
│   ├── Lexer.cpp
│   ├── Lexer.h
│   ├── Token.cpp
│   ├── Token.h
│   ├── SymbolTable.cpp
│   └── SymbolTable.h
│
├── test/
│   └── prueba_basica.lp
│
├── output/
│   ├── tokens.txt
│   ├── tabla_simbolos.txt
│   └── errores.txt
│
└── README.md
```

## Requisitos

Para compilar el proyecto se necesita un compilador de C++, por ejemplo:

* GCC
* G++

También se puede utilizar Git Bash en Windows.

## Compilación

Primero se debe ingresar a la carpeta `Fase 03`:

```bash
cd "Fase 03"
```

Luego se compilan todos los archivos `.cpp` de la carpeta `src`:

```bash
g++ src/*.cpp -o Analizador
```

Si la compilación es correcta, se generará el ejecutable `Analizador`.

## Ejecución

Para ejecutar el analizador:

```bash
./Analizador
```

El programa utiliza por defecto el archivo:

```text
test/prueba_basica.lp
```

Al finalizar el análisis se muestra un resumen similar a:

```text
Analisis lexico terminado.
Tokens: 36
Errores: 0
```

## Archivo de prueba

El archivo utilizado para realizar la prueba es:

```text
test/prueba_basica.lp
```

Contenido:

```text
int main() {
    int x;
    float precio;
    x = 10;
    precio = 15.5;

    if (x > 5) {
        println("Hola mundo");
    }

    return 0;
}
```

## Resultados

Después de ejecutar el programa se generan tres archivos dentro de `output/`.

### Tokens

El archivo:

```text
output/tokens.txt
```

contiene los tokens reconocidos por el analizador.

Ejemplo:

```text
<int> <main> <PARENTESIS_ABRE> <PARENTESIS_CIERRA> <LLAVE_ABRE> <int> <ID,0> <PUNTO_COMA>
```

### Tabla de símbolos

El archivo:

```text
output/tabla_simbolos.txt
```

contiene los identificadores encontrados y su posición.

Ejemplo:

```text
ID | Nombre
0 | x
1 | precio
```

### Errores

El archivo:

```text
output/errores.txt
```

contiene los errores léxicos encontrados durante el análisis.

Su estructura es:

```text
Linea | Columna | Lexema | Resultado | Detalle
```

Si no se encontraron errores, el archivo solamente contiene el encabezado.

## Tokens reconocidos

El analizador reconoce diferentes tipos de tokens.

### Palabras reservadas

```text
int
float
char
boolean
void
if
else
for
while
scanf
println
main
return
```

### Identificadores

Los identificadores encontrados se registran en la tabla de símbolos y se representan como:

```text
<ID,posicion>
```

Por ejemplo:

```text
<ID,0>
<ID,1>
```

### Números

Se reconocen números enteros y decimales:

```text
10
15.5
```

### Operadores

Se reconocen operadores como:

```text
+
-
*
/
=
==
!=
<
>
<=
>=
&&
||
!
```

### Símbolos

También se reconocen:

```text
(
)
{
}
;
,
```

### Cadenas de texto

Las cadenas escritas entre comillas son reconocidas como:

```text
<TEXTO>
```

## Errores léxicos

El analizador identifica diferentes tipos de errores.

### Identificador iniciado con número

Entrada:

```text
1int
```

Resultado:

```text
Un identificador no puede comenzar con un numero
```

### Número decimal incompleto

Entrada:

```text
12.
```

Resultado:

```text
Numero decimal incompleto
```

### Número decimal inválido

Entrada:

```text
1.2.3
```

Resultado:

```text
Numero decimal invalido
```

### Operadores incompletos

Entrada:

```text
&
```

o:

```text
|
```

Resultado:

```text
Operador '&' incompleto
```

o:

```text
Operador '|' incompleto
```

### Caracteres no reconocidos

Los caracteres que no pertenecen al conjunto definido por el analizador son registrados como errores léxicos.

## Prueba realizada

La prueba básica fue ejecutada correctamente con el archivo `prueba_basica.lp`.

Resultado obtenido:

```text
Analisis lexico terminado.
Tokens: 36
Errores: 0
```

Esto confirma que el archivo de prueba fue leído correctamente, los tokens fueron reconocidos y no se encontraron errores léxicos.
