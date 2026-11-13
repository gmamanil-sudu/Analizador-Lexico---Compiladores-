#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// ======================= LEXER =======================

struct Token {
    string tipo;      // ID, NUM_INT, NUM_DEC, TEXTO, COMP, ERROR, EOF; en palabras reservadas y simbolos el tipo es el propio lexema
    string lexema;
    int linea;
    int idSimbolo;    // ID de la tabla de simbolos (solo para identificadores)
};

// Tabla de simbolos para los identificadores
vector<string> nombresSimbolos;
vector<int> idsSimbolos;
int contadorId = 1;

int buscarOAgregarSimbolo(string nombre) {
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        if (nombresSimbolos[i] == nombre) return idsSimbolos[i];
    }
    nombresSimbolos.push_back(nombre);
    idsSimbolos.push_back(contadorId);
    contadorId++;
    return contadorId - 1;
}

// Palabras reservadas de LP segun la gramatica (se agrega "func")
vector<string> palabrasReservadas = {
    "char", "float", "int", "boolean", "void",   // tipos
    "main", "func", "return",                     // estructura
    "if", "else", "while",                        // control
    "scanf", "println"                            // entrada y salida
};

bool esPalabraReservada(string palabra) {
    for (int i = 0; i < (int)palabrasReservadas.size(); i++) {
        if (palabrasReservadas[i] == palabra) return true;
    }
    return false;
}

bool esLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool esDigito(char c) { return c >= '0' && c <= '9'; }

bool esLetraODigito(char c) { return esLetra(c) || esDigito(c); }

// Convierte el codigo fuente en una secuencia de tokens
vector<Token> analizar(const string& src) {
    vector<Token> out;
    int linea = 1;
    size_t i = 0;

    while (i < src.size()) {
        char c = src[i];

        // Espacios, saltos de linea y comentarios //
        if (c == '\n') { linea++; i++; continue; }
        if (c == ' ' || c == '\t' || c == '\r') { i++; continue; }
        if (c == '/' && i + 1 < src.size() && src[i + 1] == '/') {
            while (i < src.size() && src[i] != '\n') i++;
            continue;
        }

        // Identificadores y palabras reservadas
        if (esLetra(c)) {
            string p;
            while (i < src.size() && esLetraODigito(src[i])) p += src[i++];
            if (esPalabraReservada(p)) out.push_back({p, p, linea, 0});
            else out.push_back({"ID", p, linea, buscarOAgregarSimbolo(p)});
            continue;
        }

        // Numeros enteros y decimales
        if (esDigito(c)) {
            string n;
            while (i < src.size() && esDigito(src[i])) n += src[i++];
            if (i + 1 < src.size() && src[i] == '.' && esDigito(src[i + 1])) {
                n += src[i++];
                while (i < src.size() && esDigito(src[i])) n += src[i++];
                out.push_back({"NUM_DEC", n, linea, 0});
            } else {
                out.push_back({"NUM_INT", n, linea, 0});
            }
            continue;
        }

        // Cadenas de texto
        if (c == '"') {
            string t;
            i++;
            while (i < src.size() && src[i] != '"' && src[i] != '\n') t += src[i++];
            if (i < src.size() && src[i] == '"') { i++; out.push_back({"TEXTO", t, linea, 0}); }
            else out.push_back({"ERROR", "cadena sin cerrar", linea, 0});
            continue;
        }

        // Operadores de dos caracteres
        if (i + 1 < src.size()) {
            string dos = src.substr(i, 2);
            if (dos == ">=" || dos == "<=" || dos == "!=" || dos == "==") {
                out.push_back({"COMP", dos, linea, 0}); i += 2; continue;
            }
            if (dos == "&&" || dos == "||") {
                out.push_back({dos, dos, linea, 0}); i += 2; continue;
            }
        }

        // Operadores y simbolos de un caracter
        string s(1, c);
        if (c == '>' || c == '<') { out.push_back({"COMP", s, linea, 0}); i++; continue; }
        if (string("=!+-*/%(){}[],;").find(c) != string::npos) {
            out.push_back({s, s, linea, 0}); i++; continue;
        }

        out.push_back({"ERROR", "caracter no reconocido '" + s + "'", linea, 0});
        i++;
    }
    out.push_back({"EOF", "", linea, 0});
    return out;
}

// ======================= PARSER (descenso recursivo) =======================

struct ErrorSintactico { string mensaje; };

vector<Token> tokens;
size_t pos = 0;

