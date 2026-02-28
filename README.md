# CS 3502 – Project 1  
## Multi-Threaded Banking System

### Overview
This project demonstrates key multi-threading concepts in C using POSIX threads.

So far, the project includes:

- Phase 1: Race condition demonstration using unsafe shared data access
- Phase 2: Mutex protection using per-account locks and safe transfers
- Phase 3: Intentional deadlock creation and stalled-thread detection
- Phase 4: Deadlock prevention using consistent lock ordering

Future phases will explore deadlock creation and resolution.

---

## Compilation

To compile all phases:

make

To compile individually:

make phase1
make phase2
make phase3
make phase4

---

## Running the Program

### Phase 1 – Race Condition

./phase1

-Demonstrates unsafe concurrent deposits and withdrawals.
-Because no synchronization is used, totals may differ between runs,
 showing a race condition.

---

### Phase 2 – Mutex Protection

./phase2

-Adds mutex locks to protect critical sections.
-Transfers are now thread-safe and the total balance remains constant.
-Execution time is measured to compare performance with other phases.

---

### Phase 3 – Deadlock Demonstration

./phase3

Intentionally creates a deadlock by locking resources in inconsistent order.
Two threads each hold one lock while waiting for the other,
satisfying all four Coffman conditions:

- Mutual exclusion
- Hold and wait
- No preemption
- Circular wait

The program detects stalled progress and reports a suspected deadlock.

---

### Phase 4 – Deadlock Resolution

./phase4

Prevents deadlock by enforcing consistent lock ordering.
All threads acquire locks in the same order, eliminating circular wait.
The program completes successfully while preserving the total balance.
Execution time is measured to compare performance with other phases.

---

## Concepts Demonstrated

- Thread creation and joining
- Shared memory concurrency
- Race conditions
- Critical sections
- Mutex-based mutual exclusion
- Deadlock creation
- Deadlock detection concepts
- Deadlock prevention via lock ordering 

---

## Author
Tanner Giddens
CS 3502 – Operating Systems
