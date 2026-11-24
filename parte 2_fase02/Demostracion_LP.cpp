/*
 * DEMOSTRACION AUTOMATICA DEL ENTREGABLE 2 PARA WINDOWS
 * Ejecutar sin argumentos: pruebas integrales, errores, AST y archivos de evidencia.
 * Ejecutar con archivo.lp: analiza solamente ese archivo, igual que el avance.
 * --sin-pausa: permite verificar la demostracion desde un script.
 *
 * Este controlador reutiliza el analizador comentado sin duplicar su logica.
 * Los textos de prueba se incluyen en el ejecutable: no necesita instalacion,
 * archivos externos, compilador ni bibliotecas adicionales de GCC.
 */
#define NOMINMAX
#include <windows.h>
#include <stdexcept>
#include <utility>
#define main ejecutarAnalizadorLP
#include "Analizador_Sintactico_LP_Parte2.cpp"
#undef main

struct CasoPrueba {
    const char* nombre;
    bool debeAceptar;
    const char* programa;
};

const CasoPrueba casos[] = {
    {"archivo_original_usuario", true, R"LPTEST(void main() {
    int edad;
    edad = 20;
}
)LPTEST"},
    {"asignacion_arreglos", true, R"LPTEST(void main() { int[5][3] a; a[0][1] = -(2 + 3); a[1 + 1][2] = a[0][1]; }
)LPTEST"},
    {"comparaciones", true, R"LPTEST(void main() { boolean b; b = 1 > 2 || 1 < 2 || 1 >= 2 || 1 <= 2 || 1 == 2 || 1 != 2; }
)LPTEST"},
    {"control_anidado", true, R"LPTEST(void main() { int x; x = 2; if (x > 0) { while (x > 0) { if (x == 1) { println(x); } else { println("otro"); } x = x - 1; } } }
)LPTEST"},
    {"ejemplo_oficial", true, R"LPTEST(void main() {
int edad;
float promedio;
edad = 20;
promedio = 15.5;
if (edad >= 18 && edad <= 60) {
println("Edad valida");
} else {
println("Edad no valida");
}
while (edad > 0) {
edad = edad - 1;
}
return edad;
}

)LPTEST"},
    {"integral", true, R"LPTEST(int suma(int a, int b) {
    return a + b;
}
void saludo() {
    println("Hola");
}
float media(float x, float y) {
    return (x + y) / 2;
}
int primero(int[5] datos) {
    return datos[0];
}
void main() {
    int edad, resultado;
    int[5][3] matriz;
    int[5] datos;
    float promedio;
    boolean listo;
    char letra;
    scanf(edad, datos[0], matriz[1][2]);
    datos[0] = -(edad + 2) * 3;
    matriz[1][2] = datos[0] % 2;
    resultado = func suma(edad, datos[0]);
    promedio = func media(15.5, 18.5);
    resultado = func primero(datos);
    func saludo();
    listo = !(edad < 18) && edad != 60 || edad == 20;
    resultado = +edad + -2 * 3 / 1 % 2;
    letra = "A";
    if (edad >= 18 && edad <= 60) {
        println("Edad valida", edad, 15.5, matriz[1][2]);
    } else {
        println("Edad no valida");
    }
    while (edad > 0) {
        edad = edad - 1;
    }
    return resultado;
}

)LPTEST"},
    {"precedencia", true, R"LPTEST(void main() { int x; x = 10 - 3 - 2 + 4 * 5; x = 1 || 2 && 3 < 4 + 5 * 6; x = 20 / 5 / 2; }
)LPTEST"},
    {"principal_anterior", true, R"LPTEST(main() {}
)LPTEST"},
    {"principal_void_vacio", true, R"LPTEST(void main() {}
)LPTEST"},
    {"texto_y_decimales", true, R"LPTEST(void main() { float x; x = 15.5; println("", "texto", 0, 15.5, x); }
)LPTEST"},
    {"tipos_retorno", true, R"LPTEST(char eco(char c) { return c; } boolean prueba(boolean b) { return !b; } float mitad(float x) { return x / 2; } void aviso() { println("ok"); } void main() { func aviso(); }
)LPTEST"},
    {"unarios", true, R"LPTEST(void main() { int x; x = -(1 + 2); x = +(-x); x = - -2; x = !(!x); }
)LPTEST"},
    {"argumento_faltante", false, R"LPTEST(void main() { func suma(1,); }
)LPTEST"},
    {"cadena_sin_cerrar", false, R"LPTEST(void main() { println("hola); }
)LPTEST"},
    {"comparacion_encadenada", false, R"LPTEST(void main() { boolean b; b = 1 < 2 < 3; }
)LPTEST"},
    {"dimension_decimal", false, R"LPTEST(void main() { int[2.5] a; }
)LPTEST"},
    {"error_lexico", false, R"LPTEST(void main() { int x; x = @; }
)LPTEST"},
    {"errores_y_llave_faltante", false, R"LPTEST(void main() { int x; x = ; println(,);
)LPTEST"},
    {"falta_main", false, R"LPTEST(int suma(int a, int b) { return a + b; }
)LPTEST"},
    {"falta_operando", false, R"LPTEST(void main() { int x; x = 1 + ; }
)LPTEST"},
    {"falta_parentesis", false, R"LPTEST(void main() { int x; x = (1 + 2; }
)LPTEST"},
    {"falta_punto_coma", false, R"LPTEST(void main() { int x; x = 1 }
)LPTEST"},
    {"indice_vacio", false, R"LPTEST(void main() { int[5] a; a[] = 1; }
)LPTEST"},
    {"main_duplicado", false, R"LPTEST(void main() {} void main() {}
)LPTEST"},
    {"parametro_faltante", false, R"LPTEST(int suma(int a, int) { return a; } void main() {}
)LPTEST"},
    {"println_vacio", false, R"LPTEST(void main() { println(); }
)LPTEST"},
    {"retorno_sin_expresion", false, R"LPTEST(void main() { return; }
)LPTEST"},
    {"scanf_constante", false, R"LPTEST(void main() { scanf(1); }
)LPTEST"},
    {"tipo_main_incorrecto", false, R"LPTEST(int main() {}
)LPTEST"},
    {"tokens_despues_main", false, R"LPTEST(void main() {} println("fuera");
)LPTEST"},
    {"vacio", false, R"LPTEST(
)LPTEST"},
    {"varios_errores", false, R"LPTEST(void main() { int x; x = ; println(,); x = 3; }
)LPTEST"}
};

