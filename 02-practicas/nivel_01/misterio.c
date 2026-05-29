#include <stdio.h>
#include <string.h>

int procesar_entrada(char *entrada)
{
    int contador = 0;
    char buffer[20];
    strcpy(buffer, entrada);

   for (int i = 0; i < strlen(entrada); i++)
   {
       if (entrada[i] >= 'A' && entrada[i] <= 'Z')
           contador++;
   }

    printf("Buffer contiene: %s\n", buffer);
    printf("Conteo de mayusculas: %d\n", contador);
    return contador;
}

int main(int ac, char **av)
{
    if (ac < 2)
    {
        printf("error entry\n");
        return 1;
    }
    int resultado = procesar_entrada(av[1]);
    printf("Resulado final: %d\n", resultado);
    return 0;
}
