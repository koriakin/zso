.. _05-p1-yatb:

============================
Przykład 1: You are the best
============================

Moduł rejestrujący proste urządzenie znakowe umożliwiające odczyt
napisu ``You are the best!`` z pliku ``/dev/yatb``; zapis do pliku ``/dev/yatb``
pozwala ustalić ile razy napis zostanie powtórzony na wyjściu (liczba
zapisanych znaków oznacza liczbę powtórzeń przy odczycie). Dodatkowo
rejestrowane jest urządzenie ``/dev/yatb_once``, które zwraca zawsze
pojedynczy napis.


Kod
---

- paczka: :download:`../p1_yatb.tar`
- źródło: :download:`drv_yatb.c`
- Makefile: :download:`Makefile`
- KBUILD: :download:`Kbuild`


Instalacja
----------

Należy utworzyć pliki specjalne.

Do odczytu wielokrotnego (numer drugorzędny 0)::

    mknod /dev/yatb c 42 0  

Do odczytu jednokrotnego (numer drugorzędny 1)::

    mknod /dev/yatb_once c 42 1 

Załadować moduł::

    insmod ./drv_yatb.ko


Testy
-----
Sterownik jest widoczny w ``/proc/devices``.

Po załadowaniu modułu odczyt zarówno z ``/dev/yatb``, jak i ``/dev/yatb_once`` powinien 
zwracać napis ``You are the best!``::

    $ cat /dev/yatb
    You are the best!
    $ cat /dev/yatb_once
    You are the best!

Zapisanie do ``/dev/yatb`` pewnej liczby znaków zwiększa licznik powtórzeń dla ``yatb``, 
ale zachowanie ``yatb_once`` nie zmienia się::

    $ echo "abc" > /dev/yatb
    $ cat /dev/yatb
    You are the best!
    You are the best!
    You are the best!
    You are the best!
    $ cat /dev/yatb_once
    You are the best!
