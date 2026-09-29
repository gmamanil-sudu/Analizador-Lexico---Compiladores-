
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
struct Token {
    string tipo;
    int atributo;    // posicion en la tabla de simbolos (solo ID)
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

//tabla de simbolos

class TablaSimbolos {
public:
    int buscarOAgregar(const string& nombre) {
        auto it = indice.find(nombre);
        if (it != indice.end()) return it->second;

        int pos = (int)nombres.size();
        nombres.push_back(nombre);
        indice[nombre] = pos;
        return pos;
    }

    const vector<string>& obtenerNombres() const { return nombres; }

private:
    vector<string> nombres;
    unordered_map<string, int> indice;
};

//analizador lexico
class Lexer {
public:
    explicit Lexer(const string& codigo) : src(codigo) {}

    void analizar() {
        while (!fin()) {
            char c = actual();

            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                avanzar();
            } else if (esDigito(c)) {
                leerNumero();
            } else if (esLetra(c)) {
                leerIdentificador();
            } else if (c == '"') {
                leerTexto();
            } else if (c == '/' && siguiente() == '/') {
                saltarComentario();
            } else {
                leerSimbolo();
            }
        }
    }

    const vector<Token>& obtenerTokens() const { return tokens; }
    const vector<ErrorLexico>& obtenerErrores() const { return errores; }
    const TablaSimbolos& obtenerTabla() const { return tabla; }

private:
    string src;
    size_t pos = 0;
    int linea = 1;
    int columna = 0;   // columna del ultimo caracter consumido (el primero es 1)

    vector<Token> tokens;
    vector<ErrorLexico> errores;
    TablaSimbolos tabla;

   

    bool fin() const { return pos >= src.size(); }

    char actual() const { return fin() ? '\0' : src[pos]; }

    char siguiente() const {
        return (pos + 1 < src.size()) ? src[pos + 1] : '\0';
    }

    char avanzar() {
        char c = src[pos++];
        if (c == '\n') {
            linea++;
            columna = 0;
        } else if (((unsigned char)c & 0xC0) != 0x80) {
            columna++;   // los bytes de continuacion UTF-8 no abren columna nueva
        }
        return c;
    }

    static bool esDigito(char c) { return c >= '0' && c <= '9'; }

    static bool esLetra(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    static bool esLetraODigito(char c) { return esLetra(c) || esDigito(c); }

    // registro

    void agregarToken(const string& tipo, const string& lexema,
                      int lin, int col, int atributo = -1) {
        tokens.push_back({tipo, atributo, lexema, lin, col});
    }

    void agregarError(int lin, int col, const string& lexema,
                      const string& detalle) {
        errores.push_back({lin, col, lexema, detalle});
    }

    // palabra reservada

    static string tipoReservado(const string& palabra) {
        static const unordered_map<string, string> reservadas = {
            {"int", "INT"},         {"float", "FLOAT"}, {"char", "CHAR"},
            {"boolean", "BOOLEAN"}, {"void", "VOID"},
            {"if", "IF"},           {"else", "ELSE"},   {"for", "FOR"},
            {"while", "WHILE"},
            {"scanf", "SCANF"},     {"println", "PRINTLN"},
            {"main", "MAIN"},       {"return", "RETURN"}
        };
        auto it = reservadas.find(palabra);
        return (it == reservadas.end()) ? "" : it->second;
    }


    void leerNumero() {
        int linIni = linea;
        int colIni = columna + 1;

        string numero;
        int puntos = 0;

        while (!fin() && (esDigito(actual()) || actual() == '.')) {
            if (actual() == '.') puntos++;
            numero += avanzar();
        }

        // Numero pegado a letras: 1int
        if (!fin() && esLetra(actual())) {
            while (!fin() && esLetraODigito(actual())) numero += avanzar();
            agregarError(linIni, colIni, numero, "identificador mal formado");
            return;
        }

        // Varios puntos: 1.2.3
        if (puntos > 1) {
            agregarError(linIni, colIni, numero, "numero mal formado");
            return;
        }

        if (puntos == 1) {
            // Termina en punto: 12.
            if (numero.back() == '.') {
                agregarError(linIni, colIni, numero,
                             "numero decimal mal formado");
                return;
            }
            agregarToken("NUM_DEC", numero, linIni, colIni);
        } else {
            agregarToken("NUM_INT", numero, linIni, colIni);
        }
    }

    //palabras reservadas

    void leerIdentificador() {
        int linIni = linea;
        int colIni = columna + 1;

        string palabra;
        while (!fin() && esLetraODigito(actual())) palabra += avanzar();

        string reservado = tipoReservado(palabra);
        if (!reservado.empty()) {
            agregarToken(reservado, palabra, linIni, colIni);
        } else {
            int id = tabla.buscarOAgregar(palabra);
            agregarToken("ID", palabra, linIni, colIni, id);
        }
    }


    void leerTexto() {
        int linIni = linea;
        int colIni = columna + 1;

        avanzar();   // comilla de apertura
        string texto;

        while (!fin() && actual() != '"' && actual() != '\n') {
            texto += avanzar();
        }

        if (!fin() && actual() == '"') {
            avanzar();   // comilla de cierre
            agregarToken("TEXTO", texto, linIni, colIni);
        } else {
            // No se consume el '\n': lo procesa el bucle principal
            agregarError(linIni, colIni, "\"" + texto,
                         "texto sin comilla de cierre");
        }
    }


    void saltarComentario() {
        while (!fin() && actual() != '\n') avanzar();
    }

    //simbolos y operadores

    void leerSimbolo() {
        int linIni = linea;
        int colIni = columna + 1;

        char c = avanzar();

        switch (c) {
        case '=':
            if (actual() == '=') { avanzar(); agregarToken("COMP", "==", linIni, colIni); }
            else                 { agregarToken("=", "=", linIni, colIni); }
            return;
        case '!':
            if (actual() == '=') { avanzar(); agregarToken("COMP", "!=", linIni, colIni); }
            else                 { agregarToken("!", "!", linIni, colIni); }
            return;
        case '>':
            if (actual() == '=') { avanzar(); agregarToken("COMP", ">=", linIni, colIni); }
            else                 { agregarToken("COMP", ">", linIni, colIni); }
            return;
        case '<':
            if (actual() == '=') { avanzar(); agregarToken("COMP", "<=", linIni, colIni); }
            else                 { agregarToken("COMP", "<", linIni, colIni); }
            return;
        case '&':
            if (actual() == '&') { avanzar(); agregarToken("&&", "&&", linIni, colIni); }
            else agregarError(linIni, colIni, "&", "operador no reconocido");
            return;
        case '|':
            if (actual() == '|') { avanzar(); agregarToken("||", "||", linIni, colIni); }
            else agregarError(linIni, colIni, "|", "operador no reconocido");
            return;
        case '+': case '-': case '*': case '/': case '%':
        case '(': case ')': case '[': case ']': case '{': case '}':
        case ',': case ';':
            agregarToken(string(1, c), string(1, c), linIni, colIni);
            return;
        default:
            break;
        }

        // Caracter no reconocido (se agrupan los bytes UTF-8 de un mismo caracter)
        string seq(1, c);
        if (((unsigned char)c & 0xC0) == 0xC0) {
            while (!fin() && (((unsigned char)actual() & 0xC0) == 0x80))
                seq += avanzar();
        }
        agregarError(linIni, colIni, seq, "caracter no reconocido");
    }
};

//formato de salida
// <TIPO>   o   <ID,posicion>
string formatearToken(const Token& t) {
    if (t.tipo == "ID") return "<ID," + to_string(t.atributo) + ">";
    return "<" + t.tipo + ">";
}

// Un renglon por cada linea del programa fuente:
// <VOID> <MAIN> <(> <)> <{>
void escribirTokens(ostream& out, const vector<Token>& tokens) {
    int lineaActual = -1;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (lineaActual != -1)
            out << (tokens[i].linea != lineaActual ? '\n' : ' ');
        out << formatearToken(tokens[i]);
        lineaActual = tokens[i].linea;
    }
    out << endl;
}

