int sumar(int a, int b) {
    return a + b;
}

float promedio(float a, float b) {
    return (a + b) / 2;
}

int primero(int[5] valores) {
    return valores[0];
}

void saludar() {
    println("Prueba integral del analizador LP");
}

void main() {
    int edad, resultado, indice;
    float media;
    boolean condicion;
    char letra;
    int[5] datos;
    int[2][3] matriz;

    func saludar();
    edad = 20;
    indice = 1;
    letra = "A";
    media = 15.5;

    scanf(edad, datos[0], matriz[1][2]);
    resultado = func sumar(edad, 5);
    media = func promedio(15.5, 18.5);
    resultado = func primero(datos);

    resultado = 2 + 3 * 4;
    resultado = (2 + 3) * 4;
    resultado = 20 / 2 % 3;
    resultado = 10 - 3 - 2;
    resultado = +edad + -resultado;
    resultado = datos[indice + 1] + matriz[1][2];

    condicion = !(edad < 18) && edad <= 60 || edad == 65;
    condicion = edad > 0 && edad != 100;
    condicion = edad >= 18;

    if (edad >= 18 && edad <= 60) {
        int limite;
        limite = 60;
        println("Edad valida", edad, limite);
        if (edad == 20) {
            println("Edad igual a veinte");
        } else {
            println("Otra edad");
        }
    } else {
        println("Edad fuera del intervalo");
    }

    if (edad != 0) {
        println("Edad distinta de cero");
    }

    while (edad > 0) {
        edad = edad - 1;
    }

    println("Resultados", resultado, media, letra, datos[0], matriz[1][2], 42, 3.5);
    return resultado;
}