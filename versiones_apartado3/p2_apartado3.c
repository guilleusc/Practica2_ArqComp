#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <immintrin.h>

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
    double ** a, ** b, **b_t, * c, ** d, f = 0, ck;
    int * ind, s, tam_linea;
    FILE * arquivo;
    __m512d c_vector, a_fila, b_columna, temp, e_vector;
    
    if (argc != 2)
    {
        printf("Uso: %s <valor_N>\n", argv[0]);
        return 1;
    }

    /* Convertir o argumento a enteiro */
    N = atoi(argv[1]);

    /* Crear archivo de salida */
    arquivo = fopen("resultados.txt","a+");
    if(arquivo == NULL)
    {
        printf("Error na apertura do arquvio\n");
        return 1;
    }

    tam_linea = sysconf(_SC_LEVEL1_DCACHE_LINESIZE); // Obter o tamaño da linea caché

    /* Reservar memoria matrices e vectores */
    a = (double** )_mm_malloc(N * sizeof(double *), tam_linea);
    for (int i = 0;i<N;i++)
    {
        a[i] = (double *)_mm_malloc(8 * sizeof(double), tam_linea);
    }
    b = (double** )_mm_malloc(8 * sizeof(double *), tam_linea);
    for (int i = 0;i<8;i++)
    {
        b[i] = (double *)_mm_malloc(N * sizeof(double), tam_linea);
    }
    d = (double** )_mm_malloc(N * sizeof(double *), tam_linea);
    for (int i = 0;i<N;i++)
    {
        d[i] = (double *)_mm_malloc(N * sizeof(double), tam_linea);
    }
    c = (double *)_mm_malloc(8 * sizeof(double), tam_linea);
    ind = (int *)_mm_malloc(N * sizeof(int), tam_linea);
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

    /* Inicialización a cero da matriz D */
    /* TODO posiblemente sexa boa idea facer esto con instrucciones AVX, de 8 en 8 ou así, ou incluso con msacaras*/
   for (int i = 0; i < N; i++){
   	for (int j = 0; j < N; j++){
   		d[i][j] = 0;
   	}
   }
    /* Tanspoñer a matriz B */
    b_t = (double** )_mm_malloc(N * sizeof(double *), tam_linea);
    for (int i = 0;i < N;i++)
    {
        b_t[i] = (double *)_mm_malloc(8 * sizeof(double), tam_linea);
        for(int j = 0; j < 8; j++){ /* TODO posiblemente sexa boa idea facer esto con instrucciones AVXZ*/
        	b_t[i][j] = b[j][i];
        }
    }
   
   
    /* Cálculos */
    c_vector = _mm512_load_pd(c);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            a_fila = _mm512_load_pd(a[i]);
            b_columna = _mm512_load_pd(b_t[j]);
            temp = _mm512_mul_pd(a_fila, _mm512_sub_pd(b_columna, c_vector));
            d[i][j] = 2 * _mm512_reduce_add_pd(temp);
        }
    }

    // Computación del vector e y de f
    
    for (s = 0; s < N - N%8; s += 8)
    {    	
    	e_vector = _mm512_div_pd(_mm512_setr_pd(d[ind[s]][ind[s]], d[ind[1+s]][ind[1+s]], d[ind[2+s]][ind[2+s]], d[ind[3+s]][ind[3+s]], d[ind[4+s]][ind[4+s]], d[ind[5+s]][ind[5+s]], d[ind[6+s]][ind[6+s]], d[ind[7+s]][ind[7+s]]), _mm512_set1_pd(2));
        f += _mm512_reduce_add_pd(e_vector);
    }
    for(int i = s; i < N; i ++)
    {
        f +=  d[ind[i]][ind[i]] / 2;
    }

    ck = get_counter();

    // Imprimir o valor de f
    printf("f = %.2f\n", f);

    // Imprimir o valor dos ciclos medidos:
    fprintf(arquivo,"%1.10lf\t",ck);

    _mm_free(ind);
    _mm_free(c);

    for(int i = 0; i < N ;i++)
    {
        _mm_free(a[i]);
    }
    free(a);
    
    for(int i = 0; i < N ;i++)
    {
        _mm_free(b_t[i]);
    }
    free(b_t);
    
    for(int i = 0; i < 8 ;i++)
    {
        _mm_free(b[i]);
    }
    _mm_free(b);
    for(int i = 0; i < N ;i++)
    {
        _mm_free(d[i]);
    }
    _mm_free(d);
    fclose(arquivo);


    return 0;
}
