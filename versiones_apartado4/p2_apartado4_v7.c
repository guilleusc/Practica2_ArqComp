#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

int N; // Tamaño de las matrices y vectores
int C; // Número de cores

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
    if (argc != 3)
    {
        printf("Uso: %s <valor_N> <valor_C>\n", argv[0]);
        return 1;
    }

    /* Convertir el argumento a entero */
    N = atoi(argv[1]);
    C = atoi(argv[2]);

    double ** a, ** b, * c, ** d, * e, f = 0, ck, suma_particular = 0;
    int * ind;
    FILE * arquivo;

    /* Crear archivo de salida */
    arquivo = fopen("resultados.txt","a+");
    if(arquivo == NULL)
    {
        printf("Error na apertura do arquvio\n");
        return 1;
    }

    /* Reservar memoria matrices e vectores */
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
	d[i] = (double *)malloc(N * sizeof(double)); 
    }    
    c = (double *)malloc(8 * sizeof(double));
    ind = (int *)malloc(N * sizeof(int));
    e = (double *)malloc(N * sizeof(double));
    /* Establecer valores iniciales */
    
    srand(3); // Inicializar semilla
    
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
   #pragma omp parallel private(suma_particular) num_threads(C)
   {
   #pragma omp for schedule(static)
   for (int i = 0; i < N; i++){
   	for (int j = 0; j < N; j++){
   		d[i][j] = 0;
   	}
   }    
   #pragma omp barrier /* Esperar a que se acabe a inicilización a cero da matriz D, para proceder cos cálculos*/
   
   /* Cálculos */
   #pragma omp for schedule(static)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            d[i][j] +=  2 * ( a[i][0] * (b[0][j] - c[0]) + a[i][1] * (b[1][j] - c[1]) + a[i][2] * (b[2][j] - c[2]) + a[i][3] * (b[3][j] - c[3]) + a[i][4] * (b[4][j] - c[4]) + a[i][5] * (b[5][j] - c[5]) + a[i][6] * (b[6][j] - c[6]) + a[i][7] * (b[7][j] - c[7]) );
        }
    }
    #pragma omp barrier /* Esperar polo cálculo completo da matriz D*/
    // Computación del vector e y de f
    #pragma omp for schedule(static)
    for(int i = 0; i < N; i ++)
    {
        e[i] =  d[ind[i]][ind[i]] / 2;
        suma_particular += e[i];
    }
    #pragma omp atomic // Para evitar carreiras criticas
    f += suma_particular;
    } // Corchete de pragma omp
    
    ck = get_counter();

    // Imprimir o valor de f
    printf("f = %.2f\n", f);

    // Imprimir o valor dos ciclos medidos:
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