// Las rutas Unicode se administran mediante Windows; el parser trabaja con
// nombres relativos ASCII dentro de cada carpeta de evidencia.
wstring unir(const wstring& base, const wstring& nombre) {
    return base + L"\\" + nombre;
}

wstring aUnicode(const string& valor) {
    return wstring(valor.begin(), valor.end()); // Los nombres de caso son ASCII.
}

void crearDirectorio(const wstring& ruta) {
    if (CreateDirectoryW(ruta.c_str(), NULL)) return;
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        DWORD atributos = GetFileAttributesW(ruta.c_str());
        if (atributos != INVALID_FILE_ATTRIBUTES &&
            (atributos & FILE_ATTRIBUTE_DIRECTORY)) return;
    }
    throw runtime_error("No se pudo crear una carpeta de resultados.");
}

void entrar(const wstring& ruta) {
    if (!SetCurrentDirectoryW(ruta.c_str()))
        throw runtime_error("No se pudo acceder a la carpeta de resultados.");
}

// El avance usa estado global. Se reinicia para que cada prueba sea independiente.
void reiniciarAnalizador() {
    nombresSimbolos.clear();
    idsSimbolos.clear();
    contadorId = 1;
    tokens.clear();
    erroresSintacticos.clear();
    pos = 0;
}

string leerTexto(const char* nombre) {
    ifstream archivo(nombre);
    if (!archivo) throw runtime_error(string("No se pudo leer ") + nombre);
    return string(istreambuf_iterator<char>(archivo), istreambuf_iterator<char>());
}

// Restaura la consola incluso si ocurre una excepcion mientras se guarda la salida.
struct CapturarConsola {
    streambuf* previoOut;
    streambuf* previoErr;
    explicit CapturarConsola(ostream& destino)
        : previoOut(cout.rdbuf(destino.rdbuf())), previoErr(cerr.rdbuf(destino.rdbuf())) {}
    ~CapturarConsola() { cout.rdbuf(previoOut); cerr.rdbuf(previoErr); }
};

