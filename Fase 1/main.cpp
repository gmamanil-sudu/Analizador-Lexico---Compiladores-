#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {

    ifstream archivo("prueba.lp");

    if (!archivo.is_open()) {

        cout << "No se pudo abrir el archivo." << endl;
        return 1;
    }

    char c;
    string numero;

    while (archivo.get(c)) {

        if (c >= '0' && c <= '9') {

            numero = "";
            numero += c;

            while (archivo.peek() != EOF) {

                char siguiente = archivo.peek();

                if (siguiente >= '0' && siguiente <= '9') {
                    archivo.get(c);
                    numero += c;
                }
                else if (siguiente == '.') {
                    archivo.get(c);
                    numero += c;
                }
                else {
                    break;
                }
            }

            if (numero.find('.') != string::npos) {
                cout << "NUM_DEC -> " << numero << endl;
            }
            else {
                cout << "NUM_INT -> " << numero << endl;
            }
        }
    }

    archivo.close();

    return 0;
}
