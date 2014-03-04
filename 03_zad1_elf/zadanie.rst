.. _03-zadanie:

=====================
Zadanie 1: biblioteki
=====================

.. contents::

Data ogłoszenia: 04.03.2014

Termin oddania: 02.04.2014, 08:00


Materiały dodatkowe
===================

- :download:`loader.h`


Wprowadzenie
============

"Zwykłe" biblioteki współdzielone w systemie Linux (i innych systemach
bazowanych na formacie ELF) opierają się na mechanizmie PIC (Position
Indpendent Code). Mechanizm ten ma znaczne zalety (możliwość współdzielenia
kodu biblioteki, brak konieczności koordynacji przydziału adresów), lecz
ma też wady (niższa wydajność, większy rozmiar kodu). W tym zadaniu spróbujemy
zaimplementować alternatywne podejście (w bardzo uproszczonym wariancie):
koordynowany przydział adresów.

Przy koordynowanym przydziale adresów, kod kompilowany jest normalnie (tzn.
bez sekwencji PIC), a każda biblioteka ma z góry ustalony adres pod którym
się znajdzie. Adresy wszystkich bibliotek używanych w jednym procesie muszą być
rozłączne, więc musi istnieć mechanizm przydzielania ich przed konsolidacją.
Kod nie jest modyfikowany przez loader, a więc może być współdzielony między
procesami. W prostym wariancie użytym w tym zadaniu, będziemy linkować
wszystkie biblioteki jednocześnie - adresy symboli zewnętrznych wykorzystywane
przez bibliotekę będą więc zakodowane w niej na stałe.

