int suma(int a, int b) {
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

