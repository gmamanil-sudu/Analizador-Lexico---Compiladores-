#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============================================================
// ANALIZADOR LEXICO
// ============================================================
struct Token {
    string tipo;
    string lexema;
    int linea;
    int columna;
};

vector<string> nombresSimbolos;
vector<int> idsSimbolos;
int contadorId = 1;

int buscarOAgregarSimbolo(const string& nombre) {
    for (int i = 0; i < (int)nombresSimbolos.size(); i++) {
        if (nombresSimbolos[i] == nombre) return idsSimbolos[i];
    }
    nombresSimbolos.push_back(nombre);
    idsSimbolos.push_back(contadorId);
    return contadorId++;
}

vector<string> palabrasReservadasLP = {
    "char", "float", "int", "boolean", "void", "main", "func", "return",
    "if", "else", "while", "scanf", "println"
};

bool esPalabraReservadaLP(const string& palabra) {
    for (int i = 0; i < (int)palabrasReservadasLP.size(); i++)
        if (palabrasReservadasLP[i] == palabra) return true;
    return false;
}

bool esLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool esDigito(char c) { return c >= '0' && c <= '9'; }
bool esLetraODigito(char c) { return esLetra(c) || esDigito(c); }

vector<Token> analizarLexico(const string& src) {
    vector<Token> salida;
    int linea = 1, columna = 1, i = 0;
    int n = (int)src.size();

    while (i < n) {
        char c = src[i];
        if (c == '\n') { linea++; columna = 1; i++; continue; }
        if (c == ' ' || c == '\t' || c == '\r') { columna++; i++; continue; }

        int lineaInicio = linea, columnaInicio = columna;

        if (esLetra(c)) {
            string palabra;
            while (i < n && esLetraODigito(src[i])) {
                palabra += src[i++]; columna++;
            }
            if (esPalabraReservadaLP(palabra))
                salida.push_back({palabra, palabra, lineaInicio, columnaInicio});
            else {
                buscarOAgregarSimbolo(palabra);
                salida.push_back({"ID", palabra, lineaInicio, columnaInicio});
            }
            continue;
        }

        if (esDigito(c)) {
            string numero;
            while (i < n && esDigito(src[i])) { numero += src[i++]; columna++; }
            if (i + 1 < n && src[i] == '.' && esDigito(src[i + 1])) {
                numero += src[i++]; columna++;
                while (i < n && esDigito(src[i])) { numero += src[i++]; columna++; }
                salida.push_back({"NUM_DEC", numero, lineaInicio, columnaInicio});
            } else salida.push_back({"NUM_INT", numero, lineaInicio, columnaInicio});
            continue;
        }

        if (c == '"') {
            i++; columna++;
            string texto;
            while (i < n && src[i] != '"' && src[i] != '\n') {
                texto += src[i++]; columna++;
            }
            if (i < n && src[i] == '"') {
                i++; columna++;
                salida.push_back({"TEXTO", texto, lineaInicio, columnaInicio});
            } else salida.push_back({"ERROR", "cadena de texto sin cerrar", lineaInicio, columnaInicio});
            continue;
        }

        if (i + 1 < n) {
            string dos = src.substr(i, 2);
            if (dos == ">=" || dos == "<=" || dos == "!=" || dos == "==") {
                salida.push_back({"COMP", dos, lineaInicio, columnaInicio});
                i += 2; columna += 2; continue;
            }
            if (dos == "&&" || dos == "||") {
                salida.push_back({dos, dos, lineaInicio, columnaInicio});
                i += 2; columna += 2; continue;
            }
        }

        string simbolo(1, c);
        if (c == '>' || c == '<') salida.push_back({"COMP", simbolo, lineaInicio, columnaInicio});
        else if (string("=!+-*/%(){}[],;").find(c) != string::npos)
            salida.push_back({simbolo, simbolo, lineaInicio, columnaInicio});
        else salida.push_back({"ERROR", "caracter no reconocido '" + simbolo + "'", lineaInicio, columnaInicio});
        i++; columna++;
    }
    salida.push_back({"EOF", "", linea, columna});
    return salida;
}

