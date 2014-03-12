.. _04-p2-export:

=================================
Przykład 2: Eksportowanie symboli
=================================

Dwa moduły, które eksportują funkcje o tej samej nazwie oraz jeden, który 
chce z tej funkcji skorzystać. Różna kolejność ładowania i usuwania modułów 
pozwala prześledzić sposób, w jaki symbole dołączane są dynamicznie do 
jądra.


Kod
---

- paczka: :download:`../p2_export.tar`
- źródło: :download:`base1.c`, :download:`base2.c`, :download:`call_base.c`
- Makefile/KBUILD: :download:`Makefile`

Działanie
---------

- ``base1.o`` - eksportuje funkcję ``base_fun()``
- ``base2.o`` - eksportuje funkcję ``base_fun()``
- ``call_base.o`` - przy inicjalizacji woła funkcję ``base_fun()``

Obie funkcje ``base_fun()`` wypisują za pomocą ``printk()`` do logu
systemowego (lub na konsolę, zależnie od ustawienia sysloga) napis, który
pozwala zidentyfikować, z którego modułu pochodzą.

Testy
-----

Należy zbadać różne warianty załadowania modułów (za pomocą ``insmod``),
sprawdzając za każdym razem (za pomocą ``lsmod`` i sprawdzania logu), co się
dzieje.  Usuwanie pojedynczych modułów realizuje się poleceniem ``rmmod``.
Log można zobaczyć za pomocą polecenia ``dmesg``.  W każdym przypadku
zakładamy, że na początku żaden z modułów nie jest załadowany.

1. Próba załadowania ``call_base`` bez załadowania ``base1`` lub ``base2``.

   Symbol ``base_fun`` nie jest zdefiniowany

2. Załadowanie ``base1``, potem ``call_base``.

   Wywolana zostanie funkcja ``base_fun`` z ``base1``.

3. Załadowanie ``base1`` a potem ``base2``.

   Nie uda się załadowanie ``base2``, ponieważ zawiera ten sam symbol
   co ``base1``.  Od wersji 2.6.16 jądro sprawdza, czy ładowany moduł nie
   zawiera symboli już obecnych w jądrze i w takim wypadku odmawia
   załadowania modułu.  Wcześniejsze jądra pozwalały załadowac moduł, a symbol
   z nowego modułu przykrywał poprzednią definicję symbolu.

4. Załadowanie ``base2``, ``call_base``.

   Wywołana zostanie funkcja ``base_fun`` z ``base1``.

   1. Następnie próba usunięcia ``base2``.

      Nie powiedzie się, bo ``call_base`` używa ``base2``.

   2. Usunięcie ``call_base``, a potem ``base1``.

      Powiedzie się, bo moduł ``base1`` staje się nieużywany, gdy moduł
      ``call_base`` zostanie odładowany.

Użycie modprobe
---------------

Należy wykonać::

    make install # jako root lub za pomocą sudo
    modprobe call_base

Moduł ``call_base`` powinien załadować moduł ``base1`` automatycznie.
Sprawdzenie: ``lsmod``

Nastlpnie należy wykonać polecenie::

    modprobe -r call_base

Moduł ``call_base`` i ``base1`` zostaną odładowane - ``modprobe -r`` usuwa
moduł, a potem jeśli moduł używany przez usuwany moduł staje się nieużywany,
to usuwa rownież jego.  Przydaje się to do usuwania "stosów" modułów (np.
modułów ALSA).
