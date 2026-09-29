#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "Token.h"
#include "SymbolTable.h"

using namespace std;

class Lexer {
private:
    string contenido;
    int posicion;
    int linea;
    int columna;

    TablaSimbolos tablaSimbolos;
    vector<Token> tokens;
    vector<ErrorLexico> errores;

    bool esLetra(char c);
    bool esDigito(char c);
    bool esEspacio(char c);
    bool esPalabraReservada(string palabra);

    void avanzar();
    void analizarIdentificador();
    void analizarNumero();
    void analizarTexto();
    void analizarComentario();

    void agregarToken(
        string tipo,
        string lexema,
        int lineaToken,
        int columnaToken
    );

public:
    Lexer();

    bool cargarArchivo(string nombreArchivo);
    void analizar();

    vector<Token> obtenerTokens();
    vector<ErrorLexico> obtenerErrores();
    vector<string> obtenerTablaSimbolos();
};

#endif