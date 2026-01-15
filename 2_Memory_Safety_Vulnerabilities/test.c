#include <stdio.h>
#include <stdlib.h>

int sum(int, int);

int main(int argc, char *argv[]){
    int a = 5, b = 10;

    printf("%d\n", sum(a, b));

    return EXIT_SUCCESS;
}
int sum(int x, int y){
    return x + y;
}
