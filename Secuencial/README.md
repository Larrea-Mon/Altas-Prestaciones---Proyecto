Cracker Secuencial Larrea - Mishelle:

Compilación: gcc -fopenmp crackerLM.c -o crackerLM.exe -lcrypto -Wno-deprecated-declarations

Uso: ./crackerLM.exe <hash>

Ejemplo: ./crackerLM.exe 4d186321c1a7f0f354b297e8914ab240

Testado en linux (WSL).


Para obtener un hash que comprobar, utiliza el programa hasher. Una vez que tengas un Hash, utiliza el cracker para revertir el proceso.
El cracker obtiene contraseñas con letras minúsculas y mayúsculas excepto la ñ, cifras, y los caracteres especiales !?+. Si introducieras un hash de una contraseña con un caracter fuera de esta especificación, nunca encontrará la contraseña y correrá para siempre. Utiliza CTRL+C para parar el programa. 