// ============================================================
// ARBOL SINTACTICO ABSTRACTO (AST)
// ============================================================
struct Nodo {
    string tipo;
    string valor;
    vector< shared_ptr<Nodo> > hijos;
    Nodo(const string& t, const string& v = "") : tipo(t), valor(v) {}
};

typedef shared_ptr<Nodo> NodoPtr;

NodoPtr crearNodo(const string& tipo, const string& valor = "") {
    return make_shared<Nodo>(tipo, valor);
}

void imprimirAST(const NodoPtr& nodo, int nivel = 0) {
    if (!nodo) return;
    for (int i = 0; i < nivel; i++) cout << "  ";
    cout << nodo->tipo;
    if (!nodo->valor.empty()) cout << ": " << nodo->valor;
    cout << endl;
    for (int i = 0; i < (int)nodo->hijos.size(); i++)
        imprimirAST(nodo->hijos[i], nivel + 1);
}

void guardarAST(ofstream& out, const NodoPtr& nodo, int nivel = 0) {
    if (!nodo) return;
    for (int i = 0; i < nivel; i++) out << "  ";
    out << nodo->tipo;
    if (!nodo->valor.empty()) out << ": " << nodo->valor;
    out << "\n";
    for (int i = 0; i < (int)nodo->hijos.size(); i++)
        guardarAST(out, nodo->hijos[i], nivel + 1);
}

// ============================================================
// ANALIZADOR SINTACTICO POR DESCENSO RECURSIVO
// Implementa la gramatica del documento Gramática_LP_LL1.
// ============================================================
struct ErrorSintactico { string mensaje; };
vector<Token> tokens;
int pos = 0;
vector<string> erroresSintacticos;

const Token& actual() { return tokens[pos]; }

// Consulta de anticipacion: distingue void main de una funcion ordinaria.
// El parser sigue consumiendo exclusivamente los tokens del lexer.
const Token& siguiente() {
    return tokens[pos + 1 < (int)tokens.size() ? pos + 1 : pos];
}
bool es(const string& tipo) { return actual().tipo == tipo; }
void avanzar() { if (pos < (int)tokens.size() - 1) pos++; }

string describir(const Token& t) {
    if (t.tipo == "EOF") return "fin de archivo";
    if (t.tipo == "ID" || t.tipo == "NUM_INT" || t.tipo == "NUM_DEC" ||
        t.tipo == "TEXTO" || t.tipo == "COMP")
        return t.tipo + " ('" + t.lexema + "')";
    return "'" + t.lexema + "'";
}

void error(const string& esperado) {
    throw ErrorSintactico{"Error sintactico en linea " + to_string(actual().linea) +
        ", columna " + to_string(actual().columna) + ": se esperaba " + esperado +
        " pero se encontro " + describir(actual())};
}

void coincidir(const string& tipo, const string& nombre = "") {
    if (es(tipo)) avanzar();
    else error(nombre.empty() ? "'" + tipo + "'" : nombre);
}

bool esTipoBase() {
    return es("char") || es("float") || es("int") || es("boolean");
}

bool esInicioExpresion() {
    return es("ID") || es("NUM_INT") || es("NUM_DEC") || es("TEXTO") ||
           es("+") || es("-") || es("!") || es("(");
}

