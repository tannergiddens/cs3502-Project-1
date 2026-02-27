#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSACTIONS_PER_THREAD 10
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

void deposit_unsafe(int account_id, double amount, int teller_id) {
    double current = accounts[account_id].balance;
    usleep(1);  // force race window
    double new_balance = current + amount;
    accounts[account_id].balance = new_balance;
    accounts[account_id].transaction_count++;

    printf("Teller %d: Deposited $%.2f to Account %d\n",
           teller_id, amount, account_id);
}

void withdraw_unsafe(int account_id, double amount, int teller_id) {
    double current = accounts[account_id].balance;
    usleep(1);
    double new_balance = current - amount;
    accounts[account_id].balance = new_balance;
    accounts[account_id].transaction_count++;

    printf("Teller %d: Withdrew $%.2f from Account %d\n",
           teller_id, amount, account_id);
}

void* teller_thread(void* arg) {
    int id = *(int*)arg;
    unsigned int seed = time(NULL) ^ id;

    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        int acc = rand_r(&seed) % NUM_ACCOUNTS;
        double amt = (rand_r(&seed) % 100) + 1;

        if (rand_r(&seed) % 2)
            deposit_unsafe(acc, amt, id);
        else
            withdraw_unsafe(acc, amt, id);
    }

    return NULL;
}

int main() {
    printf("=== Phase 1: Race Condition Demo ===\n\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
    }

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, teller_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double expected = NUM_ACCOUNTS * INITIAL_BALANCE;
    double actual = 0.0;

    printf("\n=== Final Results ===\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%.2f (%d tx)\n",
               i, accounts[i].balance, accounts[i].transaction_count);
        actual += accounts[i].balance;
    }

    printf("\nExpected total: $%.2f\n", expected);
    printf("Actual total:   $%.2f\n", actual);

    if (actual != expected)
        printf("\nRACE CONDITION DETECTED\n");
    else
        printf("\nNo race detected (rare case)\n");

    return 0;
}
