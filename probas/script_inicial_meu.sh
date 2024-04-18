#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p2acgNN

# Codes must print only f value, you can use debug mode to print anything else (#define DEBUG) 

# Number of cores (change to desired number)
C=4

# Flag to compare
flag=0

gcc p2_apartado1.c  -o p21o0 -O0
gcc p2_apartado1.c  -o p21o2 -O2
gcc p2_apartado1.c  -o p21o3 -O3

gcc apartado2_a.c  -o p22a -O0
gcc apartado2_b.c  -o p22b -O0
gcc apartado2_c.c  -o p22c -O0
gcc apartado2_d.c  -o p22d -O0
gcc apartado2_e.c  -o p22e -O0

for N in 250 500
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

echo "Ejercicio 2 a"
F4=`./p22a $N`
echo $F4

echo "Ejercicio 2 b"
F5=`./p22b $N`
echo $F5

echo "Ejercicio 2 c"
F6=`./p22c $N`
echo $F6

echo "Ejercicio 2 d"
F7=`./p22d $N`
echo $F7

echo "Ejercicio 2 e"
F8=`./p22e $N`
echo $F8

echo "##################################"
echo ""

# Check f is the same in all codes
if [[ $F1 == $F2 ]] && [[ $F1 == $F3 ]] && [[ $F1 == $F4 ]] && [[ $F1 == $F5 ]] && [[ $F1 == $F6 ]] && [[ $F1 == $F7 ]]
then
    echo "Everything is fine so far"
else
    flag=1
fi

# Saltar de linea
echo "" >> resultados.txt
done

# Separar este arquivo para este N
medianas=$(Rscript mediana.R resultados.txt)
echo $medianas >> datos.txt
rm resultados.txt
done



if [[ $flag == 0 ]]
then
    echo "Everything is fine"
else
    echo "Wrong F. Check it out."
fi

