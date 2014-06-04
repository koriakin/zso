.. _15-zadanie:

===================================
Zadanie 4: transakcyjna baza danych
===================================

Data ogłoszenia: 03.06.2014

Termin oddania: 09.09.2014 08:00


Wprowadzenie
============

Klasyczne bazy danych (model ACID) oparte są o transakcje.  Transakcja to
seria odczytów i zapisów do bazy danych działająca następująco:

- Transakcja kończy się, gdy zostanie zatwierdzona lub porzucona przez
  użytkownika.  Zmiany dokonane w zatwierdzonej transakcji permanentnie stają
  się częścią bazy danych, a zmiany dokonane w porzuconej transkacji giną
  bezpowrotnie.
- Transakcje widzą wyniki wcześniejszych zatwierdzonych transakcji, ale nie
  widzą zmian z niezatwierdzonych transakcji.
- "Stan świata" widziany przez transakcję nie zmienia się w trakcie jej trwania.

Czasem w bazach danych używa się mniej ścisłej izolacji między transakcjami,
ale tutaj przyjmujemy powyższy model (serializowalny).

Podstawowym problemem w implementacji transakcji jest mechanizm blokowania
bazy danych.  Bardzo proste bazy danych (``sqlite3``) po prostu używają
blokady typu czytelnicy/pisarze na całej bazie danych, ale jest to wysoce
nieefektywne, gdy mamy wielu użytkowników.  Bardziej zaawansowane systemy
blokowania (np. MVCC) blokują tylko część bazy danych.  Pozwala to na
zwiększenie wydajności, ale niestety czasami powoduje konieczność przerwania
transakcji, gdy okaże się, że dwa programy odwołały się jednocześnie do
tego samego obszaru danych w konfliktujący sposób (np. program A czyta x,
program B czyta x, program A pisze x, program B pisze x).

Systemy baz danych zazwyczaj implementowane są w przestrzeni użytkownika.
W tym zadaniu spróbujemy zaimplementować mechanizm transakcji w przestrzeni
jądra.


Plik /dev/db
============

Nasza baza danych będzie widziana jako urzadzenie znakowe ``/dev/db``.  Dla
uproszczenia, uznamy że dane w bazie mają bardzo prostą strukturę: jest
to po prostu wielka tablica bajtów.  Aby nie przejmować się kwestią zmiany
rozmiaru bazy, przyjmujemy że baza danych ma rozmiar nieskończony
(a przynajmniej taki, jaki mieści się w dodatniej części zakresu off_t),
a każdy bajt, jaki jeszcze nie został zapisany, ma wartość 0.




Operacje na /dev/db
===================

Plik ``/dev/db`` powinien wspierać następujące operacje:

- ``read``/``pread``: odczytuje dane w ramach transakcji.  Jeśli nie ma
  aktywnej transakcji, rozpoczyna nową.
- ``write``/``pwrite``: zapisuje dane w ramach transakcji.  Jeśli nie ma
  aktywnej transakcji, rozpoczyna nową.  Może rzucić ``EDEADLK``, jeśli
  wiadomo już, że transakcja nie ma szans się udać.  W takim wypadku następuje
  jednocześnie porzucenie transakcji.
- ``lseek``: jak zwykle
- ``ioctl(DB_COMMIT)``: zatwierdza transakcję.  Może rzucić ``EDEADLK``
  (i porzucić transakcję), jeżeli wystąpił konflikt, w przyciwnym wypadku
  zwraca ``0``.
- ``ioctl(DB_ROLLBACK)``: porzuca transakcję, zwraca 0.  Zawsze się udaje.

Każde otwarcie pliku ``/dev/db`` tworzy niezależny "kontekst transakcji"
(aktywna transakcja jest związana z pojedynczym otwarciem pliku).

Zamknięcie pliku, gdy jest na nim aktywna transakcja, powoduje przerwanie
transakcji.


Zasady oceniania
================

Za zadanie można uzyskać do 10 punktów. Na ocenę zadania składają się trzy
części:

- jakość użytego mechanizmu blokad (od 0 do 5 punktów)
  - pełna serializacja transakcji (w stylu sqlite3): 0p
  - pełna serializacja zapisów, ale równoległy odczyt z werjonowaniem: 2p
  - jak wyżej, ze wsparciem niekonfliktujących równoległych zapisów: 4p
  - jak wyżej, ale z blokowaniem potencjalnie konfliktujących zapisów
    zamiast przerywania (w miarę możliwości): 5p
- wynik automatycznych testów (od 0 do 5 punktów)
- ocena kodu rozwiązania (od 0 do -10 punktów)


Forma rozwiązania
=================

Baza danych powinna zostać zrealizowana jako moduł jądra Linux w wersji
3.13.3.  Moduł powinien nazywać się ``transdb.ko``.  Po załadowaniu, moduł
powinien zarejestrować urządzenie znakowe i wpiąć je do ``sysfs``.  Jako
rozwiązanie należy dostarczyć paczkę zawierającą:

- źródła modułu
- plik/pliki Makefile i Kbuild pozwalające na zbudowanie modułu
- krótki opis rozwiązania


Pytania i odpowiedzi
====================

Pytania należy wysyłać na adres m.koscielnicki@mimuw.edu.pl, odpowiedzi
(wiążące) będą zamieszczane tutaj.
