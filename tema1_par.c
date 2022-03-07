/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P; // threads
int widthJ, heightJ,widthM,heightM;
int **resultJ;
int **resultM;
pthread_barrier_t Zidu_Plangerii;

// structura pentru un numar complex
typedef struct _complex {
    double a;
    double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
    int is_julia, iterations;
    double x_min, x_max, y_min, y_max, resolution;
    complex c_julia;
} params;
params parJ, parM;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
    if (argc < 6) {
        printf("Numar insuficient de parametri:\n\t"
               "./tema1 fisier_intrare_julia fisier_iesire_julia "
               "fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
        exit(1);
    }

    in_filename_julia = argv[1];
    out_filename_julia = argv[2];
    in_filename_mandelbrot = argv[3];
    out_filename_mandelbrot = argv[4];
    P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
    FILE *file = fopen(in_filename, "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului de intrare!\n");
        exit(1);
    }

    fscanf(file, "%d", &par->is_julia);
    fscanf(file, "%lf %lf %lf %lf",
           &par->x_min, &par->x_max, &par->y_min, &par->y_max);
    fscanf(file, "%lf", &par->resolution);
    fscanf(file, "%d", &par->iterations);

    if (par->is_julia) {
        fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
    }

    fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
    int i, j;

    FILE *file = fopen(out_filename, "w");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului de iesire!\n");
        return;
    }

    fprintf(file, "P2\n%d %d\n255\n", width, height);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            fprintf(file, "%d ", result[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
    int **result;
    int i;

    result = malloc(height * sizeof(int*));
    if (result == NULL) {
        printf("Eroare la malloc!\n");
        exit(1);
    }

    for (i = 0; i < height; i++) {
        result[i] = malloc(width * sizeof(int));
        if (result[i] == NULL) {
            printf("Eroare la malloc!\n");
            exit(1);
        }
    }

    return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
    int i;

    for (i = 0; i < height; i++) {
        free(result[i]);
    }
    free(result);
}
int min(int x, int y)
{
    return (x < y) ? x : y;
}

void *flexTapeMyPain(void *arg)
{
    int thread_id = *(int *)arg;
    int startJ = thread_id * (double)widthJ / P;
    int endJ = min((thread_id + 1) * (double) widthJ/ P, widthJ);
        int w, h, i;
        // Julia:
        for (w = startJ; w < endJ; w++) {
            for (h = 0; h < heightJ; h++) {
                int stepJ = 0;
                complex zJ = { .a = w * parJ.resolution + parJ.x_min,
                        .b = h * parJ.resolution + parJ.y_min };

                while (sqrt(pow(zJ.a, 2.0) + pow(zJ.b, 2.0)) < 2.0 && stepJ < parJ.iterations) {
                    complex z_auxJ = { .a = zJ.a, .b = zJ.b };

                    zJ.a = pow(z_auxJ.a, 2) - pow(z_auxJ.b, 2) + parJ.c_julia.a;
                    zJ.b = 2 * z_auxJ.a * z_auxJ.b + parJ.c_julia.b;

                    stepJ++;
                }

                resultJ[h][w] = stepJ % 256;
            }
        }

        pthread_barrier_wait(&Zidu_Plangerii);
        // transforma rezultatul din coordonate matematice in coordonate ecran
        int startReverseJ = thread_id * (double)(heightJ / 2) / P;
        int endReverseJ = min((thread_id + 1) * (double) (heightJ / 2)/ P, heightJ / 2);

        for (i = startReverseJ; i < endReverseJ; i++) {
            int *auxJ = resultJ[i];
            resultJ[i] = resultJ[heightJ - i - 1];
            resultJ[heightJ - i - 1] = auxJ;
        }
    // - se scrie rezultatul in fisierul de iesire
        if(thread_id == 0)
        {
            write_output_file(out_filename_julia, resultJ, widthJ, heightJ);
        }
    pthread_barrier_wait(&Zidu_Plangerii);

    // Mandelbrot:
    int startM = thread_id * (double)widthM / P;
    int endM = min((thread_id + 1) * (double)widthM / P, widthM);
    for (w = startM; w < endM; w++) {
        for (h = 0; h < heightM; h++) {
            complex c = { .a = w * parM.resolution + parM.x_min,
                    .b = h * parM.resolution + parM.y_min };
            complex zM = { .a = 0, .b = 0 };
            int stepM = 0;

            while (sqrt(pow(zM.a, 2.0) + pow(zM.b, 2.0)) < 2.0 && stepM < parM.iterations) {
                complex z_auxM = { .a = zM.a, .b = zM.b };

                zM.a = pow(z_auxM.a, 2.0) - pow(z_auxM.b, 2.0) + c.a;
                zM.b = 2.0 * z_auxM.a * z_auxM.b + c.b;

                stepM++;
            }

            resultM[h][w] = stepM % 256;
        }
    }
    pthread_barrier_wait(&Zidu_Plangerii);
    // transforma rezultatul din coordonate matematice in coordonate ecran
    int startReverseM = thread_id * (double)(heightM / 2) / P;
    int endReverseM = min((thread_id + 1) * (double) (heightM / 2)/ P, heightM / 2);

    for (i = startReverseM; i < endReverseM; i++) {
        int *auxM = resultM[i];
        resultM[i] = resultM[heightM - i - 1];
        resultM[heightM - i - 1] = auxM;
    }
    // - se scrie rezultatul in fisierul de iesire
    if(thread_id == 0)
    {
        write_output_file(out_filename_mandelbrot, resultM, widthM, heightM);
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{

    int i, err;
    void *thread_status;
    // se citesc argumentele programului
    get_args(argc, argv);
    pthread_t threads[P];
    int arg[P];
    pthread_barrier_init(&Zidu_Plangerii,NULL, P);
    // Julia:
    // - se citesc parametrii de intrare
    read_input_file(in_filename_julia, &parJ);
    // - se citesc parametrii de intrare
    widthJ = (parJ.x_max - parJ.x_min) / parJ.resolution;
    heightJ = (parJ.y_max - parJ.y_min) / parJ.resolution;
    // - se aloca tabloul cu rezultatul
    resultJ = allocate_memory(widthJ, heightJ);
    // - se ruleaza algoritmul

    // Mandelbrot:
    // - se citesc parametrii de intrare
    read_input_file(in_filename_mandelbrot, &parM);
    widthM = (parM.x_max - parM.x_min) / parM.resolution;
    heightM = (parM.y_max - parM.y_min) / parM.resolution;
    // - se aloca tabloul cu rezultatul
    resultM = allocate_memory(widthM, heightM);
    // - se ruleaza algoritmul

    for (i = 0; i < P; i++) {
        arg[i] = i;
        err = pthread_create(&threads[i], NULL, flexTapeMyPain, &arg[i]);

        if (err) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
    }

    for (i = 0; i < P; i++) {
        err = pthread_join(threads[i], &thread_status);

        if (err) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
    }
    // - se elibereaza memoria alocata algoritmului Julia
    free_memory(resultJ, heightJ);
    // - se elibereaza memoria alocata algoritmului Mandelbrot:
    free_memory(resultM, heightM);
    pthread_barrier_destroy(&Zidu_Plangerii);
    return 0;
}
