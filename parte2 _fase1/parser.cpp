#include <iostream>   
#include <fstream>    
#include <string>   
#include <vector>    

using namespace std; 

struct Token {
    string tipo;      // Tipo de token: ID, NUM_INT, TEXTO, etc.
    string lexema;   
    int linea;        
};

// Tabla de simbolos para los identificadores
vector<string> nombresSimbolos;  // Guarda los nombres: edad, nombre...
vector<int> idsSimbolos;          // Guarda sus respectivos IDs: 1, 2...
int contadorId = 1;               // ID que se asignara al siguiente identificador

// Busca un identificador o lo agrega si es nuevo
int buscarOAgregarSimbolo(string nombre) {

    // Recorre todos los identificadores guardados
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {

        // Comprueba si el identificador ya existe
        if (nombresSimbolos[i] == nombre) {

            // Si existe, devuelve el ID que ya tenia
            return idsSimbolos[i];
        }
    }

    // Si no existe, agrega el nuevo identificador
    nombresSimbolos.push_back(nombre);

    // Le asigna el ID actual
    idsSimbolos.push_back(contadorId);

    // Aumenta el contador para el siguiente ID
    contadorId++;

    // Devuelve el ID que se acaba de asignar
    return contadorId - 1;
}

// Lista de palabras reservadas del lenguaje
vector<string> palabrasReservadas = {
    "si", "sino", "mientras", "para", "hacer",       // Condiciones y ciclos
    "inicio", "fin", "funcion", "retorna",            // Estructura del programa
    "entero", "decimal", "texto", "booleano",        // Tipos de datos
    "verdadero", "falso", "imprimir", "leer"         // Valores y funciones
};
// Comprueba si una palabra es reservada
bool esPalabraReservada(string palabra) {

    // Recorre todas las palabras reservadas
    for (int i = 0; i < (int)palabrasReservadas.size(); i++) {

        // Compara la palabra con cada palabra de la lista
        if (palabrasReservadas[i] == palabra) {

            // Si coincide, devuelve verdadero
            return true;
        }
    }
    // Si no coincide con ninguna, no es reservada
    return false;
}

