#include "SymbolTable.h"

int TablaSimbolos::buscarOAgregar(string nombre) {

    if (posiciones.find(nombre) != posiciones.end()) {
        return posiciones[nombre];
    }

    int posicion = nombres.size();

    nombres.push_back(nombre);
    posiciones[nombre] = posicion;

    return posicion;
}

vector<string> TablaSimbolos::obtenerNombres() {
    return nombres;
}