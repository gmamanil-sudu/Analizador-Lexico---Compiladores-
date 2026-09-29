#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

struct Token {
    string tipo;
    int atributo;
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

string formatearToken(const Token& t);

#endif
