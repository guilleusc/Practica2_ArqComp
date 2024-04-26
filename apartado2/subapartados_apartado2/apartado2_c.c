#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int N; // Tamaño de las matrices y vectores

void start_counter();
double get_counter();
double mhz();

/* Initialize the cycle counter */

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


/* Set *hi and *lo to the high and low order bits of the cycle counter.
Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
            : "=r" (*hi), "=r" (*lo) /* and move results to */
            : /* No input */ /* the two outputs */
            : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <valor_N>\n", argv[0]);
        return 1;
    }

    // Convertir el argumento a entero
    N = atoi(argv[1]);

    double ** a, ** b, * c, ** d, * e, f = 0, ck;
    int * ind;
    FILE * arquivo;

    /** Crear archivo de salida **/
    arquivo = fopen("resultados.txt","a+");
    if(arquivo == NULL)
    {
        printf("Error na apertura do arquvio\n");
        return 1;
    }

    /** Reservar memoria matrices e vectores **/
    a = (double** )malloc(N * sizeof(double *));
    for (int i = 0;i<N;i++)
    {
        a[i] = (double *)malloc(8 * sizeof(double));
    }
    b = (double** )malloc(8 * sizeof(double *));
    for (int i = 0;i<8;i++)
    {
        b[i] = (double *)malloc(N * sizeof(double));
    }
    d = (double** )malloc(N * sizeof(double *));
    for (int i = 0;i<N;i++)
    {
        d[i] = (double *)malloc(N, sizeof(double)); 
    }
    c = (double *)malloc(8 * sizeof(double));
    ind = (int *)malloc(N * sizeof(int));
    e = (double *)malloc(N * sizeof(double));
    /** Establecer valores iniciales **/
    srand(3); 
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            a[i][j] = (double) rand() / RAND_MAX;
            b[j][i] = (double) rand() / RAND_MAX;
            c[j] = (double)rand() / RAND_MAX;
        }
        ind[i] = i;
    }

    /** Desordenar vector de índices **/
    // Algoritmo de Fisher-Yates.
    for (int i = N - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = ind[i];
        ind[i] = ind[j];
        ind[j] = temp;
    }
    /**********************************/
    /** COMPUTACIÓN **/
    /* Código a medir*/
    start_counter();

   /* Inicializar a cero a matriz D */
    for (int i = 0; i < N; i++){
   	for (int j = 0; j < N; j++){
   		d[i][j] = 0;
   	}
    }
    
    for (int i = 0; i < N; i++)
    {
        for (int k = 0; k < 8; k++)
        {
            for (int j = 0; j < N; j++)
            {
                d[i][j] += 2 * a[i][k] * (b[k][j] - c[k]);
            }
        }
    }

    // Computación del vector e y de f
    for (int i = 0; i < N; i++)
    {
        e[i] = d[ind[i]][ind[i]] / 2;
        f += e[i];
    }

    ck = get_counter();

    // Imprimir el valor de f
    printf("f = %.2f\n", f);

    // Imprimir el valor de los ciclos medidos:
    fprintf(arquivo,"%1.10lf\t",ck);

    free(ind);
    free(e);
    free(c);

    for(int i = 0; i < N ;i++)
    {
        free(a[i]);
    }
    free(a);
    for(int i = 0; i < 8 ;i++)
    {
        free(b[i]);
    }
    free(b);
    for(int i = 0; i < N ;i++)
    {
        free(d[i]);
    }
    free(d);
    fclose(arquivo);

    return 0;
}