// Declaraciones adelantadas
NodoPtr PROGRAMA();
NodoPtr LISTA_FUNCIONES();
NodoPtr DECLARACION_FUNCION();
NodoPtr TIPO();
NodoPtr TIPO_RETORNO();
NodoPtr DIMENSION();
NodoPtr PARAMETROS();
NodoPtr LISTA_PARAMETROS();
NodoPtr PRINCIPAL();
NodoPtr BLOQUE();
NodoPtr LISTA_VARIABLES();
NodoPtr LISTA_ID();
NodoPtr COMANDOS();
NodoPtr COMANDO();
NodoPtr LECTURA();
NodoPtr TERMINO_LECTURA();
NodoPtr NUEVO_TERMINO_LECTURA();
NodoPtr DIMENSION2();
NodoPtr ESCRITURA();
NodoPtr TERMINO_ESCRITURA();
NodoPtr NUEVO_TERMINO_ESCRITURA();
NodoPtr SELECCION();
NodoPtr SINO();
NodoPtr MIENTRAS();
NodoPtr ASIGNACION();
NodoPtr COMPLEMENTO();
NodoPtr FUNCION();
NodoPtr ARGUMENTOS();
NodoPtr NUEVO_ARGUMENTO();
NodoPtr RETORNO();
NodoPtr EXPRESION();
NodoPtr EXPRESION_O();
NodoPtr EXPRESION_Y();
NodoPtr EXPRESION_RELACIONAL();
NodoPtr EXPRESION_ADITIVA();
NodoPtr EXPRESION_MULTIPLICATIVA();
NodoPtr FACTOR();
NodoPtr TERMINO();
NodoPtr CONSTANTE();

// PROGRAMA -> LISTA_FUNCIONES PRINCIPAL EOF
NodoPtr PROGRAMA() {
    NodoPtr n = crearNodo("Programa");
    n->hijos.push_back(LISTA_FUNCIONES());
    n->hijos.push_back(PRINCIPAL());
    coincidir("EOF", "el fin del archivo despues del programa principal");
    return n;
}

// LISTA_FUNCIONES -> DECLARACION_FUNCION LISTA_FUNCIONES | epsilon
NodoPtr LISTA_FUNCIONES() {
    NodoPtr n = crearNodo("ListaFunciones");
    while ((esTipoBase() || es("void")) && !(es("void") && siguiente().tipo == "main"))
        n->hijos.push_back(DECLARACION_FUNCION());
    return n;
}

// DECLARACION_FUNCION -> TIPO_RETORNO ID ( PARAMETROS ) BLOQUE
NodoPtr DECLARACION_FUNCION() {
    NodoPtr n = crearNodo("DeclaracionFuncion");
    n->hijos.push_back(TIPO_RETORNO());
    if (!es("ID")) error("el identificador de la funcion");
    n->valor = actual().lexema;
    avanzar();
    coincidir("(");
    n->hijos.push_back(PARAMETROS());
    coincidir(")");
    n->hijos.push_back(BLOQUE());
    return n;
}

// TIPO_RETORNO -> TIPO | void
NodoPtr TIPO_RETORNO() {
    if (es("void")) {
        NodoPtr n = crearNodo("TipoRetorno", "void");
        avanzar();
        return n;
    }
    NodoPtr n = crearNodo("TipoRetorno");
    n->hijos.push_back(TIPO());
    return n;
}

// TIPO -> TIPO_BASE DIMENSION
NodoPtr TIPO() {
    if (!esTipoBase()) error("un tipo base: char, float, int o boolean");
    NodoPtr n = crearNodo("Tipo", actual().lexema);
    avanzar();
    n->hijos.push_back(DIMENSION());
    return n;
}

// DIMENSION -> [ NUM_INT ] DIMENSION | epsilon
NodoPtr DIMENSION() {
    NodoPtr n = crearNodo("Dimension");
    while (es("[")) {
        avanzar();
        if (!es("NUM_INT")) error("un numero entero dentro de los corchetes");
        n->hijos.push_back(crearNodo("Tamanio", actual().lexema));
        avanzar();
        coincidir("]");
    }
    return n;
}