int demostracion() {
    vector<wchar_t> modulo(32768);
    DWORD longitud = GetModuleFileNameW(NULL, modulo.data(), (DWORD)modulo.size());
    if (longitud == 0 || longitud >= modulo.size())
        throw runtime_error("No se pudo localizar el ejecutable.");
    wstring rutaExe(modulo.data(), longitud);
    wstring carpetaExe = rutaExe.substr(0, rutaExe.find_last_of(L"\\/"));
    wstring carpetaResultados = unir(carpetaExe, L"Resultados_LP_Entregable2");
    crearDirectorio(carpetaResultados);
    entrar(carpetaResultados);
    ofstream resumen("RESUMEN_PRUEBAS.txt");
    if (!resumen) throw runtime_error("No se pudo escribir el resumen de pruebas.");

    cout << "============================================================\n"
         << " ANALIZADOR SINTACTICO LP - ENTREGABLE 2\n"
         << " DEMOSTRACION AUTOMATICA\n"
         << "============================================================\n\n"
         << "Se ejecutan archivos validos e invalidos.\n"
         << "Un programa invalido RECHAZADO cuenta como prueba correcta.\n\n";
    resumen << "PRUEBA | ESPERADO | OBTENIDO | VERIFICACION\n";
    int correctas = 0;
    string astEjemplo;
    string erroresEjemplo;
    const int total = (int)(sizeof(casos) / sizeof(casos[0]));
    for (int i = 0; i < total; ++i) {
        const CasoPrueba& caso = casos[i];
        wstring carpeta = unir(carpetaResultados, aUnicode(caso.nombre));
        crearDirectorio(carpeta);
        entrar(carpeta);
        ofstream entrada("entrada.lp");
        entrada << caso.programa;
        entrada.close();
        if (!entrada) throw runtime_error("No se pudo guardar un archivo LP de prueba.");
        ofstream salida("salida_completa.txt");
        if (!salida) throw runtime_error("No se pudo guardar la salida de una prueba.");
        int resultado;
        {
            CapturarConsola captura(salida);
            reiniciarAnalizador();
            char nombrePrograma[] = "analizador";
            char nombreEntrada[] = "entrada.lp";
            char* argumentos[] = {nombrePrograma, nombreEntrada, NULL};
            resultado = ejecutarAnalizadorLP(2, argumentos);
        }
        salida.close();
        if (!salida) throw runtime_error("No se pudo completar la salida de una prueba.");
        bool aceptado = resultado == 0;
        bool hayAST = GetFileAttributesW(L"ast.txt") != INVALID_FILE_ATTRIBUTES;
        bool correcta = aceptado == caso.debeAceptar && hayAST == caso.debeAceptar;
        if (correcta) ++correctas;
        const char* esperado = caso.debeAceptar ? "ACEPTADO" : "RECHAZADO";
        const char* obtenido = aceptado ? "ACEPTADO" : "RECHAZADO";
        cout << "[" << (correcta ? "OK" : "FALLO") << "] " << caso.nombre
             << " -> " << obtenido << "\n";
        resumen << caso.nombre << " | " << esperado << " | " << obtenido
                << " | " << (correcta ? "CORRECTA" : "FALLO") << "\n";
        if (string(caso.nombre) == "ejemplo_oficial" && aceptado)
            astEjemplo = leerTexto("ast.txt");
        if (string(caso.nombre) == "errores_y_llave_faltante")
            erroresEjemplo = leerTexto("errores.txt");
    }
    cout << "\nRESULTADO: " << correctas << "/" << total << " pruebas correctas.\n";
    resumen << "\nRESULTADO: " << correctas << "/" << total << " pruebas correctas.\n";
    cout << "\n--- EJEMPLO DE ERRORES MULTIPLES ---\n" << erroresEjemplo;
    cout << "\n--- AST DEL EJEMPLO OFICIAL ---\n" << astEjemplo;
    cout << "\nLos resultados estan en Resultados_LP_Entregable2, junto a este ejecutable.\n"
         << "Cada carpeta contiene entrada.lp, salida_completa.txt, tokens.txt,\n"
         << "tabla_simbolos.txt, errores.txt y ast.txt cuando se acepta.\n"
         << "RESUMEN_PRUEBAS.txt contiene el resultado general.\n"
         << "El programa analiza la sintaxis de LP; no ejecuta sus instrucciones.\n";
    resumen.close();
    if (!resumen) throw runtime_error("No se pudo completar el resumen.");
    return correctas == total ? 0 : 1;
}

int main(int argc, char* argv[]) {
    // Un archivo como argumento conserva el uso normal del analizador.
    bool sinPausa = argc == 2 && string(argv[1]) == "--sin-pausa";
    if (argc > 1 && !sinPausa) return ejecutarAnalizadorLP(argc, argv);
    int resultado = 1;
    try {
        resultado = demostracion();
    } catch (const exception& e) {
        cerr << "\nNo se pudo completar la demostracion: " << e.what() << "\n"
             << "Coloca el ejecutable en una carpeta donde puedas guardar archivos.\n";
    }
    // Mantiene visible la ventana al abrir el EXE con doble clic.
    if (!sinPausa) {
        cout << "\nPresiona ENTER para cerrar..." << flush;
        string linea;
        getline(cin, linea);
    }
    return resultado;
}
