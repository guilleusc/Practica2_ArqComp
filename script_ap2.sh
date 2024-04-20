#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p2acg10-versions2

# Codes must print only f value, you can use debug mode to print anything else (#define DEBUG) 

# Number of cores (change to desired number)
C=4

# Flag to compare
flag=0

gcc p2_apartado1.c  -o p21o0 -O0
gcc p2_apartado1.c  -o p21o2 -O2
gcc p2_apartado1.c  -o p21o3 -O3

gcc p2_apartado2_v1.c -o p2_v1 -O0
gcc p2_apartado2_v2.c -o p2_v2 -O0
gcc p2_apartado2_v3.c -o p2_v3 -O0
gcc p2_apartado2_v4.c -o p2_v4 -O0
gcc p2_apartado2_v5.c -o p2_v5 -O0
gcc p2_apartado2_v6.c -o p2_v6 -O0
gcc p2_apartado2_v7.c -o p2_v7 -O0
gcc p2_apartado2_v8.c -o p2_v8 -O0
gcc p2_apartado2_v9.c -o p2_v9 -O0
gcc p2_apartado2_v10.c -o p2_v10 -O0

for N in 250 500 750 1000 1500 2000 2550 3000
do

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
echo "##################################"
echo "N: $N"
echo "##################################"

echo "Ejercicio 1 o0"
F1=`./p21o0 $N`
echo $F1 

echo "Ejercicio 1 o2"
F2=`./p21o2 $N`
echo $F2

echo "Ejercicio 1 o3"
F3=`./p21o3 $N`
echo $F3

echo "Ejercicio 2 e"
F9=`./p2_v1 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v2 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v3 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v4 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v5 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v6 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v7 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v8 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v9 $N`
echo $F9

echo "Ejercicio 2 e"
F9=`./p2_v10 $N`
echo $F9

echo "##################################"
echo ""


# Saltar de linea
echo "" >> resultados.txt
done

# Separar este arquivo para este N
medianas=$(Rscript mediana.R resultados.txt)
echo $medianas >> datos.txt
rm resultados.txt
done


rm p21o0 p21o2 p21o3 p2_v1 p2_v2 p2_v3 p2_v4 p2_v5 p2_v6 p2_v7 p2_v8 p2_v9 p2_v10