// PARAMETROS -> TIPO ID LISTA_PARAMETROS | epsilon
NodoPtr PARAMETROS() {
    NodoPtr n = crearNodo("Parametros");
    if (es(")")) return n;
    NodoPtr p = crearNodo("Parametro");
    p->hijos.push_back(TIPO());
    if (!es("ID")) error("el identificador del parametro");
    p->hijos.push_back(crearNodo("Identificador", actual().lexema));
    avanzar();
    n->hijos.push_back(p);
    n->hijos.push_back(LISTA_PARAMETROS());
    return n;
}

// LISTA_PARAMETROS -> , TIPO ID LISTA_PARAMETROS | epsilon
NodoPtr LISTA_PARAMETROS() {
    NodoPtr n = crearNodo("ListaParametros");
    while (es(",")) {
        avanzar();
        NodoPtr p = crearNodo("Parametro");
        p->hijos.push_back(TIPO());
        if (!es("ID")) error("el identificador del parametro");
        p->hijos.push_back(crearNodo("Identificador", actual().lexema));
        avanzar();
        n->hijos.push_back(p);
    }
    return n;
}

// PRINCIPAL -> [ void ] main ( ) BLOQUE
// Se admiten main() (regla descrita) y void main() (ejemplos del enunciado).
NodoPtr PRINCIPAL() {
    NodoPtr n = crearNodo("Principal");
    if (es("void")) {
        n->hijos.push_back(crearNodo("TipoRetorno", "void"));
        avanzar();
    }
    coincidir("main", "la palabra reservada main");
    coincidir("(");
    coincidir(")");
    n->hijos.push_back(BLOQUE());
    return n;
}

// BLOQUE -> { LISTA_VARIABLES COMANDOS }
NodoPtr BLOQUE() {
    coincidir("{");
    NodoPtr n = crearNodo("Bloque");
    n->hijos.push_back(LISTA_VARIABLES());
    n->hijos.push_back(COMANDOS());
    coincidir("}", "'}' para cerrar el bloque");
    return n;
}

// LISTA_VARIABLES -> TIPO ID LISTA_ID ; LISTA_VARIABLES | epsilon
NodoPtr LISTA_VARIABLES() {
    NodoPtr n = crearNodo("ListaVariables");
    while (esTipoBase()) {
        NodoPtr d = crearNodo("DeclaracionVariable");
        d->hijos.push_back(TIPO());
        if (!es("ID")) error("un identificador despues del tipo");
        d->hijos.push_back(crearNodo("Identificador", actual().lexema));
        avanzar();
        d->hijos.push_back(LISTA_ID());
        coincidir(";");
        n->hijos.push_back(d);
    }
    return n;
}

// LISTA_ID -> , ID LISTA_ID | epsilon
NodoPtr LISTA_ID() {
    NodoPtr n = crearNodo("ListaIdentificadores");
    while (es(",")) {
        avanzar();
        if (!es("ID")) error("un identificador despues de la coma");
        n->hijos.push_back(crearNodo("Identificador", actual().lexema));
        avanzar();
    }
    return n;
}

// COMANDOS -> COMANDO COMANDOS | epsilon
NodoPtr COMANDOS() {
    NodoPtr n = crearNodo("Comandos");
    while (!es("}") && !es("EOF")) {
        try {
            n->hijos.push_back(COMANDO());
        } catch (ErrorSintactico& e) {
            erroresSintacticos.push_back(e.mensaje);
            // Recuperacion sencilla hasta un separador o el cierre del bloque.
            while (!es(";") && !es("}") && !es("EOF")) avanzar();
            if (es(";")) avanzar();
        }
    }
    return n;
}

// COMANDO -> LECTURA | ESCRITURA | ASIGNACION | FUNCION | SELECCION | MIENTRAS | RETORNO
NodoPtr COMANDO() {
    if (es("scanf")) return LECTURA();
    if (es("println")) return ESCRITURA();
    if (es("ID")) return ASIGNACION();
    if (es("func")) {
        NodoPtr n = FUNCION();
        coincidir(";");
        return n;
    }
    if (es("if")) return SELECCION();
    if (es("while")) return MIENTRAS();
    if (es("return")) return RETORNO();
    error("scanf, println, una asignacion, func, if, while o return");
    return crearNodo("Error");
}

