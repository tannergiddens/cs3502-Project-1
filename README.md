# CS 3502 – Project 1  
## Multi-Threaded Banking System

### Overview
This project demonstrates key multi-threading concepts in C using POSIX threads.

So far, the project includes:

- Phase 1: Race condition demonstration using unsafe shared data access
- Phase 2: Mutex protection using per-account locks and safe transfers

Future phases will explore deadlock creation and resolution.

---

## Compilation

To compile all phases:

make

To compile individually:

make phase1
make phase2

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

---

## Concepts Demonstrated (So Far)

- Thread creation and joining
- Shared memory concurrency
- Race conditions
- Critical sections
- Mutex-based mutual exclusion

---

## Author
Tanner Giddens
CS 3502 – Operating Systems
