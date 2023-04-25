#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N_EXEC 1000000 // número de execuções do método para calcular o tempo médio
#define MAX_NR_ITER 4 // número máximo de iterações do método de Newton-Raphson
#define N_VALUES 100 // número de valores a serem testados para benchmark
#define FILE_NAME "plot_data_1000.csv" // nome do arquivo para salvar os dados do benchmark
#define EPSILON 5e-17 // epsilon, tolerância usada como critério de parada do método de Newton-Raphson

#define SQRT_2 1.41421356237309504880168872420969807 // raiz quadrada de 2

// definição de um tipo union para manipulação dos bits de um float em IEEE 754
typedef union {
    float f;
    struct
    {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
 
    } bits;
} f_ieee_754;

// função para converter um float em IEEE 754 para seus componentes
void float_to_ieee_754(float x, char *sign, int* exponent, float *mantissa) {
    f_ieee_754 x_ieee;
    x_ieee.f = x;
    *exponent = x_ieee.bits.exponent - 127;
    *sign = x_ieee.bits.sign;
    *mantissa = (float)x_ieee.bits.mantissa / (1 << 23);
}

// função para converter os componentes de um float em IEEE 754 para o float original
float ieee_754_to_float(char sign, float mantissa, int exponent) {
    int exp = exponent + 127;
    unsigned int mantissa_bits = (unsigned int)(mantissa * (1 << 23));
    mantissa_bits |= 1 << 23;
    f_ieee_754 float_bits;
    float_bits.bits.sign = sign;
    float_bits.bits.exponent = exp;
    float_bits.bits.mantissa = mantissa_bits;
    return float_bits.f;
}

float inv_sqrt_math(float x) {
    return 1.0 / sqrt(x);
}

// Implementação do método de Newton-Raphson para calcular a raiz quadrada
float nr_sqrt (float x) {
    if (x == 0) return INFINITY;

    float mantissa;
    int exponent;
    char sign;

    float_to_ieee_754(x, &sign, &exponent, &mantissa);

    int is_odd = 0;
    if (exponent & 1) {
        is_odd = 1;
        exponent--;
    }
    exponent >>= 1;
    
    double xk;
    double xk_1 = 1.0 + mantissa * 0.5;
    mantissa += 1.0;
    int k = 0;
    do {
        xk = xk_1;
        xk_1 = 0.5 * (xk + mantissa / xk);
        k++;
    } while(fabs(xk_1 - xk) > EPSILON && k < MAX_NR_ITER);

    float result;
    if (is_odd) result = (xk_1 * SQRT_2);
    else result = xk_1;

    return 1 / ieee_754_to_float(sign, result, exponent);
}


// Implementação do método de Newton-Raphson para calcular a raiz quadrada inversa
float nr_invsqrt (float x) {
    if (x == 0) return INFINITY;

    float mantissa;
    int exponent;
    char sign;

    float_to_ieee_754(x, &sign, &exponent, &mantissa);

    int is_odd = 0;
    if (exponent & 1) {
        is_odd = 1;
        exponent++;
    }
    exponent >>= 1;
    exponent = -exponent;

    double xk;
    double xk_1 = 2.0 / (2 + mantissa);
    mantissa += 1.0;
    double mantissa_half = mantissa * 0.5;
    int k = 0;
    do {
        xk = xk_1;
        xk_1 = xk * (1.5 - mantissa_half * xk * xk);
        k++;
    } while(fabs(xk_1 - xk) > EPSILON && k < MAX_NR_ITER);

    float result = xk_1;
    if (is_odd) result = (xk_1 * SQRT_2);
    else result = xk_1;

    if (result < 1) {
        result *= 2;
        exponent -= 1;
    }

    return ieee_754_to_float(sign, result, exponent);
}


// Implementação do método de Fast sqrt de Tarolli
float fast_invsqrt_tarolli(float x) {
    float xhalf = 0.5f * x;
    union {
        float x;
        int k;
    } u;
    u.x = x;
    u.k = 0x5f375286 - (u.k >> 1); // what the fuck?
    u.x = u.x * (1.5f - xhalf * u.x * u.x);     
    return u.x;
}

struct benchmark_result {
    char* method_name;
    float exec_time;
    float result;
    float error;
};

struct benchmark_result benchmark(float x, float (*method_func)(float), char* method_name) {
    clock_t start_time = clock();
    for (int i = 0; i < N_EXEC; i++) method_func(x);
    clock_t end_time = clock();
    float result = method_func(x);
    float real = inv_sqrt_math(x);
    struct benchmark_result b_result = {
        method_name,
        ((double)(end_time - start_time)) / CLOCKS_PER_SEC,
        result,
        fabsf(result - real)
    };
    return b_result;
}

void print_info(float x, float (*method_func)(float), char* method_name) {
    struct benchmark_result b_result = benchmark(x, method_func, method_name);
    printf("\n%s: %.25f\n", method_name, b_result.result);
    printf("Tempo de execucao (%dx): %f segundos\n", N_EXEC, b_result.exec_time);
    printf("Erro comparado com a funcao sqrt(): %.25f\n", b_result.error);
    return;
}

void increment_and_store_errors(float (*method_func[])(float x), char* method_name[], int n, char* file_name) {
    FILE *fp;
    errno_t error = fopen_s(&fp, file_name, "w"); // Abrir arquivo CSV para escrita no modo de acrescentamento
    if (error != 0) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return;
    }
    
    fprintf(fp, "Valor,Metodo,Resultado,Tempo(s),Erro\n");

    int x = 0;
    while (x++ < N_VALUES) {
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%d,", x);

            float real = inv_sqrt_math(x);

            struct benchmark_result b_result = benchmark(x, method_func[i], method_name[i]);

            fprintf(fp, "%s,", b_result.method_name);
            fprintf(fp, "%.20f,", b_result.result);
            fprintf(fp, "%.20f,", b_result.exec_time);
            fprintf(fp, "%.20f\n", b_result.error);

            printf("Adicionado resultado para x = %d\n", x);
        }
    }
    fclose(fp);
    return;
}

int main() {
    int n = 4;
    float (*method_func[4])(float x) = {inv_sqrt_math, nr_sqrt, nr_invsqrt, fast_invsqrt_tarolli};
    char *method_name[4] = {"math.h", "NR-sqrt", "NR-invsqrt", "Tarolli"};

    printf("Deseja gerar um arquivo para plot do grafico? (S/N)\n");
    char choice;
    scanf_s(" %c", &choice, sizeof(choice));

    if (choice == 'S' || choice == 's') {
        char file_name[50];
        sprintf(file_name, FILE_NAME);
        increment_and_store_errors(method_func, method_name, n, file_name);
        printf("Dados do grafico armazenados em %s\n", file_name);
    } else {
        float x;
        printf("Digite o valor de x: ");
        scanf_s("%f", &x, sizeof(x));
        
        float real = inv_sqrt_math(x);
        for (int i = 0; i < n; i++) print_info(x, method_func[i], method_name[i]);
    }
}
