#include <iostream>
#include <fstream>
#include <vector>

#include "Lexer.h"
#include "Token.h"

using namespace std;

int main(int argc, char* argv[]) {

    string archivoEntrada = "test/prueba_basica.lp";

    if (argc > 1) {
        archivoEntrada = argv[1];
    }

    Lexer lexer;

    if (!lexer.cargarArchivo(archivoEntrada)) {

        cout << "No se pudo abrir el archivo: "
             << archivoEntrada << endl;

        return 1;
    }

    lexer.analizar();

    vector<Token> tokens = lexer.obtenerTokens();
    vector<ErrorLexico> errores = lexer.obtenerErrores();
    vector<string> simbolos = lexer.obtenerTablaSimbolos();

    ofstream archivoTokens("output/tokens.txt");

    for (int i = 0; i < tokens.size(); i++) {

        archivoTokens << formatearToken(tokens[i]);

        if (i + 1 < tokens.size()) {
            archivoTokens << " ";
        }
    }

    archivoTokens.close();

    ofstream archivoSimbolos(
        "output/tabla_simbolos.txt"
    );

    archivoSimbolos << "ID | Nombre" << endl;

    for (int i = 0; i < simbolos.size(); i++) {

        archivoSimbolos
            << i
            << " | "
            << simbolos[i]
            << endl;
    }

    archivoSimbolos.close();

    ofstream archivoErrores(
        "output/errores.txt"
    );

    archivoErrores
        << "Linea | Columna | Lexema | Resultado | Detalle"
        << endl;

    for (int i = 0; i < errores.size(); i++) {

        archivoErrores
            << errores[i].linea << " | "
            << errores[i].columna << " | "
            << errores[i].lexema << " | "
            << "ERROR | "
            << errores[i].detalle
            << endl;
    }

    archivoErrores.close();

    cout << "Analisis lexico terminado." << endl;

    cout << "Tokens: "
         << tokens.size()
         << endl;

    cout << "Errores: "
         << errores.size()
         << endl;

    return 0;
}