const Token& actual() { return tokens[pos]; }
const Token& siguiente() { return tokens[min(pos + 1, tokens.size() - 1)]; }
bool es(const string& tipo) { return actual().tipo == tipo; }
void avanzar() { if (pos < tokens.size() - 1) pos++; }

string describir(const Token& t) {
    if (t.tipo == "EOF") return "fin de archivo";
    if (t.tipo == "ID" || t.tipo == "NUM_INT" || t.tipo == "NUM_DEC" || t.tipo == "TEXTO" || t.tipo == "COMP")
        return t.tipo + " '" + t.lexema + "'";
    return "'" + t.lexema + "'";
}

// Lanza un error sintactico. Si "alFinalDeAnterior" es true, se reporta la linea
// del token previo (util para ';' faltante, que se nota en la linea siguiente).
void error(const string& esperado, bool alFinalDeAnterior = false) {
    int linea = (alFinalDeAnterior && pos > 0) ? tokens[pos - 1].linea : actual().linea;
    throw ErrorSintactico{"Error sintactico (linea " + to_string(linea) + "): se esperaba " +
                          esperado + " pero se encontro " + describir(actual())};
}

// Consume el token si coincide; si no, error
void coincidir(const string& tipo, const string& nombre = "", bool alFinalDeAnterior = false) {
    if (es(tipo)) avanzar();
    else error(nombre.empty() ? "'" + tipo + "'" : nombre, alFinalDeAnterior);
}

bool esTipoBase() { return es("char") || es("float") || es("int") || es("boolean"); }

// Declaraciones adelantadas
void EXPRESION();
void EXPRESION_ADITIVA();
void BLOQUE();

// DIMENSION -> [ num_int ] DIMENSION | e
void DIMENSION() {
    while (es("[")) { avanzar(); coincidir("NUM_INT", "una constante entera"); coincidir("]"); }
}

// DIMENSION2 -> [ EXPRESION_ADITIVA ] DIMENSION2 | e
void DIMENSION2() {
    while (es("[")) { avanzar(); EXPRESION_ADITIVA(); coincidir("]"); }
}

// TIPO -> TIPO_BASE DIMENSION
void TIPO() {
    if (!esTipoBase()) error("un tipo (char, float, int o boolean)");
    avanzar();
    DIMENSION();
}

// TERMINO -> id DIMENSION2 | CONSTANTE
void TERMINO() {
    if (es("ID")) { avanzar(); DIMENSION2(); }
    else if (es("NUM_INT") || es("NUM_DEC")) avanzar();
    else error("un identificador o una constante");
}

// FACTOR -> SIGNO TERMINO | texto | ! FACTOR | ( EXPRESION )
void FACTOR() {
    if (es("+") || es("-")) { avanzar(); TERMINO(); }
    else if (es("TEXTO")) avanzar();
    else if (es("!")) { avanzar(); FACTOR(); }
    else if (es("(")) { avanzar(); EXPRESION(); coincidir(")"); }
    else TERMINO();
}

// EXPRESION_MULTIPLICATIVA -> FACTOR ( OPERADOR_MULTIPLICATIVO FACTOR )*
void EXPRESION_MULTIPLICATIVA() {
    FACTOR();
    while (es("*") || es("/") || es("%")) { avanzar(); FACTOR(); }
}

// EXPRESION_ADITIVA -> EXPRESION_MULTIPLICATIVA ( OPERADOR_ADITIVO EXPRESION_MULTIPLICATIVA )*
void EXPRESION_ADITIVA() {
    EXPRESION_MULTIPLICATIVA();
    while (es("+") || es("-")) { avanzar(); EXPRESION_MULTIPLICATIVA(); }
}

// EXPRESION_RELACIONAL -> EXPRESION_ADITIVA ( comp EXPRESION_ADITIVA )?
void EXPRESION_RELACIONAL() {
    EXPRESION_ADITIVA();
    if (es("COMP")) { avanzar(); EXPRESION_ADITIVA(); }
}

// EXPRESION_Y -> EXPRESION_RELACIONAL ( && EXPRESION_RELACIONAL )*
void EXPRESION_Y() {
    EXPRESION_RELACIONAL();
    while (es("&&")) { avanzar(); EXPRESION_RELACIONAL(); }
}

// EXPRESION_O -> EXPRESION_Y ( || EXPRESION_Y )*
void EXPRESION_O() {
    EXPRESION_Y();
    while (es("||")) { avanzar(); EXPRESION_Y(); }
}

