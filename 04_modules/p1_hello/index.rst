.. _04-p1-hello:

========================
Przykład 1: Hello, world
========================

Prosty moduł wypisujący "Hello world" przy instalacji i "Good bye" przy usuwaniu z jadra.

:download:`../p1_hello.tar`

Kompilacja
----------

::

    make

Instalacja 
----------

::

    insmod ./hello_mod.ko

Usunięcie
---------

::

    rmmod hello_mod

Uwaga
-----

Komunikaty są zapisywane do logu systemowego (zazwyczaj ``/var/log/messages``
- proszę spróbować komendę ``dmesg``) lub wyświetlane na konsoli (w zależności
od konfiguracji syslog).
