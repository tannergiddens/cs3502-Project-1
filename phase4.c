#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000.0
#define NUM_THREADS 2
#define TRANSFERS_PER_THREAD 50

typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;
} Account;

static Account accounts[NUM_ACCOUNTS];

/*
 * Deadlock-free transfer using consistent lock ordering.
 * Always lock the lower account ID first.
 */
static void transfer_ordered(int from_id, int to_id, double amount, int teller_id) {
    if (from_id == to_id) return;

    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    if (accounts[from_id].balance >= amount) {
        accounts[from_id].balance -= amount;
        accounts[to_id].balance += amount;

        printf("Teller %d: Transferred $%.2f from Account %d to Account %d\n",
               teller_id, amount, from_id, to_id);
    }

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

typedef struct {
    int teller_id;
    int from;
    int to;
} ThreadArgs;

static void* transfer_thread(void* arg) {
    ThreadArgs* a = (ThreadArgs*)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(a->teller_id * 1234);

    for (int i = 0; i < TRANSFERS_PER_THREAD; i++) {
        double amount = (double)((rand_r(&seed) % 100) + 1);
        transfer_ordered(a->from, a->to, amount, a->teller_id);
    }

    return NULL;
}

int main(void) {
    printf("=== Phase 4: Deadlock Resolution with Timing ===\n\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].id = i;
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

    pthread_t t1, t2;
    ThreadArgs a1 = {0, 0, 1};
    ThreadArgs a2 = {1, 1, 0};

    pthread_create(&t1, NULL, transfer_thread, &a1);
    pthread_create(&t2, NULL, transfer_thread, &a2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

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

    if (total == NUM_ACCOUNTS * INITIAL_BALANCE) {
        printf("\nOK: completed without deadlock; total preserved.\n");
    } else {
        printf("\nERROR: total mismatch.\n");
    }

    printf("Execution time: %.6f seconds\n", elapsed);

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}