// LECTURA -> scanf ( TERMINO_LECTURA NUEVO_TERMINO_LECTURA ) ;
NodoPtr LECTURA() {
    coincidir("scanf");
    NodoPtr n = crearNodo("Lectura", "scanf");
    coincidir("(");
    n->hijos.push_back(TERMINO_LECTURA());
    n->hijos.push_back(NUEVO_TERMINO_LECTURA());
    coincidir(")");
    coincidir(";");
    return n;
}

// TERMINO_LECTURA -> ID DIMENSION2
NodoPtr TERMINO_LECTURA() {
    if (!es("ID")) error("un identificador en scanf");
    NodoPtr n = crearNodo("Identificador", actual().lexema);
    avanzar();
    n->hijos.push_back(DIMENSION2());
    return n;
}

// NUEVO_TERMINO_LECTURA -> , TERMINO_LECTURA NUEVO_TERMINO_LECTURA | epsilon
NodoPtr NUEVO_TERMINO_LECTURA() {
    NodoPtr n = crearNodo("MasTerminosLectura");
    while (es(",")) {
        avanzar();
        n->hijos.push_back(TERMINO_LECTURA());
    }
    return n;
}

// DIMENSION2 -> [ EXPRESION_ADITIVA ] DIMENSION2 | epsilon
NodoPtr DIMENSION2() {
    NodoPtr n = crearNodo("Indices");
    while (es("[")) {
        avanzar();
        NodoPtr indice = crearNodo("Indice");
        indice->hijos.push_back(EXPRESION_ADITIVA());
        coincidir("]");
        n->hijos.push_back(indice);
    }
    return n;
}

// ESCRITURA -> println ( TERMINO_ESCRITURA NUEVO_TERMINO_ESCRITURA ) ;
NodoPtr ESCRITURA() {
    coincidir("println");
    NodoPtr n = crearNodo("Escritura", "println");
    coincidir("(");
    n->hijos.push_back(TERMINO_ESCRITURA());
    n->hijos.push_back(NUEVO_TERMINO_ESCRITURA());
    coincidir(")");
    coincidir(";");
    return n;
}

// TERMINO_ESCRITURA -> ID DIMENSION2 | CONSTANTE | TEXTO
NodoPtr TERMINO_ESCRITURA() {
    if (es("ID")) {
        NodoPtr n = crearNodo("Identificador", actual().lexema);
        avanzar();
        n->hijos.push_back(DIMENSION2());
        return n;
    }
    if (es("NUM_INT") || es("NUM_DEC")) return CONSTANTE();
    if (es("TEXTO")) {
        NodoPtr n = crearNodo("Texto", actual().lexema);
        avanzar();
        return n;
    }
    error("un identificador, una constante o un texto en println");
    return crearNodo("Error");
}

// NUEVO_TERMINO_ESCRITURA -> , TERMINO_ESCRITURA NUEVO_TERMINO_ESCRITURA | epsilon
NodoPtr NUEVO_TERMINO_ESCRITURA() {
    NodoPtr n = crearNodo("MasTerminosEscritura");
    while (es(",")) {
        avanzar();
        n->hijos.push_back(TERMINO_ESCRITURA());
    }
    return n;
}

// SELECCION -> if ( EXPRESION ) BLOQUE SINO
NodoPtr SELECCION() {
    coincidir("if");
    NodoPtr n = crearNodo("Seleccion", "if");
    coincidir("(");
    n->hijos.push_back(EXPRESION());
    coincidir(")");
    n->hijos.push_back(BLOQUE());
    n->hijos.push_back(SINO());
    return n;
}

