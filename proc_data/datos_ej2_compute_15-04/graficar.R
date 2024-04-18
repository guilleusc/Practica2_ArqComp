argumentos <- commandArgs(trailingOnly = TRUE) #Pasar como argumento o nombre que terá a grádifa (por ejemplo: "floats directos" ou algo así)
x <- c(250,500,750,1000,1500,2000,2550, 3000)
datos <- read.table(argumentos[1], header = TRUE)
num_columnas <- ncol(datos)

plot(NULL, xlim = c(1, nrow(datos)), ylim = c(min(datos), max(datos)),
     xlab = "Valores de N", ylab = "Ciclos", xaxt="n", main = argumentos[2])


# Trazar una línea para cada columna (serie de tiempo)
for (i in 1:num_columnas) {
  lines(datos[, i], type = "l", col = i, lwd = 2)
  points(1:nrow(datos), datos[, i], col = i, pch = 16) 
}

axis(1, at = seq(1, nrow(datos), length.out = length(x)), labels = x)
legend("topleft", legend = colnames(datos), col = 1:num_columnas, lty = 1, lwd = 2, cex = 0.8)


