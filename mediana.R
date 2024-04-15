args -> commandArgs(trailingOnly = TRUE)

# Leer el archivo de texto
datos <- read.table(args[1], header = FALSE, sep = "\t")

# Calcular la mediana por columnas
medianas <- apply(datos, 2, median)

# Imprimir por terminal el vector de medianas
print(medianas)