// SINO -> else BLOQUE | epsilon
NodoPtr SINO() {
    NodoPtr n = crearNodo("Sino");
    if (es("else")) {
        avanzar();
        n->hijos.push_back(BLOQUE());
    }
    return n;
}

// MIENTRAS -> while ( EXPRESION ) BLOQUE
NodoPtr MIENTRAS() {
    coincidir("while");
    NodoPtr n = crearNodo("Mientras", "while");
    coincidir("(");
    n->hijos.push_back(EXPRESION());
    coincidir(")");
    n->hijos.push_back(BLOQUE());
    return n;
}

// ASIGNACION -> ID = COMPLEMENTO ;
NodoPtr ASIGNACION() {
    if (!es("ID")) error("un identificador al inicio de la asignacion");
    NodoPtr n = crearNodo("Asignacion");
    n->hijos.push_back(crearNodo("Identificador", actual().lexema));
    avanzar();
    coincidir("=");
    n->hijos.push_back(COMPLEMENTO());
    coincidir(";");
    return n;
}

// COMPLEMENTO -> EXPRESION | FUNCION
NodoPtr COMPLEMENTO() {
    if (es("func")) return FUNCION();
    return EXPRESION();
}

// FUNCION -> func ID ( ARGUMENTOS )
NodoPtr FUNCION() {
    coincidir("func");
    if (!es("ID")) error("el nombre de la funcion despues de func");
    NodoPtr n = crearNodo("LlamadaFuncion", actual().lexema);
    avanzar();
    coincidir("(");
    n->hijos.push_back(ARGUMENTOS());
    coincidir(")");
    return n;
}

// ARGUMENTOS -> EXPRESION NUEVO_ARGUMENTO | epsilon
NodoPtr ARGUMENTOS() {
    NodoPtr n = crearNodo("Argumentos");
    if (es(")")) return n;
    n->hijos.push_back(EXPRESION());
    n->hijos.push_back(NUEVO_ARGUMENTO());
    return n;
}

// NUEVO_ARGUMENTO -> , EXPRESION NUEVO_ARGUMENTO | epsilon
NodoPtr NUEVO_ARGUMENTO() {
    NodoPtr n = crearNodo("MasArgumentos");
    while (es(",")) {
        avanzar();
        n->hijos.push_back(EXPRESION());
    }
    return n;
}

// RETORNO -> return EXPRESION ;
NodoPtr RETORNO() {
    coincidir("return");
    NodoPtr n = crearNodo("Retorno", "return");
    n->hijos.push_back(EXPRESION());
    coincidir(";");
    return n;
}

// EXPRESION -> EXPRESION_O
NodoPtr EXPRESION() { return EXPRESION_O(); }

// EXPRESION_O -> EXPRESION_Y ( || EXPRESION_Y )*
NodoPtr EXPRESION_O() {
    NodoPtr izq = EXPRESION_Y();
    while (es("||")) {
        avanzar();
        NodoPtr op = crearNodo("OperacionLogica", "||");
        op->hijos.push_back(izq);
        op->hijos.push_back(EXPRESION_Y());
        izq = op;
    }
    return izq;
}

// EXPRESION_Y -> EXPRESION_RELACIONAL ( && EXPRESION_RELACIONAL )*
NodoPtr EXPRESION_Y() {
    NodoPtr izq = EXPRESION_RELACIONAL();
    while (es("&&")) {
        avanzar();
        NodoPtr op = crearNodo("OperacionLogica", "&&");
        op->hijos.push_back(izq);
        op->hijos.push_back(EXPRESION_RELACIONAL());
        izq = op;
    }
    return izq;
}

// EXPRESION_RELACIONAL -> EXPRESION_ADITIVA ( comp EXPRESION_ADITIVA | epsilon )
NodoPtr EXPRESION_RELACIONAL() {
    NodoPtr izq = EXPRESION_ADITIVA();
    if (es("COMP")) {
        string operador = actual().lexema;
        avanzar();
        NodoPtr op = crearNodo("OperacionRelacional", operador);
        op->hijos.push_back(izq);
        op->hijos.push_back(EXPRESION_ADITIVA());
        return op;
    }
    return izq;
}

