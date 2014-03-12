.. _04-p3-msg:

=============================
Przykład 3: Parametry modułów
=============================

Prosty moduł sparametryzowany wypisujący pewien napis przy instalacji i przy
usuwaniu z jądra.


Kod
---

- paczka: :download:`../p3_msg.tar`
- źródło: :download:`msg_mod.c`
- Makefile/KBUILD: :download:`Makefile`

Instalacja
----------

::

    insmod ./msg_mod.ko msg="\"My message\"" msgs=aaa,bbb

Przekazywanie parametrów
------------------------

Dzięki mechanizmowi przekazywania parametrów, moduł można konfigurować podczas
inicjalizacji. Jednak sam moduł może mieć zakodowaną domyślną wartość
parametrów. Proszę spróbować załadowac moduł msg_mod.o bez parametrów.

Parametry tablicowe przekazuje się oddzielając je przecinkami::

    insmod ./msg_mod.ko msg="\"My message\"" msgs=aaa,bbb

Wynik dmesg::

    [17229412.952000] Msg[0]: aaa
    [17229412.952000] Msg[1]: bbb

Przekazanie większej liczby parametrów w tablicy niż określone w początkowej
tablicy nie powiedzie się::

    insmod ./msg_mod.ko msg="\"My message\"" msgs=aaa,bbb,ccc

    [17229442.728000] msgs: can only take 2 arguments
    [17229442.728000] msg_mod: `aaa' invalid for parameter `msgs'

