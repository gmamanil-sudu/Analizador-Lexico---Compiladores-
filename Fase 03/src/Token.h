#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

struct Token {
    string tipo;
    string atributo;
    string lexema;
    int linea;
    int columna;
};

struct ErrorLexico {
    int linea;
    int columna;
    string lexema;
    string detalle;
};

string formatearToken(const Token& token);

#endif