// EXPRESION -> EXPRESION_O
void EXPRESION() { EXPRESION_O(); }

// FUNCION -> func id ( ARGUMENTOS )   con   ARGUMENTOS -> EXPRESION NUEVO_ARGUMENTO | e
void FUNCION() {
    coincidir("func");
    coincidir("ID", "un identificador");
    coincidir("(");
    if (!es(")")) {
        EXPRESION();
        while (es(",")) { avanzar(); EXPRESION(); }
    }
    coincidir(")");
}

// COMPLEMENTO -> EXPRESION | FUNCION
void COMPLEMENTO() {
    if (es("func")) FUNCION();
    else EXPRESION();
}

// ASIGNACION -> id = COMPLEMENTO ;
void ASIGNACION() {
    coincidir("ID", "un identificador");
    coincidir("=");
    COMPLEMENTO();
    coincidir(";", "';'", true);
}

// COMANDO (entrega 1: solo ASIGNACION; el resto se agrega en la entrega 2)
void COMANDO() {
    if (es("ID")) ASIGNACION();
    else error("un comando (por ahora solo se reconocen asignaciones)");
}

// COMANDOS -> COMANDO COMANDOS | e
void COMANDOS() {
    while (!es("}") && !es("EOF")) COMANDO();
}

// LISTA_VARIABLES -> TIPO id LISTA_ID ; LISTA_VARIABLES | e
void LISTA_VARIABLES() {
    while (esTipoBase()) {
        TIPO();
        coincidir("ID", "un identificador");
        while (es(",")) { avanzar(); coincidir("ID", "un identificador"); }   // LISTA_ID
        coincidir(";", "';'", true);
    }
}

// BLOQUE -> { LISTA_VARIABLES COMANDOS }
void BLOQUE() {
    coincidir("{");
    LISTA_VARIABLES();
    COMANDOS();
    coincidir("}", "'}' para cerrar el bloque");
}

// PARAMETROS -> TIPO id LISTA_PARAMETROS | e
void PARAMETROS() {
    if (!esTipoBase()) return;
    TIPO(); coincidir("ID", "un identificador");
    while (es(",")) { avanzar(); TIPO(); coincidir("ID", "un identificador"); }
}

// DECLARACION_FUNCION -> TIPO_RETORNO id ( PARAMETROS ) BLOQUE
void DECLARACION_FUNCION() {
    if (es("void")) avanzar(); else TIPO();
    coincidir("ID", "un identificador");
    coincidir("(");
    PARAMETROS();
    coincidir(")");
    BLOQUE();
}

// LISTA_FUNCIONES -> DECLARACION_FUNCION LISTA_FUNCIONES | e
// Es funcion si empieza con tipo de retorno y lo que sigue NO es main.
void LISTA_FUNCIONES() {
    while ((esTipoBase() || es("void")) && siguiente().tipo != "main") DECLARACION_FUNCION();
}

// PRINCIPAL -> main ( ) BLOQUE   (se acepta "void" antes de main, como en los ejemplos del enunciado)
void PRINCIPAL() {
    if (es("void")) avanzar();
    coincidir("main");
    coincidir("(");
    coincidir(")");
    BLOQUE();
}

// PROGRAMA -> LISTA_FUNCIONES PRINCIPAL
void PROGRAMA() {
    LISTA_FUNCIONES();
    PRINCIPAL();
    if (!es("EOF")) error("el fin de archivo despues del bloque de main");
}

// ======================= MAIN =======================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " archivo.lp [--tokens]" << endl;
        return 1;
    }
    ifstream f(argv[1]);
    if (!f) { cout << "No se pudo abrir " << argv[1] << endl; return 1; }
    stringstream buf; buf << f.rdbuf();

    tokens = analizar(buf.str());

    // Errores lexicos
    for (const Token& t : tokens) {
        if (t.tipo == "ERROR") {
            cout << "Error lexico (linea " << t.linea << "): " << t.lexema << endl;
            return 1;
        }
    }

    if (argc > 2 && string(argv[2]) == "--tokens") {
        for (const Token& t : tokens)
            cout << t.linea << "\t" << t.tipo << "\t" << t.lexema << endl;
    }

    try {
        PROGRAMA();
        cout << "Programa ACEPTADO: la estructura es sintacticamente correcta." << endl;
    } catch (ErrorSintactico& e) {
        cout << e.mensaje << endl;
        cout << "Programa RECHAZADO." << endl;
        return 1;
    }
    return 0;
}