// Comprueba si el caracter puede ser una letra o "_"
bool esLetra(char c) {

    // Acepta letras minusculas, mayusculas y "_"
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

// Comprueba si el caracter puede ser letra, "_" o numero
bool esLetraODigito(char c) {

    // Usa esLetra y tambien permite numeros del 0 al 9
    return esLetra(c) ||
           (c >= '0' && c <= '9');
}

// =====================================================================
// A PARTIR DE AQUI: AGREGADO PARA EL ENTREGABLE 1 (lo de arriba no se toco)
// =====================================================================

// ---------------- Complemento del lexer ----------------

// Palabras reservadas de la gramatica LP (incluye "func", que pide el enunciado).
// Se deja aparte porque la lista original usa palabras en espanol que no son las de la gramatica.
vector<string> palabrasReservadasLP = {
    "char", "float", "int", "boolean", "void",   // Tipos
    "main", "func", "return",                     // Estructura
    "if", "else", "while",                        // Control
    "scanf", "println"                            // Entrada y salida
};

// Comprueba si una palabra es reservada de LP
bool esPalabraReservadaLP(string palabra) {
    for (int i = 0; i < (int)palabrasReservadasLP.size(); i++) {
        if (palabrasReservadasLP[i] == palabra) return true;
    }
    return false;
}

// Comprueba si el caracter es un digito
bool esDigito(char c) {
    return c >= '0' && c <= '9';
}

// Recorre el codigo fuente y genera la secuencia de tokens.
// En palabras reservadas y simbolos, el tipo es el propio lexema (ej: "int", "{").
vector<Token> analizar(const string& src) {
    vector<Token> out;
    int linea = 1;
    int i = 0;
    int n = (int)src.size();

    while (i < n) {
        char c = src[i];

        // Saltos de linea y espacios
        if (c == '\n') { linea++; i++; continue; }
        if (c == ' ' || c == '\t' || c == '\r') { i++; continue; }

        // Identificadores y palabras reservadas
        if (esLetra(c)) {
            string p = "";
            while (i < n && esLetraODigito(src[i])) { p += src[i]; i++; }
            if (esPalabraReservadaLP(p)) {
                out.push_back({p, p, linea});
            } else {
                buscarOAgregarSimbolo(p);          // registra en la tabla de simbolos
                out.push_back({"ID", p, linea});
            }
            continue;
        }

        // Numeros enteros y decimales
        if (esDigito(c)) {
            string num = "";
            while (i < n && esDigito(src[i])) { num += src[i]; i++; }
            if (i + 1 < n && src[i] == '.' && esDigito(src[i + 1])) {
                num += src[i]; i++;
                while (i < n && esDigito(src[i])) { num += src[i]; i++; }
                out.push_back({"NUM_DEC", num, linea});
            } else {
                out.push_back({"NUM_INT", num, linea});
            }
            continue;
        }

        // Cadenas de texto
        if (c == '"') {
            string t = "";
            i++;
            while (i < n && src[i] != '"' && src[i] != '\n') { t += src[i]; i++; }
            if (i < n && src[i] == '"') { i++; out.push_back({"TEXTO", t, linea}); }
            else out.push_back({"ERROR", "cadena sin cerrar", linea});
            continue;
        }

        // Operadores de dos caracteres
        if (i + 1 < n) {
            string dos = src.substr(i, 2);
            if (dos == ">=" || dos == "<=" || dos == "!=" || dos == "==") {
                out.push_back({"COMP", dos, linea}); i += 2; continue;
            }
            if (dos == "&&" || dos == "||") {
                out.push_back({dos, dos, linea}); i += 2; continue;
            }
        }

        // Operadores y simbolos de un caracter
        string s(1, c);
        if (c == '>' || c == '<') { out.push_back({"COMP", s, linea}); i++; continue; }
        if (string("=!+-*/%(){}[],;").find(c) != string::npos) {
            out.push_back({s, s, linea}); i++; continue;
        }

        out.push_back({"ERROR", "caracter no reconocido '" + s + "'", linea});
        i++;
    }
    out.push_back({"EOF", "", linea});
    return out;
}

// ---------------- Analizador sintactico (descenso recursivo) ----------------

struct ErrorSintactico { string mensaje; };

vector<Token> tokens;   // Secuencia producida por el lexer
int pos = 0;            // Posicion del token actual

// Token actual
const Token& actual() { return tokens[pos]; }

// Comprueba si el token actual es de ese tipo
bool es(const string& tipo) { return tokens[pos].tipo == tipo; }

// Pasa al siguiente token (sin salirse de la lista)
void avanzar() { if (pos < (int)tokens.size() - 1) pos++; }

// Texto para mostrar un token en los mensajes de error
string describir(const Token& t) {
    if (t.tipo == "EOF") return "fin de archivo";
    if (t.tipo == "ID" || t.tipo == "NUM_INT" || t.tipo == "NUM_DEC" || t.tipo == "TEXTO" || t.tipo == "COMP")
        return t.tipo + " '" + t.lexema + "'";
    return "'" + t.lexema + "'";
}

// Lanza un error sintactico. Si alFinalDeAnterior es true se reporta la linea del token previo
// (el ';' faltante se detecta en la linea siguiente, pero el error esta al final de la anterior).
void error(const string& esperado, bool alFinalDeAnterior = false) {
    int linea = (alFinalDeAnterior && pos > 0) ? tokens[pos - 1].linea : actual().linea;
    throw ErrorSintactico{"Error sintactico (linea " + to_string(linea) + "): se esperaba " +
                          esperado + " pero se encontro " + describir(actual())};
}

// Consume el token esperado; si no coincide, reporta error
void coincidir(const string& tipo, const string& nombre = "", bool alFinalDeAnterior = false) {
    if (es(tipo)) avanzar();
    else error(nombre.empty() ? "'" + tipo + "'" : nombre, alFinalDeAnterior);
}

// Comprueba si el token actual es un TIPO_BASE
bool esTipoBase() { return es("char") || es("float") || es("int") || es("boolean"); }

// Declaraciones adelantadas
void EXPRESION_ADITIVA();

// DIMENSION -> [ num_int ] DIMENSION | e
void DIMENSION() {
    while (es("[")) {
        avanzar();
        coincidir("NUM_INT", "una constante entera");
        coincidir("]");
    }
}

// DIMENSION2 -> [ EXPRESION_ADITIVA ] DIMENSION2 | e
void DIMENSION2() {
    while (es("[")) {
        avanzar();
        EXPRESION_ADITIVA();
        coincidir("]");
    }
}

// TIPO -> TIPO_BASE DIMENSION
void TIPO() {
    if (!esTipoBase()) error("un tipo (char, float, int o boolean)");
    avanzar();              // TIPO_BASE
    DIMENSION();
}

// TERMINO -> id DIMENSION2 | CONSTANTE
void TERMINO() {
    if (es("ID")) { avanzar(); DIMENSION2(); }
    else if (es("NUM_INT") || es("NUM_DEC")) avanzar();
    else error("un identificador o una constante");
}

// FACTOR -> SIGNO TERMINO | texto | ( EXPRESION )   (por ahora los parentesis encierran una EXPRESION_ADITIVA)
void FACTOR() {
    if (es("+") || es("-")) { avanzar(); TERMINO(); }
    else if (es("TEXTO")) avanzar();
    else if (es("(")) { avanzar(); EXPRESION_ADITIVA(); coincidir(")"); }
    else TERMINO();
}

// EXPRESION_MULTIPLICATIVA -> FACTOR EXPRESION_MULTIPLICATIVA2
// EXPRESION_MULTIPLICATIVA2 -> OPERADOR_MULTIPLICATIVO FACTOR EXPRESION_MULTIPLICATIVA2 | e
void EXPRESION_MULTIPLICATIVA() {
    FACTOR();
    while (es("*") || es("/") || es("%")) { avanzar(); FACTOR(); }
}

// EXPRESION_ADITIVA -> EXPRESION_MULTIPLICATIVA EXPRESION_ADITIVA2
// EXPRESION_ADITIVA2 -> OPERADOR_ADITIVO EXPRESION_MULTIPLICATIVA EXPRESION_ADITIVA2 | e
void EXPRESION_ADITIVA() {
    EXPRESION_MULTIPLICATIVA();
    while (es("+") || es("-")) { avanzar(); EXPRESION_MULTIPLICATIVA(); }
}

// COMPLEMENTO -> EXPRESION | FUNCION
// Entrega 1: solo expresiones sencillas (aritmeticas). EXPRESION completa y FUNCION van en la entrega 2.
void COMPLEMENTO() {
    EXPRESION_ADITIVA();
}

// ASIGNACION -> id = COMPLEMENTO ;
void ASIGNACION() {
    coincidir("ID", "un identificador");
    coincidir("=");
    COMPLEMENTO();
    coincidir(";", "';'", true);
}

// COMANDO -> ... | ASIGNACION | ...   (entrega 1: solo ASIGNACION)
void COMANDO() {
    if (es("ID")) ASIGNACION();
    else error("una asignacion (en esta entrega solo se reconocen asignaciones)");
}

// COMANDOS -> COMANDO COMANDOS | e
void COMANDOS() {
    while (!es("}") && !es("EOF")) COMANDO();
}

// LISTA_ID -> (int edad, altura, peso;)
void LISTA_ID() {
    while (es(",")) {
        avanzar();
        coincidir("ID", "un identificador");
    }
}

// LISTA_VARIABLES -> int edad;
void LISTA_VARIABLES() {
    while (esTipoBase()) {
        TIPO();
        coincidir("ID", "un identificador");
        LISTA_ID();
        coincidir(";", "';'", true);
    }
}

// BLOQUE -> Reconoce un bloque de código delimitado por llaves.
void BLOQUE() {
    coincidir("{");
    LISTA_VARIABLES();
    COMANDOS();
    coincidir("}", "'}' para cerrar el bloque");
}

// PRINCIPAL -> main ( ) BLOQUE
// Se acepta "void" antes de main porque asi aparece en los ejemplos del enunciado.
void PRINCIPAL() {
    if (es("void")) avanzar();
    coincidir("main");
    coincidir("(");
    coincidir(")");
    BLOQUE();
}

// PROGRAMA -> LISTA_FUNCIONES PRINCIPAL
//Comprueba que después de terminar el programa no existan tokens adicionales.
void PROGRAMA() {
    LISTA_FUNCIONES();
    PRINCIPAL();
    if (!es("EOF")) error("el fin de archivo despues del bloque de main");
}

// ---------------- Archivos de salida ----------------

// Guarda la secuencia de tokens en tokens.txt
void guardarTokens() {
    ofstream out("tokens.txt");
    out << "LINEA\tTIPO\tLEXEMA" << endl;
    for (int i = 0; i < (int)tokens.size(); i++) {
        out << tokens[i].linea << "\t" << tokens[i].tipo << "\t" << tokens[i].lexema << endl;
    }
}

// Guarda la tabla de simbolos en tabla_simbolos.txt
void guardarTablaSimbolos() {
    ofstream out("tabla_simbolos.txt");
    out << "ID\tNOMBRE" << endl;
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        out << idsSimbolos[i] << "\t" << nombresSimbolos[i] << endl;
    }
}