// ID 0 | x
void escribirTabla(ostream& out, const TablaSimbolos& tabla) {
    const vector<string>& nombres = tabla.obtenerNombres();

    if (nombres.empty()) {
        out << "No hay identificadores." << endl;
        return;
    }
    for (size_t i = 0; i < nombres.size(); i++)
        out << "ID " << i << " | " << nombres[i] << endl;
}

// Linea | Columna | Lexema | ERROR_LEXICO | Detalle
void escribirErrores(ostream& out, const vector<ErrorLexico>& errores) {
    if (errores.empty()) {
        out << "Ninguno." << endl;
        return;
    }
    out << "Linea | Columna | Lexema | Resultado | Detalle" << endl;
    for (size_t i = 0; i < errores.size(); i++) {
        const ErrorLexico& e = errores[i];
        out << e.linea << " | " << e.columna << " | " << e.lexema
            << " | ERROR_LEXICO | " << e.detalle << endl;
    }
}

//archivoo de salida
template <typename Funcion>
void emitir(const string& titulo, const string& archivoSalida, Funcion escribir) {
    cout << "\n========================================\n"
         << "  " << titulo << "\n"
         << "========================================" << endl;
    escribir(cout);

    ofstream f(archivoSalida.c_str());
    if (!f.is_open()) {
        cerr << "No se pudo crear " << archivoSalida << endl;
        return;
    }
    escribir(f);
}

//main

int main(int argc, char* argv[]) {
    string nombreEntrada = (argc > 1) ? argv[1] : "prueba.lp";

    ifstream archivo(nombreEntrada.c_str(), ios::binary);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo " << nombreEntrada << "." << endl;
        return 1;
    }

    string codigo((istreambuf_iterator<char>(archivo)),
                   istreambuf_iterator<char>());
    archivo.close();

    Lexer lexer(codigo);
    lexer.analizar();

    emitir("LISTA DE TOKENS", "tokens.txt",
           [&](ostream& o) { escribirTokens(o, lexer.obtenerTokens()); });

    emitir("TABLA DE SIMBOLOS", "tabla_simbolos.txt",
           [&](ostream& o) { escribirTabla(o, lexer.obtenerTabla()); });

    emitir("ERRORES LEXICOS", "errores.txt",
           [&](ostream& o) { escribirErrores(o, lexer.obtenerErrores()); });

    return 0;
}
