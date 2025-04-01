// compile: gcc -fopenmp crackerLM.c -o crackerLM.exe -lcrypto -Wno-deprecated-declarations

#define SIZE 33
#define DICT_LEN 66
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJOKLMNOPQRSTUVWXYZ1234567890!?+"
// #define CHUNK 10000
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <time.h>

int hashMD5(const char *texto, char *md5_str)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)texto, strlen(texto), digest);

    // Convertimos el hash a string hexadecimal
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(&md5_str[i * 2], "%02x", digest[i]);
    }
    md5_str[MD5_DIGEST_LENGTH * 2] = '\0';
}
void int_to_pass(int num, char *str)
{
    char buffer[100];
    int index = 0;

    {
        while (num > 0)
        {
            num--;  // Ajuste para hacer la numeración 1-based
            int remainder = num % DICT_LEN;
            buffer[index++] = ALPHA[remainder];
            num /= DICT_LEN;
        }
    }
    buffer[index] = '\0'; // Agregar fin de cadena
    {
        // Invertir el resultado para obtener el orden correcto
        for (int i = 0; i < index / 2; i++)
        {
            char temp = buffer[i];
            buffer[i] = buffer[index - i - 1];
            buffer[index - i - 1] = temp;
        }
        // Copiar el resultado a str (sobreescribiendo el contenido previo)
        strcpy(str, buffer);
    }   
}
int check(int long candidata, char secreto[])
{
    char pass[100];
    char passMD5[100];
    int_to_pass(candidata,pass);
    hashMD5(pass,passMD5);
    //Des-comenta la linea inferior para imprimir 40 millones de líneas
    //printf("%li - comprobando contraseña %s - %s\n",candidata, pass, passMD5);
    
    int result = 0;
    // si es la contraseña, return 1
    if(strcmp(passMD5, secreto) == 0 ){
        printf("Los hashes coinciden:\n");
        printf("%s \n",passMD5);
        printf("%s \n",secreto);
        result = 1;
    }
    // si no es la contraseña, return 0
    return result;
}

int checkMD5(char entrada[])  //Esta función sirve para devolver al usuario información sobre el uso correcto del programa
{
    if (strlen(entrada) > 32)
    {
        printf("Lo que has introducido no es un HASH MD5, es demasiado largo\n");
        return 1;
    }
    if (strlen(entrada) < 32)
    {
        printf("Lo que has introducido no es un HASH MD5, es demasiado corto\n");
        return 1;
    }
    for (int i = 0; i < 32; i++)
    {

        if (!isxdigit((unsigned char)entrada[i]))
        {
            printf("Lo que has introducido no es un HASH MD5, no está en hexadecimal.\n");
            return 1;
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    unsigned char secreto[SIZE];
    int long candidata;
    unsigned char hash[SIZE];
    if (argc != 2)
    {
        printf("Error, uso: %s <hash>\n", argv[0]);
        return 1;
    }

    strncpy(secreto, argv[1], SIZE);
    printf("Secreto: %s\n", secreto);
    candidata = 0;
    int md5Flag = checkMD5(secreto);
    if (md5Flag != 0) //si no es un hash MD5, sal del programa.
    {
        return 1;
    }
    printf("Buscando contraseña...\n");
    int encontrado = 0;
    while (encontrado == 0)
    {
        //el programa asigna la contraseña "a" al numero 1, la "b" al numero 2, la aa al numero 66, etc. Cuando encuentra la contraseña con el hash correcto, para.
        candidata++;
        encontrado = check(candidata, secreto);
    }
    printf("encontrada contraseña: ");
    char resultado[100];
    int_to_pass(candidata--,resultado);
    printf("%s \n", resultado);
    return 0;
}
