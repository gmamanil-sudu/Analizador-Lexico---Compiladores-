#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// un token guarda su tipo, el texto que se reconocio y la linea
struct Token {
    string tipo;
    string lexema;
    int linea;
};

// tabla de simbolos: solo para identificadores
// usamos dos listas paralelas en vez de una estructura mas compleja
vector<string> nombresSimbolos;
vector<int> idsSimbolos;
int contadorId = 1;

// busca si el identificador ya existe en la tabla, si no existe lo agrega
int buscarOAgregarSimbolo(string nombre) {
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        if (nombresSimbolos[i] == nombre) {
            return idsSimbolos[i];
        }
    }
    nombresSimbolos.push_back(nombre);
    idsSimbolos.push_back(contadorId);
    contadorId++;
    return contadorId - 1;
}

// lista de palabras reservadas del lenguaje
// ajustar segun el documento del proyecto
vector<string> palabrasReservadas = {
    "si", "sino", "mientras", "para", "hacer",
    "inicio", "fin", "funcion", "retorna",
    "entero", "decimal", "texto", "booleano",
    "verdadero", "falso", "imprimir", "leer"
};

// revisa si una palabra esta en la lista de reservadas
bool esPalabraReservada(string palabra) {
    for (int i = 0; i < (int)palabrasReservadas.size(); i++) {
        if (palabrasReservadas[i] == palabra) {
            return true;
        }
    }
    return false;
}

// para saber si un caracter puede empezar o seguir un identificador
bool esLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool esLetraODigito(char c) {
    return esLetra(c) || (c >= '0' && c <= '9');
}

int main() {

    ifstream archivo("prueba.lp");

    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return 1;
    }

    vector<Token> tokens;
    vector<string> errores;

    char c;
    int linea = 1;

    while (archivo.get(c)) {

        // salto de linea, para llevar la cuenta
        if (c == '\n') {
            linea++;
            continue;
        }

        // ignorar espacios y tabulaciones
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }

        // caso numero (NUM_INT o NUM_DEC)
        if (c >= '0' && c <= '9') {

            string numero = "";
            numero += c;
            int puntos = 0;

            while (archivo.peek() != EOF) {
                char siguiente = archivo.peek();

                if (siguiente >= '0' && siguiente <= '9') {
                    archivo.get(c);
                    numero += c;
                }
                else if (siguiente == '.') {
                    puntos++;
                    archivo.get(c);
                    numero += c;
                }
                else {
                    break;
                }
            }

            if (puntos > 1) {
                errores.push_back("linea " + to_string(linea) + ": numero mal formado -> " + numero);
            }
            else if (numero.find('.') != string::npos) {
                tokens.push_back({"NUM_DEC", numero, linea});
            }
            else {
                tokens.push_back({"NUM_INT", numero, linea});
            }
            continue;
        }

        // caso identificador o palabra reservada
        if (esLetra(c)) {

            string palabra = "";
            palabra += c;

            while (archivo.peek() != EOF && esLetraODigito(archivo.peek())) {
                archivo.get(c);
                palabra += c;
            }

            if (esPalabraReservada(palabra)) {
                tokens.push_back({"PALABRA_RESERVADA", palabra, linea});
            } else {
                int id = buscarOAgregarSimbolo(palabra);
                tokens.push_back({"ID(" + to_string(id) + ")", palabra, linea});
            }
            continue;
        }

        // caso texto entre comillas
        if (c == '"') {

            string cadena = "";
            bool cerrada = false;
            int lineaInicio = linea;

            while (archivo.get(c)) {
                if (c == '"') {
                    cerrada = true;
                    break;
                }
                if (c == '\n') {
                    linea++;
                    break;
                }
                cadena += c;
            }

            if (cerrada) {
                tokens.push_back({"TEXTO", cadena, lineaInicio});
            } else {
                errores.push_back("linea " + to_string(lineaInicio) + ": texto sin comilla de cierre");
            }
            continue;
        }

        // cualquier otro caracter aun no se reconoce
        // (los operadores se agregan en la fase 3)
        errores.push_back("linea " + to_string(linea) + ": caracter no reconocido -> " + string(1, c));
    }

    archivo.close();

    // mostrar la lista de tokens
    cout << "lista de tokens:" << endl;
    for (int i = 0; i < (int)tokens.size(); i++) {
        cout << "linea " << tokens[i].linea << "  " << tokens[i].lexema<< "  ->  " <<  tokens[i].tipo<< endl;
    }

    // mostrar la tabla de simbolos
    cout << endl << "tabla de simbolos:" << endl;
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        cout << "id " << idsSimbolos[i] << "  " << nombresSimbolos[i] << endl;
    }

    // mostrar los errores encontrados
    cout << endl << "errores lexicos:" << endl;
    if (errores.empty()) {
        cout << "ninguno" << endl;
    } else {
        for (int i = 0; i < (int)errores.size(); i++) {
            cout << errores[i] << endl;
        }
    }

    return 0;
}