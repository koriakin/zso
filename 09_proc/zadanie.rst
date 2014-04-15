.. _09-zadanie:

===============
Małe zadanie #4
===============

Napisać moduł, który utworzy plik ``/proc/text_merge``, działający następująco:

- przy zapisie danych do tego pliku, zapamięta te dane (jako osobny element listy)
- przy odczycie, będzie wypisywał wszystkie zapisane dane (w kolejności zapisu)

Każdy zapis do pliku powinien utworzyć nowy węzeł na liście, a odczyt powinien
korzystać z ``seq_file``.
