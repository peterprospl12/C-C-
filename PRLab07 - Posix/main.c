#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int a;
    int b;
    int *arr;
    int *curr_pos;
} PrimeArgs;

void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


pthread_mutex_t m_p;

void *isPrime(void *args) {
    PrimeArgs *arguments = (PrimeArgs *) args;
    int a = arguments->a;
    int b = arguments->b;
    int *arr = arguments->arr;
    int *curr_pos = arguments->curr_pos;

    while (a < b) {
        bool is_prime = true;
        if (a == 0 || a == 1) {
            is_prime = false;
        }

        for (int i = 2; i <= a / 2; i++) {
            if (a % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            pthread_mutex_lock(&m_p);
            arr[(*curr_pos)++] = a;
            pthread_mutex_unlock(&m_p);
        }
        a += 1;
    }
    return NULL;
}

int main(void) {
    int a;
    int b;
    scanf("%d", &a);
    scanf("%d", &b);
    int iterations = b - a;
    int *arr = (int *) malloc((iterations) * sizeof(int));
    int curr_p = 0;
    pthread_mutex_init(&m_p, NULL);
    pthread_t *threads = malloc(iterations * sizeof(pthread_t));
    PrimeArgs *args = malloc(iterations * sizeof(PrimeArgs));

    int i = 0;
    while (a < b) {
        args[i].a = a;
        args[i].b = a + 10;
        args[i].arr = arr;
        args[i].curr_pos = &curr_p;
        pthread_create(&threads[i], NULL, isPrime, &args[i]);
        a += 10;
        i++;
    }
    for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
    }


    int j;
    bool swapped;
    for (i = 0; i < curr_p - 1; i++) {
        swapped = false;
        for (j = 0; j < curr_p - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }


    for (int z = 0; z < curr_p; z++) {
        printf("%d ", arr[z]);
    }


    free(arr);
    free(threads);
    free(args);
    return 0;
}