Znacznie bardziej dopracowany wariant tego mechanizmu jest używany w formacie PE
(Portable Executable, format plików wykonywalnych używany przez Windows) oraz
a.out (starożytny format plików wykonywalnych pod Linuxem.


Zadanie
=======

Zaprojektować format binarny (nazwany roboczo CSO) do przechowywania bibliotek
łączonych dynamicznie w modelu koordynowanego przydziału adresów. Napisać linker
konsolidujący zbiór plików ELF typu ``ET_REL`` w zbiór powiązanych plików CSO.
Napisać loader, będący "zwykłą" biblioteką współdzieloną dla systemu Linux,
który ładuje pliki CSO do pamięci i wyszukuje w nich symbole.


Format pliku CSO
================

Dokładny format CSO jest dowolny. Powinien on jednak zawierać co najmniej:

- Listę symboli eksportowanych przez bibliotekę (nazwy i adresy)
- Listę wymaganych bibliotek
- Jakąś w miarę unikalną sygnaturę formatu
- Jakiś w miarę sensowny mechanizm weryfikacji, że biblioteka jest tą szukaną
  (czyli nikt nie podmienił jej na inną) i pochodzi z tej samej konsolidacji,
  co reszta
- Kod i dane biblioteki, w formie gotowej do zmapowania w przestrzeń adresową

Pliki CSO powinny mieć rozszerzenie ``.cso``.


Interfejs linkera
=================

Linker powinien być programem, który dostaję listę plików ``ET_REL`` jako
parametry linii poleceń i konsoliduje je w pliki CSO (jeden plik wejściowy
odpowieda jednemu plikowi CSO na wyjściu). Wynikowy plik powinien mieć nazwę
taką jak odpowiadający plik wejściowy, lecz z rozszerzeniem zamienionym na
``.cso``. W przypadku sukcesu, linker nie powinien wypisywać nic na
standardowe wyjście i zakończyć się kodem wyjścia 0. W przypadku błędu,
linker powinien wypisać sensowny komunikat na standardowe wyjście błędów
i zakończyć się kodem wyjścia innym niż 0.

Format plików wejściowych do linkera
------------------------------------

Wejściem linkera są pliki ELF typu ``ET_REL`` na platformę i386. Przyjmujemy
następujące założenia:

- jedyne flagi sekcji, które trzeba obsłużyć to ``SHF_ALLOC``, ``SHF_WRITE``,
  ``SHF_EXECINSTR``. Pozostałe flagi można ignorować. Sekcje bez ``SHF_ALLOC``
  nie powinna znaleźć się w pliku CSO. ``SHF_WRITE`` i ``SHF_EXECINSTR``
  powinny być obsługiwane zgodnie ze specyfikacją: sekcja bez ``SHF_WRITE``
  ma nie mieć praw do zapisu po załadowaniu (analogicznie dla
  ``SHF_EXECINSTR``, na tyle na ile system na to pozwala).
- typy sekcji, które trzeba obsłużyć to: ``SHT_PROGBITS``, ``SHT_SYMTAB``,
  ``SHT_STRTAB``, ``SHT_REL``, ``SHT_NOBITS``. Sekcje pozostałych typów należy
  traktować tak, jakby były to sekcje ``SHT_PROGBITS``. Ponadto, można
  założyć, że będzie istniała dokładnie jedna sekcja ``SHT_SYMTAB.``
- typy symboli, które trzeba obsłużyć to: ``STT_NOTYPE``, ``STT_OBJECT``,
  ``STT_FUNC``, ``STT_SECTION`` (``STT_SECTION`` tylko do relokacji, nie do
  ``library_getsym``).  Pozostałe symbole można ignorować.
- typy wiązania symboli, jakie należy obsłużyć to ``STB_LOCAL``
  i ``STB_GLOBAL``.  Jeśli pojawią się inne, należy je traktować jako
  ``STB_GLOBAL``. Symbole ``STB_LOCAL`` nie powinny znaleźć się w pliku
  CSO, ani być linkowane między bibliotekami.
- nie przejmujemy się widocznością symboli (``STV_*``)
- zakładamy, że symbol jest albo zdefiniowany w prawdziwej sekcji,
  albo niezdefiniowany (``SHN_UNDEF``) - symbole w pozostałych specjalnych
  sekcjach (``SHN_*``) należy zignorować
- jedyne typy relokacji, które trzeba obsłużyć to ``R_386_32``
  i ``R_386_PC32``. Jeśli plik używa innego typu relokacji, lub gdy symbol
  używany przez relokację nie jest obsługiwany (np. ma dziwny typ lub specjalną
  sekcję), linker może zwrócić błąd.
- jeśli jakiś symbol jest wymagany przez którąś bibliotekę, ale nie jest
  w żadnej zdefiniowany, linker powinien zwrócić błąd.


Interfejs loadera
=================

Interfejs loadera jest ustalony i zadeklarowany w pliku nagłówkowym
:download:`loader.h`. Składają się na niego następujące funkcje:

``int library_load(const char *name);``
    Ładuje podaną bibliotekę do pamięci, wraz z wszystkimi bibliotekami
    przez nią wymaganymi. Zwraca 0 w razie sukcesu, -1 w razie błędu
    (ustawiając ``errno`` na sensowną wartość). Podany ciąg znaków jest
    nazwą pliku.

``void *library_getsym(const char *name);``
    Zwraca adres zadanego symbolu, szukając go w załadowanych obecnie
    bibliotekach. W przypadku braku takiego symbolu, zwraca ``NULL``.

Dla uproszczenia, loader może założyć, że obecny katalog (cwd) jest taki
sam, jak podczas linkowania.

Interfejs nie obsługuje usuwania załadowanych bibliotek - zakładamy, że
pozostają w pamięci do końca działania procesu.


Forma rozwiązania
=================

Jako rozwiązanie należy dostarczyć paczkę zawierającą:

- niezmodyfikowany plik :download:`loader.h`
- dowolną ilość plików źródłowych z kodem rozwiązania
- plik Makefile kompilujący rozwiązanie, lub odpowiadający plik z innego
  sensownego systemu budowania (np. cmake)
- plik readme z krótkim opisem rozwiązania i opisem formatu CSO

Kod loadera powinien być napisany w całości w C (ew. C++). Linker może być
napisany w dowolnym języku, którego interpreter/komplator znajduje się
w repozytorium paczek sensownej dystrybucji. Rozwiązanie może używać
tylko standardowej biblioteki dla odpowiednich języków programowania.
Loader powinien kompilować się do biblioteki ``libloader.so`` i nie eksportować
na zewnątrz żadnych symboli poza wymienionymi w interfejsie. Linker powinien
znajdować się (po ew. kompilacji) w pliku wykonywalnym o nazwie ``linker``

Wskazówki
=========

- Zakres adresów wirtualnych, w którym można w miarę bezpiecznie umieścić
  zlinkowane biblioteki, to np. ``0x10000000..0x6fffffff``.


Zasady oceniania
================

Za zadanie można uzyskać do 10 punktów. Na ocenę zadania składają się dwie
części:

- wynik automatycznych testów (od 0 do 10 punktów)
- ocena kodu rozwiązania (od 0 do -10 punktów)


Testy
=====

Testy użyte do oceny zadania będą opublikowane do 11.03.2014.


Pytania i odpowiedzi
====================

Pytania należy wysyłać na adres m.koscielnicki@mimuw.edu.pl, odpowiedzi
(wiążące) będą zamieszczane tutaj.
