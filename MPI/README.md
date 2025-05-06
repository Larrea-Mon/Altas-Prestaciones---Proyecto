Cracker MPI Larrea - Mishelle:

Compilación: gcc -fopenmp crackerLMOpenMP.c -o crackerLMOpenMP.exe -lcrypto -Wno-deprecated-declarations

Uso: ./crackerLMOpenMP.exe -t <hilos> -h <hash> [-v]

Utiliza la opción -v para que el programa imprima en pantalla todas las contraseñas que comprueba. Esta opción imprimirá billones de líneas, y ralentiza enormememente la búsqueda. 


Ejemplo: ./crackerLMOpenMP.exe -t 16 -h 4d186321c1a7f0f354b297e8914ab240 -v

Testado en linux  (WSL).


Para obtener un hash que comprobar, utiliza el programa hasher. Una vez que tengas un Hash, utiliza el cracker para revertir el proceso.
El cracker obtiene contraseñas con letras minúsculas y mayúsculas excepto la ñ, cifras, y los caracteres especiales !?+. Si introducieras un hash de una contraseña con un caracter fuera de esta especificación, nunca encontrará la contraseña y correrá para siempre. Utiliza CTRL+C para parar el programa.
Si deseas un programa que compruebe contraseñas con más caracteres, puedes editar las constantes DICT_LEN y ALPHA en el código, y compilar el programa.