// EXPRESION_ADITIVA -> EXPRESION_MULTIPLICATIVA ( (+ | -) EXPRESION_MULTIPLICATIVA )*
NodoPtr EXPRESION_ADITIVA() {
    NodoPtr izq = EXPRESION_MULTIPLICATIVA();
    while (es("+") || es("-")) {
        string operador = actual().lexema;
        avanzar();
        NodoPtr op = crearNodo("OperacionAditiva", operador);
        op->hijos.push_back(izq);
        op->hijos.push_back(EXPRESION_MULTIPLICATIVA());
        izq = op;
    }
    return izq;
}

// EXPRESION_MULTIPLICATIVA -> FACTOR ( (* | / | %) FACTOR )*
// En el documento, la segunda linea de esta produccion aparece con el
// nombre repetido; se interpreta como EXPRESION_MULTIPLICATIVA2.
NodoPtr EXPRESION_MULTIPLICATIVA() {
    NodoPtr izq = FACTOR();
    while (es("*") || es("/") || es("%")) {
        string operador = actual().lexema;
        avanzar();
        NodoPtr op = crearNodo("OperacionMultiplicativa", operador);
        op->hijos.push_back(izq);
        op->hijos.push_back(FACTOR());
        izq = op;
    }
    return izq;
}

// FACTOR -> SIGNO TERMINO | texto | ! FACTOR | ( EXPRESION )
NodoPtr FACTOR() {
    if (es("!")) {
        avanzar();
        NodoPtr n = crearNodo("Negacion", "!");
        n->hijos.push_back(FACTOR());
        return n;
    }
    if (es("TEXTO")) {
        NodoPtr n = crearNodo("Texto", actual().lexema);
        avanzar();
        return n;
    }
    if (es("(")) {
        avanzar();
        NodoPtr n = crearNodo("ExpresionAgrupada");
        n->hijos.push_back(EXPRESION());
        coincidir(")");
        return n;
    }
    string signo;
    if (es("+") || es("-")) {
        signo = actual().lexema;
        avanzar();
    }
    NodoPtr termino = TERMINO();
    if (!signo.empty()) {
        NodoPtr n = crearNodo("SignoUnario", signo);
        n->hijos.push_back(termino);
        return n;
    }
    return termino;
}

// TERMINO -> ID DIMENSION2 | CONSTANTE
NodoPtr TERMINO() {
    if (es("ID")) {
        NodoPtr n = crearNodo("Identificador", actual().lexema);
        avanzar();
        n->hijos.push_back(DIMENSION2());
        return n;
    }
    if (es("NUM_INT") || es("NUM_DEC")) return CONSTANTE();
    error("un identificador o una constante");
    return crearNodo("Error");
}

// CONSTANTE -> NUM_INT | NUM_DEC
NodoPtr CONSTANTE() {
    if (!es("NUM_INT") && !es("NUM_DEC")) error("una constante entera o decimal");
    NodoPtr n = crearNodo("Constante", actual().lexema);
    avanzar();
    return n;
}

// ============================================================
// ARCHIVOS DE SALIDA
// ============================================================
void guardarTokens() {
    ofstream out("tokens.txt");
    out << "LINEA\tCOLUMNA\tTIPO\tLEXEMA\n";
    for (int i = 0; i < (int)tokens.size(); i++)
        out << tokens[i].linea << "\t" << tokens[i].columna << "\t"
            << tokens[i].tipo << "\t" << tokens[i].lexema << "\n";
}

void guardarTablaSimbolos() {
    ofstream out("tabla_simbolos.txt");
    out << "ID\tNOMBRE\n";
    for (int i = 0; i < (int)nombresSimbolos.size(); i++)
        out << idsSimbolos[i] << "\t" << nombresSimbolos[i] << "\n";
}

