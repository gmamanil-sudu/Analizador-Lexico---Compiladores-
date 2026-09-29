#include "Lexer.h"

#include <fstream>

Lexer::Lexer() {
    posicion = 0;
    linea = 1;
    columna = 1;
}

bool Lexer::cargarArchivo(string nombreArchivo) {

    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        return false;
    }

    contenido = "";

    char c;

    while (archivo.get(c)) {
        contenido += c;
    }

    archivo.close();

    posicion = 0;
    linea = 1;
    columna = 1;

    tokens.clear();
    errores.clear();

    return true;
}

bool Lexer::esLetra(char c) {

    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::esDigito(char c) {

    return c >= '0' && c <= '9';
}

bool Lexer::esEspacio(char c) {

    return c == ' ' ||
           c == '\t' ||
           c == '\r';
}

bool Lexer::esPalabraReservada(string palabra) {

    return palabra == "int" ||
           palabra == "float" ||
           palabra == "char" ||
           palabra == "boolean" ||
           palabra == "void" ||
           palabra == "if" ||
           palabra == "else" ||
           palabra == "for" ||
           palabra == "while" ||
           palabra == "scanf" ||
           palabra == "println" ||
           palabra == "main" ||
           palabra == "return";
}

void Lexer::avanzar() {

    if (posicion >= contenido.length()) {
        return;
    }

    if (contenido[posicion] == '\n') {
        linea++;
        columna = 1;
    }
    else {
        columna++;
    }

    posicion++;
}

void Lexer::agregarToken(
    string tipo,
    string lexema,
    int lineaToken,
    int columnaToken
) {

    Token token;

    token.tipo = tipo;
    token.atributo = "";
    token.lexema = lexema;
    token.linea = lineaToken;
    token.columna = columnaToken;

    tokens.push_back(token);
}

void Lexer::analizarIdentificador() {

    int lineaInicio = linea;
    int columnaInicio = columna;

    string palabra = "";

    while (
        posicion < contenido.length() &&
        (esLetra(contenido[posicion]) ||
         esDigito(contenido[posicion]))
    ) {

        palabra += contenido[posicion];

        avanzar();
    }

    if (esPalabraReservada(palabra)) {

        agregarToken(
            palabra,
            palabra,
            lineaInicio,
            columnaInicio
        );
    }
    else {

        int posicionSimbolo =
            tablaSimbolos.buscarOAgregar(palabra);

        Token token;

        token.tipo = "ID";
        token.atributo = to_string(posicionSimbolo);
        token.lexema = palabra;
        token.linea = lineaInicio;
        token.columna = columnaInicio;

        tokens.push_back(token);
    }
}

void Lexer::analizarNumero() {

    int lineaInicio = linea;
    int columnaInicio = columna;

    string numero = "";

    while (
        posicion < contenido.length() &&
        esDigito(contenido[posicion])
    ) {

        numero += contenido[posicion];

        avanzar();
    }

    if (
        posicion < contenido.length() &&
        contenido[posicion] == '.'
    ) {

        numero += '.';

        avanzar();

        if (
            posicion >= contenido.length() ||
            !esDigito(contenido[posicion])
        ) {

            errores.push_back({
                lineaInicio,
                columnaInicio,
                numero,
                "Numero decimal incompleto"
            });

            return;
        }

        while (
            posicion < contenido.length() &&
            esDigito(contenido[posicion])
        ) {

            numero += contenido[posicion];

            avanzar();
        }

        if (
            posicion < contenido.length() &&
            contenido[posicion] == '.'
        ) {

            while (
                posicion < contenido.length() &&
                (
                    esDigito(contenido[posicion]) ||
                    contenido[posicion] == '.'
                )
            ) {

                numero += contenido[posicion];

                avanzar();
            }

            errores.push_back({
                lineaInicio,
                columnaInicio,
                numero,
                "Numero decimal invalido"
            });

            return;
        }
    }

    if (
        posicion < contenido.length() &&
        esLetra(contenido[posicion])
    ) {

        while (
            posicion < contenido.length() &&
            (
                esLetra(contenido[posicion]) ||
                esDigito(contenido[posicion])
            )
        ) {

            numero += contenido[posicion];

            avanzar();
        }

        errores.push_back({
            lineaInicio,
            columnaInicio,
            numero,
            "Un identificador no puede comenzar con un numero"
        });

        return;
    }

    agregarToken(
        "NUM",
        numero,
        lineaInicio,
        columnaInicio
    );
}

void Lexer::analizarTexto() {

    int lineaInicio = linea;
    int columnaInicio = columna;

    string texto = "";

    avanzar();

    while (
        posicion < contenido.length() &&
        contenido[posicion] != '"'
    ) {

        if (contenido[posicion] == '\n') {

            errores.push_back({
                lineaInicio,
                columnaInicio,
                texto,
                "Cadena de texto sin cerrar"
            });

            return;
        }

        texto += contenido[posicion];

        avanzar();
    }

    if (posicion >= contenido.length()) {

        errores.push_back({
            lineaInicio,
            columnaInicio,
            texto,
            "Cadena de texto sin cerrar"
        });

        return;
    }

    avanzar();

    agregarToken(
        "TEXTO",
        texto,
        lineaInicio,
        columnaInicio
    );
}

void Lexer::analizarComentario() {

    avanzar();
    avanzar();

    while (
        posicion < contenido.length() &&
        contenido[posicion] != '\n'
    ) {

        avanzar();
    }
}

void Lexer::analizar() {

    while (posicion < contenido.length()) {

        char c = contenido[posicion];

        if (esEspacio(c)) {

            avanzar();

            continue;
        }

        if (c == '\n') {

            avanzar();

            continue;
        }

        if (esLetra(c)) {

            analizarIdentificador();

            continue;
        }

        if (esDigito(c)) {

            analizarNumero();

            continue;
        }

        if (c == '"') {

            analizarTexto();

            continue;
        }

        if (
            c == '/' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '/'
        ) {

            analizarComentario();

            continue;
        }

        int lineaInicio = linea;
        int columnaInicio = columna;

        if (
            c == '=' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '='
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "IGUAL_IGUAL",
                "==",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        if (
            c == '!' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '='
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "DIFERENTE",
                "!=",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        if (
            c == '<' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '='
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "MENOR_IGUAL",
                "<=",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        if (
            c == '>' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '='
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "MAYOR_IGUAL",
                ">=",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        if (
            c == '&' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '&'
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "AND",
                "&&",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        if (
            c == '|' &&
            posicion + 1 < contenido.length() &&
            contenido[posicion + 1] == '|'
        ) {

            avanzar();
            avanzar();

            agregarToken(
                "OR",
                "||",
                lineaInicio,
                columnaInicio
            );

            continue;
        }

        switch (c) {

            case '+':
                agregarToken(
                    "SUMA",
                    "+",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '-':
                agregarToken(
                    "RESTA",
                    "-",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '*':
                agregarToken(
                    "MULTIPLICACION",
                    "*",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '/':
                agregarToken(
                    "DIVISION",
                    "/",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '=':
                agregarToken(
                    "ASIGNACION",
                    "=",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '<':
                agregarToken(
                    "MENOR",
                    "<",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '>':
                agregarToken(
                    "MAYOR",
                    ">",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '!':
                agregarToken(
                    "NOT",
                    "!",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '(':
                agregarToken(
                    "PARENTESIS_ABRE",
                    "(",
                    linea,
                    columna
                );
                avanzar();
                break;

            case ')':
                agregarToken(
                    "PARENTESIS_CIERRA",
                    ")",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '{':
                agregarToken(
                    "LLAVE_ABRE",
                    "{",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '}':
                agregarToken(
                    "LLAVE_CIERRA",
                    "}",
                    linea,
                    columna
                );
                avanzar();
                break;

            case ';':
                agregarToken(
                    "PUNTO_COMA",
                    ";",
                    linea,
                    columna
                );
                avanzar();
                break;

            case ',':
                agregarToken(
                    "COMA",
                    ",",
                    linea,
                    columna
                );
                avanzar();
                break;

            case '&':
                errores.push_back({
                    linea,
                    columna,
                    "&",
                    "Operador '&' incompleto"
                });

                avanzar();

                break;

            case '|':
                errores.push_back({
                    linea,
                    columna,
                    "|",
                    "Operador '|' incompleto"
                });

                avanzar();

                break;

            default:
                errores.push_back({
                    linea,
                    columna,
                    string(1, c),
                    "Caracter no reconocido"
                });

                avanzar();

                break;
        }
    }
}

vector<Token> Lexer::obtenerTokens() {
    return tokens;
}

vector<ErrorLexico> Lexer::obtenerErrores() {
    return errores;
}

vector<string> Lexer::obtenerTablaSimbolos() {
    return tablaSimbolos.obtenerNombres();
}