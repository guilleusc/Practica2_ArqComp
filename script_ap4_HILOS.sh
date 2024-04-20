#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p2acgNN

# Codes must print only f value, you can use debug mode to print anything else (#define DEBUG) 

# Number of cores (change to desired number)

# Flag to compare
flag=0

gcc p2_apartado4.c  -o p24o0 -O0 -fopenmp

for N in 250 500 750 1000 1500 2000 2550 3000
do

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
echo "##################################"
echo "N: $N"
echo "##################################"

for C in 1 2 4 8 16 32 64
do

echo "Ejercicio 4 o0"
F6=`./p24o0 $N $C`
echo $F6

done 

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

rm p24o0


