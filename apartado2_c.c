#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 10 // Tamaño de las matrices y vectores
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


void imprimir_matriz(double matriz[][N], int filas, int columnas)
{
    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            printf("%.2f\t", matriz[i][j]);
        }
        printf("\n");
    }
}

/**
 *  TODO:
 *      - Mirar onde realmente se mide o código
 * */


int main()
{
    double ** a, ** b, * c, ** d, * e, f = 0, ck;
    int * ind;

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
        d[i] = (double *)calloc(N, sizeof(double)); // Para inicializar a cero a matriz
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

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < 8; k++)
            {
                d[i][j] += 2 * a[i][k] * (b[k][j] - c[k]);
            }
        }

        // Computación del vector e y de f
        e[ind[i]] = d[i][i] / 2;
        f += e[ind[i]];
    }

    ck = get_counter();

    // Imprimir el valor de f
    printf("f = %.2f\n", f);

    // Imprimir el valor de los ciclos medidos:
    printf("\n Clocks=%1.10lf \n",ck);

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
    return 0;
}