// Guarda los errores (lexicos y sintacticos) en errores.txt
void guardarErrores(const vector<string>& errores) {
    ofstream out("errores.txt");
    if (errores.empty()) {
        out << "Sin errores." << endl;
    } else {
        for (int i = 0; i < (int)errores.size(); i++) out << errores[i] << endl;
    }
}

// ---------------- Programa principal ----------------

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " archivo.lp" << endl;
        return 1;
    }

    // Lee el archivo .lp completo
    ifstream f(argv[1]);
    if (!f) {
        cout << "No se pudo abrir " << argv[1] << endl;
        return 1;
    }
    string codigo = "", lineaTexto;
    while (getline(f, lineaTexto)) codigo += lineaTexto + "\n";

    vector<string> errores;

    // 1) Analisis lexico
    tokens = analizar(codigo);

    cout << "=== TOKENS ===" << endl;
    for (int i = 0; i < (int)tokens.size(); i++) {
        cout << "linea " << tokens[i].linea << "\t" << tokens[i].tipo
             << "\t" << tokens[i].lexema << endl;
    }

    cout << "=== TABLA DE SIMBOLOS ===" << endl;
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        cout << idsSimbolos[i] << "\t" << nombresSimbolos[i] << endl;
    }

    guardarTokens();
    guardarTablaSimbolos();

    // Errores lexicos (se reportan todos)
    for (int i = 0; i < (int)tokens.size(); i++) {
        if (tokens[i].tipo == "ERROR") {
            errores.push_back("Error lexico (linea " + to_string(tokens[i].linea) + "): " + tokens[i].lexema);
        }
    }
    if (!errores.empty()) {
        for (int i = 0; i < (int)errores.size(); i++) cout << errores[i] << endl;
        guardarErrores(errores);
        cout << "Archivos generados: tokens.txt, tabla_simbolos.txt, errores.txt" << endl;
        return 1;
    }

    // 2) Analisis sintactico sobre la secuencia de tokens
    cout << "=== ANALISIS SINTACTICO ===" << endl;
    int codigoSalida = 0;
    try {
        PROGRAMA();
        cout << "Programa ACEPTADO: la estructura es sintacticamente correcta." << endl;
    } catch (ErrorSintactico& e) {
        errores.push_back(e.mensaje);
        cout << e.mensaje << endl;
        cout << "Programa RECHAZADO." << endl;
        codigoSalida = 1;
    }

    guardarErrores(errores);
    cout << "Archivos generados: tokens.txt, tabla_simbolos.txt, errores.txt" << endl;
    return codigoSalida;
}
