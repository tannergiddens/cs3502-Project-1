#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSFERS_PER_THREAD 10
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void transfer_safe(int from, int to, double amount, int teller_id) {
    if (from == to) return;

    int first = (from < to) ? from : to;
    int second = (from < to) ? to : from;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    if (accounts[from].balance >= amount) {
        accounts[from].balance -= amount;
        accounts[to].balance += amount;

        printf("Teller %d: Transferred $%.2f from Account %d to Account %d\n",
               teller_id, amount, from, to);
    }

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

void* teller_thread(void* arg) {
    int teller_id = *(int*)arg;
    unsigned int seed = time(NULL) ^ teller_id;

    for (int i = 0; i < TRANSFERS_PER_THREAD; i++) {
        int from = rand_r(&seed) % NUM_ACCOUNTS;
        int to = (from + 1) % NUM_ACCOUNTS;
        double amount = (rand_r(&seed) % 100) + 1;

        transfer_safe(from, to, amount, teller_id);
    }

    return NULL;
}

int main() {
    printf("=== Phase 2: Mutex Protection (With Timing) ===\n\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    printf("Initial State:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf(" Account %d: $%.2f\n", i, accounts[i].balance);
    }

    // Start timing
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, teller_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Stop timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("\n=== Final Results ===\n");

    double total = 0.0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf(" Account %d: $%.2f\n", i, accounts[i].balance);
        total += accounts[i].balance;
    }

    printf("\nExpected total: $%.2f\n", NUM_ACCOUNTS * INITIAL_BALANCE);
    printf("Actual total:   $%.2f\n", total);

    if (total == NUM_ACCOUNTS * INITIAL_BALANCE)
        printf("\nOK: totals match (mutexes working)\n");
    else
        printf("\nERROR: totals differ\n");

    printf("Execution time: %.6f seconds\n", elapsed);

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
