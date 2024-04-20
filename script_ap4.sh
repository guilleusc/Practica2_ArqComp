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

gcc p2_apartado2.c  -o p22 -O0

gcc p2_apartado3.c  -o p23 -O0 -march=native 

gcc p2_apartado4.c  -o p24o0 -O0 -fopenmp

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

echo "Ejercicio 2"
F4=`./p22 $N`
echo $F4

echo "Ejercicio 3"
F5=`./p23 $N`
echo $F5

echo "Ejercicio 4 o0"
F6=`./p24o0 $N $C`
echo $F6


echo "##################################"
echo ""

# Check f is the same in all codes
if [[ $F1 == $F2 ]] && [[ $F1 == $F3 ]] && [[ $F1 == $F4 ]] && [[ $F1 == $F5 ]] && [[ $F1 == $F6 ]] 
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

rm p21o0 p21o2 p21o3 p22 p23 p24o0

if [[ $flag == 0 ]]
then
    echo "Everything is fine"
else
    echo "Wrong F. Check it out."
fi

