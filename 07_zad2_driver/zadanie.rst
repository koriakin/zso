.. _07-zadanie:

=======================================
Zadanie 2: sterownik urządzenia hashdev
=======================================

Data ogłoszenia: 01.04.2014

Termin oddania: 14.05.2014 08:00

.. toctree::
   :hidden:

   hashdev
   qemu


Materiały dodatkowe
===================

- :ref:`07-hashdev`
- :ref:`07-qemu`
- :download:`hashdev_ioctl.h`
- :download:`hashdev.h`
- :download:`qemu-1.7.1.tar.bz2`
- :download:`qemu-hashdev.patch`

TODO: testy


Wprowadzenie
============

Zadanie polega na napisaniu sterownika do :ref:`urządzenia
hashdev <07-hashdev>`, przyspieszającego operację SHA-1 na dużych blokach
danych. Urządzenie dostarczane jest w postaci :ref:`zmodyfikowanej wersji
qemu <07-qemu>`.

Urządzenie powinno być dostępne dla użytkownika w formie urządzenia
znakowego. Dla każdego urządzenia hashdev obecnego w systemie należy
utworzyć urządzenie znakowe /dev/hashX, gdzie X to numer kolejny
urządzenia hashdev, zaczynając od 0.

Interfejs urządzenia znakowego
==============================

Urządzenia hash* powinny obsługiwać następujące operacje:

- open: każde wywołanie open powinno stworzyć niezależny kontekst
  urządzenia składający się z bieżącego stanu operacji SHA-1.
  Operacje na różnych kontekstach nie powinny wpływać na siebie,
  pozwalając wielu użytkownikom na jednoczesne korzystanie z urządzenia.

- close: w oczywisty sposób

- write: przesyła blok danych do operacji SHA-1. Sterownik nie powinien
  wykonywać częściowych zapisów (zapisywać mniej niż podana ilość bajtów),
  chyba, że proces dostał sygnał w trakcie zapisu - w razie zapełnienia buforów,
  należy czekać aż zwolni się w nich jakieś miejsce (i tak być może wiele razy
  w przypadku otrzymania długiego bufora od użytkownika). Dane do pojedynczego
  skrótu mogą być przesłane przez użytkownika w trakcie wielu operacji write
  - należy je traktować jako jeden strumień danych, aż do wykonania operacji
  odczytu sumy. W szczególności, dane od użytkownika niekoniecznie muszą być
  równo podzielone na bloki - należy o to zadbać samemu.

- ioctl(HASHDEV_IOCTL_GET_RESULT): oznacza, że wszystkie dane do skrótu zostały
  już zapisane, powoduje uzupełnienie danych zgodnie ze specyfikacją SHA-1,
  liczy ostateczną sumę i zwraca jej wartość. Następnie resetuje kontekst
  na stan początkowy, umożliwiając obliczenie nastepnego skrótu.

Sterownik powinien rejestrować swoje urządzenia w sysfs, aby udev
automatycznie utworzył pliki urzadzeń o odpowiednich nazwach w /dev. Numery
major i minor dla tych urządzeń są dowolne (majory powinny być alokowane
dynamicznie).

Definicje ioctli są zawarte w :download:`hashdev_ioctl.h`

Sterownik może przyjąć ograniczenie do 256 urządzeń w systemie.


Założenia interakcji ze sprzętem
================================

Operacja kroku SHA-1 powinna być w całości zrealizowana sprzętowo (tzn.
niedozwolone jest ręczne wykonywanie operacji kroku SHA-1 na procesorze.

Można założyć, że przed załadowaniem sterownika rejestry ``ENABLE`` oraz
``INTR_ENABLE`` są ustawione na 0 (i tylko tyle). Urządzenie należy też
w takim stanie zostawić przy odładowaniu sterownika.

.. prawda?
   Implementując sterownik można zignorować funkcje suspend itp. - zakładamy,
   że komputer nie będzie usypiany (jest to bardzo zła praktyka, ale niestety
   nie mamy możliwości sensownego przetestowania suspendu w qemu ze względu na
   bugi w virtio).

Zasady oceniania
================

Za zadanie można uzyskać do 10 punktów. Na ocenę zadania składają się trzy
części:

- pełne wykorzystanie bloków urzadzenia (od 0 do 5 punktów):

  - tylko blok operacji SHA-1 (ręczne użycie rejestru SHA1_DATA): 0p
  - użycie bloku wczytywania danych: 3p
  - użycie bloków wczytywania danych i poleceń (zapewniając, że
	urządzenie może być bez przerwy aktywne, jeśli jest cały czas
	karmione danymi): 5p

- wynik automatycznych testów (od 0 do 5 punktów)
- ocena kodu rozwiązania (od 0 do -10 punktów)


Forma rozwiązania
=================

Sterownik powinien zostać zrealizowany jako moduł jądra Linux w wersji
3.13.3.  Moduł zawierający sterownik powinien nazywać się hashdev.ko.
Jako rozwiązanie należy dostarczyć paczkę zawierającą:

- źródła modułu
- plik/pliki Makefile i Kbuild pozwalające na zbudowanie modułu
- krótki opis rozwiązania

Pytania i odpowiedzi
====================

Pytania należy wysyłać na adres m.koscielnicki@mimuw.edu.pl, odpowiedzi
(wiążące) będą zamieszczane tutaj.
