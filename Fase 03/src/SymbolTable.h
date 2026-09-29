#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class TablaSimbolos {
private:
    vector<string> nombres;
    unordered_map<string, int> posiciones;

public:
    int buscarOAgregar(string nombre);
    vector<string> obtenerNombres();
};

#endif