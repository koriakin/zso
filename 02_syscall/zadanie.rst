.. _02-zadanie:

===============
Małe zadanie #2
===============

Napisać implementację mutexów w przestrzeni użytkownika korzystając z futexów
oraz operacji compare-and-swap. Implementacja nie powinna używać aktywnego
oczekiwania. Operacje nie wymagające czekania na mutexie powinny odbywać się
bez udziału jądra. Implementacja powinna mieć interfejs taki, jak w pliku
:download:`mutex.h`. Do testów można posłużyć się plikiem :download:`test_mutex.c`.

Podpowiedzi:

- compare-and-swap można wywołać w gcc następująco::

    int __sync_val_compare_and_swap(int *val, int oldval, int newval);

  odpowiada to operacji::

    atomic {
        int prev = *val;
        if (prev == oldval)
            *val = newval;
        return prev;
    }

- w mutuxie wystarczy mieć jedno pole typu int, przyjmujące 3 różne wartości
- użyć funkcji syscall() do wywołania futexów

Algorytm z zajęć::

    #define UNLOCKED 0
    #define LOCKED   1
    #define WAITING  2

    lock() {
        if (CAS(mutex, UNLOCKED, LOCKED) != UNLOCKED) {
            while (CAS(mutex, UNLOCKED, WAITING) != UNLOCKED) {
                CAS(mutex, LOCKED, WAITING);
                futex_wait(mutex, WAITING);
            }
        }
    }

    unlock() {
        if (CAS(mutex, LOCKED, UNLOCKED) == WAITING) {
            CAS(mutex, WAITING, UNLOCKED);
            futex_wake(mutex, 1);
        }
    }
