.. _11-zadanie:

============================
Zadanie 3: modyfikacja jądra
============================

Data ogłoszenia: 06.05.2014

Termin oddania: 30.06.2014 08:00


Materiały dodatkowe
===================

- :download:`filecrypt.h`


Wprowadzenie
============

Jest wiele systemów szyfrowania danych - niestety, dostępne rozwiązania
dzielą się na dwa rodzaje:

- szyfrowanie całego systemu plików przez szyfrowanie leżącego pod spodem
  urządzenia blokowego (np. truecrypt)
- nieprzezroczyste szyfrowanie pojedynczych plików - jednostką szyfrowania
  jest pojedynczy plik, ale programy próbujące się dostać do zaszyfrowanych
  muszą ręcznie je odszyfrować (np. gpg)

W tym zadaniu skonstruujemy system transparentnego szyfrowania pojedynczych
plików przez rozszerzenie funkcjonalności systemu plików ext4.


System szyfrowania
==================

Pliki szyfrowane będą algorytmem AES z kluczem 128-bitowym w trybie CTR.
Klucz szyfrowania wybierany jest przez użytkownika. Aby rozpoznać klucz
użyty do zaszyfrowania pliku, używany jest jego identyfikator. Identyfikatorem
klucza jest po prostu jego suma MD5.

Każdy zaszyfrowany plik ma dwa atrybuty (do ich przechowywania można używać
np. atrybutów rozszerzonych z systemowej przestrzeni nazw): identyfikator
klucza oraz wektor początkowy (IV). Każdy blok tego pliku przechowywany jest
na dysku w formie zaszyfrowanej przez AES128-CTR z wektorem początkowym
równym ``(IV_pliku ^ (indeks_bloku * rozmiar_bloku))``.

Kryptosystem ten ma oczywiście poważne dziury, ale jest to zadanie z systemów
operacyjnych, a nie z kryptografii.

Zanim użytkownik będzie mógł otworzyć (lub utworzyć) zaszyfrowany plik, musi
podać użyty klucz. Aby szyfrowanie było transparentne (tzn. otwarcie pliku
było możliwe przez zwykły ``open``), z każdym procesem będzie stowarzyszona
lista posiadanych kluczy. Przy otwarciu pliku, potrzebny klucz będzie
wyszukiwany na tej liście (jeśli nie zostanie znaleziony, otwarcie się nie
uda). Procesy mogą dodawać klucze do swojej listy przez nowo dodany syscall
``addkey``, a lista będzie dziedziczona przez procesy potomne. Tym samym,
wystarczy dodać klucze podczas procesu logowania, a będą one dostępne dla
wszystkich procesów otwartych w sesji (można by sobie wyobrazić bardziej
elastyczne podejście, ale to tylko zadanie z ZSO).

Aby utworzyć nowy szyfrowany plik, należy użyć nowo dodanego ioctla
``EXT4_ENCRYPT`` na *pustym* pliku (tzn. bez zaalokowanych bloków), podając
identyfikator klucza, którym powinien zostać zaszyfrowany (jeżeli klucz nie
jest dostępny dla danego procesu, powinien zostać zwrócony błąd).


Ustalenia techniczne
====================

Syscall ``addkey`` powinien mieć następującą sygnaturę::

    int addkey(unsigned char *key);

Parametr jest wskaźnikiem na 16-bajtowy klucz. Wynikiem syscalla jest ``0``
w przypadku sukcesu.

Ioctl ``EXT4_ENCRYPT`` musi być wywoływany na pustym (zerowy rozmiar, zero
zaalokowanych bloków) pliku w systemie plików ext4. Jego jedynym parametrem
jest identyfikator klucza. W przypadku, gdy plik nie jest pusty, należy
zwrócić błąd EINVAL.

W przypadku, gdy klucz jest nieznany w ``EXT4_ENCRYPT`` lub ``open``, należy
zwrócić błąd EPERM.

Format zapisu identyfikatora klucza i IV na dysku nie jest ustalony, ale
powinien być zgodny wstecznie (tzn. dopóki nie mamy na systemie plików
zaszyfrowanych plików, nie powinien się niczym różnić od zwykłego).


Forma rozwiązania
=================

Jako rozwiązanie należy wysłać patcha na jądro w wersji 3.13.3.
Patch powinien być w jednym z następujących formatów:

- patch wygenerowaniy przez diffa z opcjami ``-uprN`` nakładający się przez
  ``patch -p1``
- ``git format-patch``

Do patcha powinien być dołączony krótki opis rozwiązania.


Pytania i odpowiedzi
====================

Pytania należy wysyłać na adres m.koscielnicki@mimuw.edu.pl, odpowiedzi
(wiążące) będą zamieszczane tutaj.
