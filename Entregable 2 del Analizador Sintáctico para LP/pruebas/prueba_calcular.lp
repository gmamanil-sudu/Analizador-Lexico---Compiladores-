int calcular(int a, int b){
    int resultado;
    resultado = a+b*2;
    return resultado;
}

void main(){
    int x;
    int y;
    int resultado;

    scanf(x, y);
    resultado = func calcular(x,y);
    if(resultado >= 20){
        println("Resultado mayor igual que 20");
    }
    else{
        println("Resultado menor a 20");
    }
    while(x>0){
        x=x-1;
    }
    return resultado;
}