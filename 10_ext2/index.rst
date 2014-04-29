==============================
Zajęcia 10: system plików ext2
==============================

Data: 29.04.2014, 30.04.2014

.. toctree::
   :hidden:

   zadanie


Materiały dodatkowe
===================

- :ref:`10-zadanie`


EXT2
====

Wyczerpujący, szczegółowy opis wszystkich struktur danych znajduje się tu:

- http://e2fsprogs.sourceforge.net/ext2intro.html
- http://uranus.chrysocome.net/explore2fs/es2fs.htm

Narzędzia do obsługi systemu plików ext2:

``mke2fs``
    Utworzenie nowego systemu plików.
``tune2fs``
    Zmiana parametrów systemu plików.
``debugfs``
    Niskopoziomowy dostęp do systemu plików, pozwalający na eksplorowanie
    i zmianę wewnętrznych struktur danych.
``filefrag``
    Narzędzie pozwalające na zobaczenie w ilu ciągłych fragmentach na dysku
    znajduje się plik. Działa nie tylko na ext2.
