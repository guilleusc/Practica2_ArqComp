args <- commandArgs(trailingOnly = TRUE)

nombre_archivo <- args[1]

# Leer el archivo de texto
datos <- read.table(nombre_archivo, header = FALSE, sep = "\t")

# Calcular la mediana por columnas
medianas <- apply(datos, 2, median)

medianas <- head(medianas, -1)
# Imprimir por terminal el vector de medianas
cat(medianas, sep = '\n')
