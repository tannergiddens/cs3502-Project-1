#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000.0

typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;
} Account;

static Account accounts[NUM_ACCOUNTS];

static volatile int progress_counter = 0;

// INTENTIONAL DEADLOCK TRANSFER:

static void transfer_deadlock(int from_id, int to_id, double amount) {
    pthread_mutex_lock(&accounts[from_id].lock);
    printf("Thread %lu: locked account %d\n",
           (unsigned long)pthread_self(), from_id);

    // Increases Deadlock likelyhood
    usleep(200000);

    printf("Thread %lu: waiting for account %d\n",
           (unsigned long)pthread_self(), to_id);

    pthread_mutex_lock(&accounts[to_id].lock); // Where deadlock happens

    if (accounts[from_id].balance >= amount) {
        accounts[from_id].balance -= amount;
        accounts[to_id].balance += amount;
    }

    pthread_mutex_unlock(&accounts[to_id].lock);
    pthread_mutex_unlock(&accounts[from_id].lock);

    __sync_fetch_and_add(&progress_counter, 1);
}

typedef struct {
    int from;
    int to;
    double amount;
} TransferArgs;

static void* transfer_thread(void* arg) {
    TransferArgs* a = (TransferArgs*)arg;

    transfer_deadlock(a->from, a->to, a->amount);

    return NULL;
}

int main(void) {
    printf("=== Phase 3: Deadlock Demonstration ===\n\n");

    // Initialize accounts + locks
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].id = i;
        accounts[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    pthread_t t1, t2;

    // Circular wait
    TransferArgs a1 = {0, 1, 50.0};
    TransferArgs a2 = {1, 0, 75.0};

    pthread_create(&t1, NULL, transfer_thread, &a1);
    pthread_create(&t2, NULL, transfer_thread, &a2);


    // Deadlock detection:

    int last_progress = progress_counter;
    int stuck_seconds = 0;

    while (stuck_seconds < 5) {
        sleep(1);

        if (progress_counter == last_progress) {
            stuck_seconds++;
        } else {
            last_progress = progress_counter;
            stuck_seconds = 0;
        }
    }

    printf("\n*** SUSPECTED DEADLOCK: no progress for 5 seconds ***\n");
    printf("Exiting (threads may still be blocked).\n");

    return 0;
}