void guardarErrores(const vector<string>& errores) {
    ofstream out("errores.txt");
    if (errores.empty()) out << "Sin errores.\n";
    else for (int i = 0; i < (int)errores.size(); i++) out << errores[i] << "\n";
}

// ============================================================
// PROGRAMA PRINCIPAL
// ============================================================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " archivo.lp" << endl;
        return 1;
    }

    ifstream f(argv[1]);
    if (!f) {
        cout << "No se pudo abrir " << argv[1] << endl;
        return 1;
    }

    string codigo, linea;
    while (getline(f, linea)) codigo += linea + "\n";
    f.close();

    // Vaciar el AST anterior evita confundir una ejecucion rechazada con
    // el arbol de un programa aceptado previamente.
    ofstream limpiarAST("ast.txt", ios::trunc);
    if (!limpiarAST) {
        cerr << "No se puede escribir ast.txt en la carpeta actual." << endl;
        return 2;
    }
    limpiarAST.close();

    vector<string> errores;
    tokens = analizarLexico(codigo);

    cout << "=== TOKENS ===\n";
    for (int i = 0; i < (int)tokens.size(); i++)
        cout << "Linea " << tokens[i].linea << ", columna " << tokens[i].columna
             << "\t" << tokens[i].tipo << "\t" << tokens[i].lexema << endl;

    cout << "=== TABLA DE SIMBOLOS ===\n";
    for (int i = 0; i < (int)nombresSimbolos.size(); i++)
        cout << idsSimbolos[i] << "\t" << nombresSimbolos[i] << endl;
    guardarTokens();
    guardarTablaSimbolos();

    for (int i = 0; i < (int)tokens.size(); i++) {
        if (tokens[i].tipo == "ERROR") {
            errores.push_back("Error lexico en linea " + to_string(tokens[i].linea) +
                ", columna " + to_string(tokens[i].columna) + ": " + tokens[i].lexema);
        }
    }

    if (!errores.empty()) {
        for (int i = 0; i < (int)errores.size(); i++) cout << errores[i] << endl;
        guardarErrores(errores);
        cout << "Programa RECHAZADO por errores lexicos." << endl;
        return 1;
    }

    cout << "=== ANALISIS SINTACTICO ===\n";
    int codigoSalida = 0;
    try {
        pos = 0;
        erroresSintacticos.clear();
        NodoPtr raiz = PROGRAMA();

        if (!erroresSintacticos.empty()) {
            for (int i = 0; i < (int)erroresSintacticos.size(); i++) {
                errores.push_back(erroresSintacticos[i]);
                cout << erroresSintacticos[i] << endl;
            }
            cout << "Programa RECHAZADO por errores sintacticos." << endl;
            codigoSalida = 1;
        } else {
            cout << "Programa ACEPTADO: la estructura es sintacticamente correcta." << endl;
            cout << "=== AST (ARBOL SINTACTICO ABSTRACTO) ===\n";
            imprimirAST(raiz);
            ofstream ast("ast.txt");
            guardarAST(ast, raiz);
            ast.close();
            cout << "AST guardado en ast.txt" << endl;
        }
    } catch (ErrorSintactico& e) {
        // Conservar tambien los errores recuperados antes del error fatal.
        for (const string& mensaje : erroresSintacticos) {
            errores.push_back(mensaje);
            cout << mensaje << endl;
        }
        errores.push_back(e.mensaje);
        cout << e.mensaje << endl;
        cout << "Programa RECHAZADO." << endl;
        codigoSalida = 1;
    }

    guardarErrores(errores);
    cout << "Archivos generados: tokens.txt, tabla_simbolos.txt, errores.txt";
    if (codigoSalida == 0) cout << " y ast.txt";
    cout << endl;
    return codigoSalida;
}
