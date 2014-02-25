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
