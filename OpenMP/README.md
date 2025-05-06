Cracker OpenMP Larrea - Mishelle:

Compilación:  mpicc crackerLMMPI.c -o crackerLMMPI.exe -lcrypto -Wno-deprecated-declarations

Uso: mpirun -np <procesos> ./crackerLMMPI.exe -h <hash> [-v]

Utiliza la opción -v para que el programa imprima en pantalla todas las contraseñas que comprueba. Esta opción imprimirá billones de líneas, y ralentiza enormememente la búsqueda. 

Ejemplo:  mpirun -np 4 ./crackerLMMPI.exe -h 827ccb0eea8a706c4c34a16891f84e7b

Testado en linux  (WSL).

Si al prender el programa todos los procesos piensan que son el proceso 0, desinstala todas tus versiones de MPI y haz "sudo apt install openmpi-bin libopenmpi-dev", y recompila el programa. Este error suele ocurrir cuando le usuarie tiene más de una versión de MPI instalada.  

Para obtener un hash que comprobar, utiliza el programa hasher. Una vez que tengas un Hash, utiliza el cracker para revertir el proceso.
El cracker obtiene contraseñas con letras minúsculas y mayúsculas excepto la ñ, cifras, y los caracteres especiales !?+. Si introducieras un hash de una contraseña con un caracter fuera de esta especificación, nunca encontrará la contraseña y correrá para siempre. Utiliza CTRL+C para parar el programa.
Si deseas un programa que compruebe contraseñas con más caracteres, puedes editar las constantes DICT_LEN y ALPHA en el código, y compilar el programa.
