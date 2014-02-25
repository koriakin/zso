==============
Zajęcia 3: ELF
==============

Data: 04.03.2014, 05.03.2014

.. warning::

   Materiały na te zajęcia mogą się jeszcze zmienić.

.. contents::

.. toctree::
   :hidden:

   zadanie


Materiały dodatkowe
===================

- :ref:`03-zadanie`


O formacie ELF
==============

ELF jest formatem plików używanym w systemie Linux (i wielu innych
systemach) dla programów, bibliotek dzielonych (.so), plików pośrednich
kompilacji (.o), oraz plików zrzutu pamięci (core).

Choć podstawowe cechy formatu ELF są zawsze takie same, jest wiele
elementów zależnych od architektury procesora, a czasem i systemu
operacyjnego. Tutaj będziemy się zajmować jedynie formatem ELF
na architekturze x86 w systemie Linux.

Niestety nie istnieje żadna pełna specyfikacja ELFa. "Bazowy" format ELF
jest opisany w dokumentacji System V generic ABI (gABI), zaś części zależne
od architektury powinny być opisane w odpowiednim processor-specific ABI
(psABI). W praktyce jednak psABI dla wielu architektur bywają cieżkie do
zdobycia, bardzo niekompletne, lub w ogóle nie zostały napisane. Sprawę
dalej komplikują rozszerzenia ELFa: ELF, jako otwarty i elastyczny format,
pozwala systemom operacyjnym na definiowanie własnych typów sekcji,
relokacji, symboli itp. Wiele z nich nie jest nigdzie opisane.

Prawdopodobnie najbardziej kompletnym pojedynczym dokumentem opisującym
ELFa jest Linker and Libraries Guide Sun'a. Jest to w zasadzie dokumentacja
linkera i bibliotek dynamicznych na systemie Solaris, ale zawiera pełny
opis formatu ELF dla architektur i386, x86_64, sparc, sparc64, a także
kilku rozszerzeń używanych również na systemach Linuxowych (wersjonowanie,
TLS).

Innym przydatnym zasobem jest plik nagłówkowy elf.h z biblioteki glibc
(``/usr/include/elf.h``). Zawiera on stałe i struktury ELFa dla architektur
i systemów operacyjnych wspieranych przez glibc. Pliki nagłówkowe
ze wsparciem dla jeszcze większej ilości architektur można znaleźć
w bibliotece libbfd, będącej częścią binutils.

W tym pliku zostaną przedstawiony jedynie szkic formatu ELF - po
szczegółowe informacje odsyłam do Linker and Libraries Guide.


ELF - podstawowa struktura
==========================

Pliki ELF, na pierwszym poziomie, są złożone z 4 obszarów:

- nagłówek ELF (na początku pliku): zawiera informacje o parametrach
  pliku i maszynie, na którą jest przeznaczony, a także informacje
  o położeniu nagłówków sekcji i programu

- nagłówki sekcji: każdy nagłówek opisuje typ i położenie jednej sekcji.
  Sekcja to spójny blok pamięci o jednolitych atrybutach. Większość
  sekcji opisuje po prostu obszar pamięci, który powinien zostać
  stworzony przy uruchamianiu programu i zainicjowany danymi z pliku,
  ale istnieje wiele specjalnych typów sekcji o bardziej skomplikowanej
  semantyce.

- nagłówki programu: każdy nagłówek opisuje typ i położenie jednego
  segmentu. Segment to spójny blok pamięci o jednolitym przeznaczeniu
  i atrybutach z punktu widzenia procesu ładowania i uruchamiania
  programu. Jeśli plik ma zarówno segmenty, jak i sekcje, segmenty
  mają relację jeden-do-wielu z sekcjami (bo może być wiele sekcji,
  które linker musi rozróżniać, ale loader już nie)

- zawartość sekcji/segmentów

To, czy nagłówki sekcji/programu mogą lub muszą być obecne zależy od typu
pliku ELF. Istnieją 4 typy plików ELF:

``ET_REL`` (relocatable file)
  Plik skompilowany, ale jeszcze nie zlinkowany (.o). Zazwyczaj powstaje
  w wyniku kompilacji pojedynczego pliku źródłowego. Nie da się go
  bezpośrednio uruchomić - takie pliki są pośrednim etapem kompilacji
  i są łączone przez linker (program ld) w pliki wykonywalne lub biblioteki
  dynamiczne.

  Istnieje również (niezbyt często używana) możliwość połączenia kilku
  plików .o w jeden większy, używając ``ld -r``.

  Jako pliki pośrednie, ``ET_REL`` mogą zawierać niezdefiniowane symbole
  i nieustalone odwołania - zostaną one ustalone przez dalsze etapy
  linkowania.

  W typie ``ET_REL`` nagłówki sekcji są wymagane, zaś nagłówki programu nie
  są używane.

``ET_EXEC`` (executable file)
  Skompilowany i zlinkowany program, powstały zazwyczaj z połączenia plików
  .o przez linker. Taki plik jest gotowy do uruchomienia - wszystkie segmenty
  mają już ustalony adres, pod którym będą dostępne w czasie działania
  programu. Wszystkie odwołania w pliku również są już ustalone - jedynym
  wyjątkiem są specjalne typy odwołań do bibliotek współdzielonych,
  ograniczone do jednego segmentu.  Powoduje to, że prawie cała zawartość
  pamięci ładowana z pliku wykonywalnego jest identyczna we wszystkich
  procesach wykonujących dany program i pozwala na współdzielenie pamięci.

  W typie ``ET_EXEC`` nagłówki programu są wymagane. Nagłówki sekcji nie są
  potrzebne do działania programu, lecz są używane przez debuggery
  i są zazwyczaj załączane.

``ET_DYN`` (shared object file)
  Skompilowana i zlinkowana biblioteka dynamiczna (.so). Bardzo podobne do
  ``ET_EXEC``, ale z następującymi różnicami:

  - choć większość zawartości jest już ustalona (nieustalone odwołania,
    podobnie jak w ``ET_EXEC``, są ograniczone do odwołań zewnętrznych
    w jednym segmencie), nie jest ustalony ostateczny adres, pod którym
    ta zawartość będzie załadowana - biblioteka może być załadowana
    pod dowolne miejsce w pamięci

  - ponieważ kod biblioteki nie może zawierać odwołań do jej adresu,
    używany jest specjalny styl kodu nazywany PIC (Position-Independent
    Code). Za każdym razam, gdy potrzebny jest adres jakiegoś obiektu
    w bibliotece, kod typu PIC musi w jakiś sposób ustalić swoją własną
    pozycję i wyliczyć z niej adres żądanego obiektu. Taki kod jest
    zazwyczaj większy i wolniejszy niż "zwykły" kod.

  - dzięki powyższym cechom, program może ładować wiele bibliotek
    dynamicznych do swojej przestrzeni adresowej, a nawet ładować je
    w trakcie działania

  Należy zauważyć, że choć typ ``ET_DYN`` jest zazwyczaj używany dla
  bibliotek, to nic nie szkodzi na przeszkodzie, aby używać go również
  dla głównego programu - technika ta jest nazywana PIE
  (Position-Independent Executable) i bywa używana ze względu na
  możliwość pełnej randomizacji przestrzeni adresowej procesu.

  Przykładem wykonywalnego pliku ``ET_DYN`` może być biblioteka libc
  (``/lib/libc.so.6``) - przy uruchomieniu wypisuje informacje o wersji.
  Również linker dynamiczny jest zaimplementowany jako wykonywalny
  ``ET_DYN`` (aby nie konfliktować z programem, który ładuje).

``ET_CORE`` (core file)
  Zrzut pamięci procesu, tworzony w przypadku zabicia procesu przez niektóre
  sygnały. Zawiera pełny stan procesu w momencie śmierci, pozwalając
  na otwarcie go w debuggerze i ustalenie przyczyny problemu.

Co ciekawe, moduły jądra Linux (.ko) są typu ``ET_REL``, a są bezpośrednio
ładowane przez jądro - korzyści z typów ``ET_EXEC`` i ``ET_DYN`` (czyli
współdzielenie pamięci) nie stosują się w trybie jądra, zaś ich wady
(ustalona pozycja ``ET_EXEC``, nieefektywność PIC) byłyby dość dotkliwe.

Nagłówek ELF
------------

Nagłówek ELF zawiera następujące informacje:

- identyfikator formatu pliku (``"\x7fELF"``)
- format pliku: little endian czy big endian, 32-bit czy 64-bit - określa
  format pozostałych struktur
- wersję formatu ELF (na razie istnieje tylko 1.0)
- identyfikator systemu operacyjnego (często ignorowany)
- typ pliku ELF (``ET_*``)
- architektura docelowa (``EM_386``, ``EM_X86_64``, ``EM_SPARC``, ...)
- położenie i rozmiar nagłówków sekcji i programu
- adres punktu wejścia programu (dla ``ET_EXEC`` i wykonywalnych ``ET_DYN``)

Sekcje
------

Informacje zawarte w nagłówku sekcji to:

- nazwa sekcji (sekcje mogą mieć dowolne nazwy, ale dla standardowych
  sekcji przyjęło się używać nazw zaczynających się od kropki)
- typ sekcji
- atrybuty sekcji
- rozmiar, położenie w pliku, i wyrównanie sekcji
- dla ``ET_EXEC`` i ``ET_DYN``: ostateczny adres sekcji w pamięci (relatywny do
  bazowego adresu w przypadku ``ET_DYN``)
- identyfikatory stowarzyszonych sekcji (w przypadku niektórych typów)

Typ sekcji determinuje większość jej semantyki. Ważniejsze typy to:

``SHT_PROGBITS``
  zwykła sekcja, zawartość ładowana z pliku
``SHT_NOBITS``
  zwykła sekcja, ale zawartość jest wypełniana zerami zamiast być ładowana z pliku
``SHT_SYMTAB``
  tabela symboli - zawiera informacje o obiektach zawartych w pliku
  i zewnętrznych obiektach, do których ten plik ma odwołania
``SHT_STRTAB``
  tabela ciągów znaków - zawiera nazwy używane przez nagłówki sekcji i wpisy
  w tabeli symboli
``SHT_REL/SHT_RELA``
  zawiera informacje o nieustalonych odwołaniach użytych w danej
  (stowarzyszonej) sekcji
``SHT_DYNAMIC``
  zawiera informacje dla linkera dynamicznego

Ważniejsze atrybuty sekcji to:

``SHF_WRITE``
  sekcja ma prawo do zapisu w czasie wykonania
``SHF_EXECINSTR``
  sekcja zawiera kod wykonywalny
``SHF_ALLOC``
  sekcja będzie ładowana do pamięci w czasie wykonania (sekcje bez tej flagi
  są używane tylko przez narzędzia kompilacji i debugowania)

Standardowe nazwy sekcji używane dla zwykłego kodu w C to:

``.text``
  sekcja kodu
``.rodata``
  sekcja danych tylko do odczytu (``const int x = 3;``)
``.data``
  sekcja danych (``int x = 3;``)
``.bss``
  sekcja danych wyzerowanych (``int x = 0;``)

Segmenty
--------

Informacje zawarte w nagłówku programu to:

- typ segmentu
- atrybuty segmentu
- położenie segmentu w pliku i adres w pamięci
- rozmiar semgentu w pliku i rozmiar segmentu w pamięci (w przypadku różnicy,
  pozostała część jest wypełniana zerami - używane dla sekcji typu ``SHT_NOBITS``)

Ważniejsze typy segmentów to:

``PT_LOAD``
  "zwykły" segment: ładuje obszar do pamięci
``PT_DYNAMIC``
  wskazuje obszar z informacjami dla linkera dynamicznego
``PT_INTERP``
  wskazuje nazwę pliku z linkerem dynamicznym

Jedyne niezależne od procesora/systemu atrybuty segmentu to jego prawa
dostępu (rwx).

W czasie linkowania, segmenty ``PT_LOAD`` są tworzone przez scalenie wszystkich
sekcji z flagą ``SHF_ALLOC`` o zgodnych prawach dostępu.

Symbole i odniesienia
=====================

Jednym z głównych zadań formatu ELF jest przechowywanie informacji
o obiektach zawartych w pliku i o odwołaniach do obiektów zewnętrznych.
Przez obiekt rozumiemy funkcję lub zmienną (globalną). Z punktu widzenia
ELFa, obiekt to po prostu obszar wewnątrz sekcji (``ET_REL``) lub przestrzeni
adresowej programu (``ET_EXEC``, ``ET_DYN``).

Symbole to nazwy przypisane obiektom. Symbol może być zdefiniowany
(przypisany do obiektu w danym pliku) lub niezdefiniowany (stanie się
zdefiniowany w momencie połączenia z plikiem, który go defniuje).

Symbole są przechowywane w tabeli symboli. Informacje o symbolu to:

- nazwa
- wartość: pozycja w sekcji (``ET_REL``) lub pamięci (``ET_EXEC``, ``ET_DYN``)
- zawierająca sekcja
- rozmiar (czyli rozmiar zmiennej lub rozmiar kodu funkcji); może być
  zerowy jeśli interesuje nas tylko adres
- typ:

  ``STT_OBJECT``
    zmienna
  ``STT_FUNC``
    funkcja
  ``STT_SECTION``
    specjalny symbol reprezentujący początek sekcji w odniesieniach

- reguły łączenia:

  ``STB_LOCAL``
    symbol lokalny (static w C)
  ``STB_GLOBAL``
    symbol globalny
  ``STB_WEAK``
    słaby symbol globalny (``__attribute__((weak))`` w gcc)
    - specjalny wariant symbolu globalnego, który automatycznie
    "przegrywa" ze zwykłym symbolem globalnym o tej samej nazwie,
    gdy oba są zdefiniowane

- reguły widoczności - używane do wiązania symboli między modułami
  (moduł to program wykonywalny lub biblioteka dynamiczna):

  ``STV_DEFAULT``
    domyślne reguły - symbol jest widoczny i może zostać
    przysłonięty przez symbol o tej samej nazwie z innego modułu
  ``STV_PROTECTED``
    symbol jest widoczny, ale odniesienia do niego z wnętrza
    zawierającego modułu nie będą przysłonięte
  ``STV_HIDDEN``
    symbol nie jest widoczny z zewnątrz modułu - jak ``STB_LOCAL``,
    ale na poziomie modułu, a nie pliku źródłowego
  ``STV_INTERNAL``
    jak ``STV_HIDDEN``, ale gdy symbol jest funkcją, dodatkowo zakładamy,
    że nigdy nie będzie wywołany z zewnątrz modułu (co byłoby możliwe
    przez przekazanie wskaźnika).  Może być wykorzystana do dodatkowej
    optymalizacji kodu PIC.
    
  Reguły te można ustawić w gcc przez odpowiedni ``__attribute__``.

Symbole mogą być wykorzystywane w kodzie przez odniesienia (zwane
relokacjami). Relokacja jest informacją dla linkera, że w danym
miejscu sekcji, zamiast ustalonych w czasie kompilacji bajtów,
powinien znaleźć się adres symbolu (lub jakaś inna wartość nieznana
w czasie kompilacji). Relokacje są przechowywane w tabelach relokacji
(po jednej na każdą sekcję, która tego wymaga). Informacja
przechowywana dla każdej relokacji to:

- indeks wykorzystywanego symbolu w tablicy symboli
- pozycja relokacji w sekcji
- typ relokacji
- addend: dodatkowy składnik do wartości - dokładna interpretacja
  zależy od typu relokacji, najczęściej jest to po prostu liczba
  dodawana do relokowanej wartości. Może być użyta np. gdy ktoś
  prosi o adres ``a.y``, gdy mamy definicję ``struct { int x, y; } a;``

Istnieją dwa typy tablic relokacji: ``SHT_REL`` i ``SHT_RELA``. W przypadku
``SHT_RELA``, addend jest przechowywany w tablicy relokacji, natomiast
dla ``SHT_REL``, addend jest przechowywany jako początkowa zawartość
relokowanego miejsca. ``SHT_REL`` pozwala na zmniejszenie rozmiaru pliku,
lecz ``SHT_RELA`` jest wymagany dla architektur ze skomplikowanymi typami
relokacji (np. dwuczęściowe relokacje po 16 bitów każda). Architektura
i386 zawsze używa ``SHT_REL``.

Typy relokacji są bardzo zależne od architektury. Większość typów
relokacji służy do łączenia dynamicznego. Podstawowe typy relokacji
na i386 to:

``R_386_32``
  Relokowane jest 32-bitowe pole, wartość relokowana to
  adres symbolu + addend. Tzn na przykład następujący kod::

    extern struct {
        int x;
        int y;
    } a;
    a.y = 13;

  będzie wyglądał tak w assemblerze::

    movl $13, a+4

  co tłumaczy się na kod maszynowy następująco::

    c7 05 XX XX XX XX 0d 00 00 00

  gdzie ``XX XX XX XX`` powinno być zastąpione adresem ``a + 4``. Assembler
  w sekcji pliku ELF zapisze to jako::

    c7 05 04 00 00 00 0d 00 00 00

  A w tabeli relokacji dla tej sekcji wpisze relokację typu ``R_386_32``
  symbolu ``a`` na pozycji 2 (zakładając, że ten kod znajduje się na samym
  początku sekcji).

``R_386_PC32``
  Relokowane jest 32-bitowe pole, wartość relokowana to
  adres symbolu - adres pola + addend. Ten typ relokacji jest używany
  do instrukcji skoków i wywołań (przypominam, że w instrukcjach
  skoku i wywołań na x86 cel jest określany przez różnicę między adresem
  końca instrukcji skoku a adresem celu). Kod::

    extern void f(void);
    v();

  czyli w assemblerze::

    call f

  w kodzie maszynowym zostanie zapisany jako::

    e8 XX XX XX XX .

  gdzie ``XX XX XX XX`` to (adres ``f`` - adres kropki). W sekcji pliku ELF
  będzie to zapisane jako::

    e8 fc ff ff ff

  A w tabeli relokacji będzie relokacja typu ``R_386_PC32`` symbolu ``f``
  na pozycji 1. Proszę zauważyć, że assembler ustawił addend
  relokacji na ``0xfffffffc`` (czyli -4) - jest to poprawka na to,
  że ``R_386_PC32`` jest zdefiniowane jako offset od początku relokowanego
  pola, a instrukcja skoku używa offsetu od końca instrukcji skoku,
  czyli od końca relokowanego pola.

Konwencja wywołań funkcji na architekturze x86 w systemie Linux
===============================================================

(Nie jest to w zasadzie część tematu, ale pewnie się przyda.)

Architektura x86 ma w zasadzie 7 rejestrów ogólnego przeznaczenia: ``%eax``,
``%ecx``, ``%edx``, ``%ebx``, ``%ebp``, ``%esi``, ``%edi``. Oprócz tego,
z programów użytkownika jest jeszcze dostępny wskaźnik stosu ``%esp``
i rejestr znaczników ``%eflags``.

Standardowe konwencje wywołań dla architektury x86 są w skrócie
następujące:

- stos rośnie w dół, %esp wskazuje szczyt stosu, czyli najmniejszy adres
  będący w użyciu przez program. Każdy adres na stosie mniejszy niż ``%esp``
  może zostać w każdej chwili zniszczony (np. przez wywołanie funkcji
  obsługi sygnału).
- w punkcie wejścia do funkcji (czyli zaraz po wywołaniu instrukcji call)
  ``%esp`` = -4 (mod 16), a słowo na szczycie stosu (pod ``%esp``) jest adresem
  powrotnym z funkcji
- funkcja powinna powrócić przez zdjęcie adresu powrotu ze stosu (zwiększając
  ``%esp`` o 4) i skoczenie do niego. Zazwyczaj wykonuje się to instrukcją ``ret``.
- zawartość rejestrów ``%ebx``, ``%ebp``, ``%esi``, ``%edi`` po powrocie z funkcji musi być
  równa ich zawartości z momentu jej wykonania - funkcja musi albo zachować
  i odtworzyć wartość tych rejestrów, albo ich wcale nie używać
- zawartość rejestrów ``%eax``, ``%ecx``, ``%edx``, ``%eflags`` może zostać zmieniona przez
  funkcję bez żadnych konsekwencji
- jeśli funkcja pobiera parametry, zostaną one przekazane na stosie,
  zaczynając od ``%esp+4`` (czyli zaraz po adresie powrotu). Funkcja ma je tam
  pozostawić - zdejmowany ze stosu jest tylko adres powrotu
- jeśli funkcja zwraca wartość, ma ją zostawić w ``%eax``.

Powyższa lista nie uwzględnia przekazywania parametrów i zwracania wartości
innych niż inty/wskaźniki, ani dziwniejszych rejestrów x86. Po więcej
szczegółów odsyłam do psABI-i386.

Biblioteki dynamiczne
=====================

Global Offset Table
-------------------

Jak zostało wcześniej wspomniane, głównym celem projektowym ELFa dla
``ET_EXEC`` i ``ET_DYN`` była możliwość współdzielenia kodu i danych między
procesami. Ponieważ odwołania zewnętrzne (czyli relokacje) w oczywisty
sposób wymagają modyfikacji zawartości pamięci w stosunku do "szablonu"
zawartego w pliku, postanowiono je zabrać w jedno miejsce, ograniczając
liczbę stron pamięci, które nie będą mogły być współdzielone.

To miejsce nazywa się GOT (Global Offset Table). Istnieje jeden GOT dla
każdego modułu (czyli biblioteki lub głównego programu), który go
potrzebuje. Jest to po prostu duża tablica adresów symboli zewnętrznych
wymaganych przez dany moduł. Gdy piszemy bibliotekę dynamiczną (i używamy
PIC), kompilator automatycznie generuje kod ładujący odpowiedni adres
z GOT za każdym razem, gdy potrzebuje adresu obiektu zewnętrznego.
W przypadku plików ``ET_EXEC``, używanych jest kilka trików, aby kompilator
nie musiał jawnie używać GOT, lecz GOT w pewnej formie wciąż jest używany
przy wywołaniach funkcji zewnętrznych.

GOT jest automatycznie tworzony przez linker w momencie linkowania programu
czy biblioteki dynamicznej. Emitowana jest specjalna tablica relokacji
w sekcji ``.rel.dyn``, w którą wpisywane są relokacje wypełniające GOT (jak
i wszystkie inne relokacje wymagana w trakcie dynamicznego łączenia).
Te relokacje są typu ``R_<arch>_GLOB_DAT``, który (w przypadku x86) działa
identycznie jak ``R_386_32``, ale dodatkowo identyfikuje cel relokacji jako
slot GOT.

PIC na x86
----------

Sekwencje kodu niezależnego od pozycji (PIC) są często trikowe, a stopień
trudności zależy od architektury. Architektura x86 jest pod tym względem
dość średnia - są dostępne względne instrukcje skoku, ale brak jest innych
sposobów adresowania pamięci względem wskaźnika instrukcji. Podstawowe
sekwencje kodu używane na architekturze x86 to:

- znalezienie pozycji GOT::

    call _l1
    _l1:
    popl %ebx
    addl $_GLOBAL_OFFSET_TABLE_+(.-_l1), %ebx

  W tej sekwencji instrukcja ``call`` jest używana, aby zapisać adres
  etykiety ``_l1`` (czyli adres "powrotu") na stos. Ten adres jest następnie
  zdejmowany ze stosu, a adres GOT jest otrzymywany przez dodanie różnicy
  adresów między adresem GOT a adresem ``_l1``.

  Kropka w instrukcji ``addl`` (oznaczająca adres obecnej instrukcji) jest
  spowodowana zaszłością historyczną - ``_GLOBAL_OFFSET_TABLE_`` jest
  specjalnym symbolem rozumianym przez assembler jako (adres GOT - adres
  obecnej instrukcji). Użycie tego symbolu powoduje również wyemitowanie
  specjalnej relokacji ``R_386_GOTPC`` (działa jak ``R_386_PC32``, ale zamiast
  adresu symbolu docelowego używa adresu GOT).
   
  Po skończonej sekwencji, adres GOT znajduje się w rejestrze ``%ebx``. Jest
  to standardowy rejestr przeznaczony na adres GOT - według konwencji
  wywołań, musi być ustawiony na adres GOT zawsze, gdy wykonywane jest
  wywołanie przez PLT (patrz niżej).

- Znalezienie adresu zmiennej lokalnej (``static int x;``) (mając adres GOT)::

    leal x@gotoff(%ebx), %ecx 

  Ponieważ większość funkcji i tak musi znaleźć GOT, wykorzystywany jest
  fakt, że zmienne lokalne mają stały offset od GOT - adres zmiennej
  jest znajdowany po prostu przez dodanie tej różnicy do adresu GOT.
  x@gotoff jest specjalną składnią assemblera oznaczającą tą różnicę.
  Odpowiada to relokacji ``R_386_GOTOFF`` (wartość = adres symbolu + addend
  - adres GOT).

- Znalezienie adresu zmiennej zewnętrznej (``extern int x;``) (mając adres GOT)::

    movl x@got(%ebx), %ecx 

  ``x@got`` jest specjalną składnią oznaczjącą (adres adresu ``x`` w got
  - adres got). Ta instrukcja po prostu ładuje zawartość odopowiedniego
  slotu GOT. ``x@got`` odpowiada relokacji ``R_386_GOT32``. Użycie tej relokacji
  automatycznie powoduje stworzenie slotu w GOT dla odpowiedniego symbolu.

PLT
---

Jako optymalizacja w stosunku do powyższych mechanizmów, stworzony został
specjalny mechanizm do wywoływania funkcji zewnętrznych: PLT (Procedure
Linkage Table), pozwalający na leniwe wiązanie funkcji przez linker
dynamiczny.

PLT jest specjalną tabelą, zawierającą (na x86) kod zamiast danych. Każda
zewnętrzna funkcja wywoływana przez PLT ma wpis w PLT. Wpis dla funkcji f
wygląda tak::

    f@plt:
    jmp *f_GOT_PLT_OFF(%ebx)
    f_unbound:
    pushl $f_REL_OFF
    jmp plt0

A ``plt0`` jest pojedynczym specjalnym wpisem wyglądającym tak::

    pushl 4(%ebx)
    jmp *8(%ebx)

Wywołanie funkcji w kodzie PIC wygląda natomiast tak::

    call f@plt

I zakłada, że ``%ebx`` zawiera adres GOT.

Mechanizm działa następująco:

- ``f_GOT_PLT_OFF`` jest offsetem w GOT specjalnego slotu dla danego wpisu PLT
- ten slot działa podobnie jak zwykły slot GOT, ale używa ``R_386_JMP_SLOT``
  zamiast ``R_386_GLOB_DAT``, i jest początkowo ustawiony (przez linker)
  na offset etykiety f_unbound względem bazy biblioteki. Co więcej,
  relokacje ``R_386_JMP_SLOT`` są umieszczane w specjalnej, osobnej tablicy
  relokacji ``.rel.plt``
- dynamiczny linker, widząc ten typ relokacji, wypełni ten slot adresem
  etykiety ``f_unbound`` przez dodanie adresu bazowego biblioteki, zamiast
  od razu szukać symbolu ``f``
- Kiedy program dojdzie do wywołania ``f@plt`` po raz pierwszy, zostanie
  wykonana instrukcja jmp do zawartości slotu, prowadząc do etykiety
  ``f_unbound``
- na stos wrzucany jest offset relokacji ``R_386_JMP_SLOT`` odpowiadającej
  temu slotowi wewnątrz sekcji ``.rel.plt``
- kod plt0 wrzuca zawartość specjalnego slotu GOT o offsecie 4 na stos
  - ten slot jest wcześniej wypełniany przez dynamiczny linker i zawiera
  jakiś uchwyt danego modułu
- sterowanie jest przekazywane specjalnej funkcji ze specjalnego slotu
  GOT o offsecie 8 - ten slot również jest wcześniej wypełniany przez
  dynamiczny linker i zawiera adres specjalnej funkcji wiążącej symbole
- dynamiczny linker, korzystając z dwóch parametrów na stosie, ustala
  o jaki symbol chodzi, i gdzie należy wpisać jego adres, po czym go
  wpisuje i przekazuje sterowanie do funkcji ``f``
- kiedy program następnym razem wywoła ``f@plt``, slot będzie już wypełniony
  i sterowanie od razu pójdzie do funkcji ``f``

ET_EXEC - specjalne triki dla wiązania dynamicznego
---------------------------------------------------

Aby kompilacja głównego programu (plików ET_EXEC) nie wymagała żadnej
znajomości mechanizmów GOT/PLT ze strony kompilatora, używane są dwa
dodatkowe triki:

- jeśli program odwołuje się do zewnętrznego symbolu będącego funkcją,
  automatycznie jest tworzony wpis PLT dla tej funkcji wewnątrz programu
  głównego, a adres tego wpisu PLT staje się "oficjalnym" adresem tej
  funkcji wewnątrz całego procesu (jest to wymagane, aby odniesienia
  do adresu tej funkcji z kodu programu były wiązane w trakcie linkowania,
  a ``&f`` zwracało tą samą wartość w całym programie)
- jeśli program odwołuje się do zewnętrznego symbolu będącego zmienną,
  linker automatycznie tworzy kopię tej zmiennej w segmencie danych
  głównego programu oraz emituje do .rel.dyn specjalną relokację
  ``R_386_COPY``, która spowoduje skopiowanie początkowej zawartości tej
  zmiennej z modułu, który ją oryginalnie definiował. Tak utworzona
  kopia zmiennej staje się "oficjalną" lokalizacją tej zmiennej
  w czasie wykonania, zaś oryginalna zmienna w bibliotece definiującej
  nie jest już dalej używana.

Struktura _DYNAMIC
------------------

Struktura ``_DYNAMIC`` jest tabelą par klucz:wartość stanowiącą informację
o zawartości modułu dla linkera dynamicznego. Zawiera przede wszystkim:

- adres i rozmiar tabeli symboli biorących udział we wiązaniu dynamicznym
- adres i rozmiar tabeli ``.rel.dyn`` i ``.rel.plt``
- adres GOT
- listę bibliotek wymaganych przez ten moduł
- listę ścieżek poszukiwania bibliotek

Linker znajduje strukturę ``_DYNAMIC`` przez nagłówek programu ``PT_DYNAMIC``.

Proces uruchamiania programu i linker dynamiczny
================================================

Uruchamianie programów łączonych statycznie
-------------------------------------------

W przypadku programów połączonych statycznie, cały proces inicjalizacji
programu jest wykonywany przez jądro. Jądro czyta nagłówek ELF, nagłówki
programu, po czym ładuje wszystkie segmenty do pamięci. Następnie tworzy
początkowy stan programu:

- alokowany jest stos głównego wątku
- na stosie głównego wątku są umieszczane:

  - argumenty programu (``argc``, ``argv``)
  - zmienne środowiskowe (``environ``)
  - wektor pomocniczy (``auxv``)

- wskaźnik instrukcji jest ustawiany na początek programu (z nagłówka
  ELF). Przy standardowym procesie kompilacji, to pole jest ustawiane
  przez linker na adres symbolu ``_start``
- program rozpoczyna pracę

Należy zauważyć, że ``_start`` nie jest funkcją - nie używa standardowej
konwencji przekazywania parametrów ani nie może wrócić. Standardowa
implementacja ``_start`` przekazuje parametry funkcji ``main()``, po czym
wykonuje ``exit()`` z wartością zwróconą przez ``main()`` jako parametrem.

Linker dynamiczny
-----------------

Uruchomienie programu połączonego dynamicznie jest znacznie bardziej
skomplikowane - jądro nie potrafi tego zrobić w całości. Zamiast tego
używany jest specjalny program nazywany linkerem dynamicznym. Program
ten bywa też znany jako ``ld.so`` (od nazwy pliku w którym oryginalnie się
znajdował). Na architekturze i386 w systemie Linux dynamiczny linker
znajduje się w pliku ``/lib/ld-linux.so.2``.

Jądro rozpoznaje programy łączone dynamicznie po obecności semgentu typu
``PT_INTERP``, który zawiera nazwę pliku zawierającego linker dynamiczny.
Gdy taki znajdzie, zamiast przekazać sterowanie do programu po załadowaniu
go, ładuje dodatkowo i uruchamia wskazany linker dynamiczny (który jest
plikem typu ``ET_DYN``).

Linker dynamiczny rozpoczyna pracę przez znalezienie własnej sekcji
``_DYNAMIC`` i wypełnienie własnych relokacji. W następnej fazie, linker
przegląda wektor pomocniczy (``auxv``) przekazany przez jądro. Jest to
lista par klucz:wartość opisująca stan procesu i jego środowisko.
Zawiera np. informacje o położeniu nagłówków programu głównego pliku
wykonywalnego w pamięci. Po zlokalizowaniu pliku wykonywalnego, linker
ładuje (rekurencyjnie) jego zależności. Następnie, linker wypełnia
wszystkie relokacje z ``.rel.dyn``, zapycha stubami relokacje z ``.rel.plt``,
a na koniec przekazuje sterowanie do głównego programu (przez wykonanie
wskazanego w jego nagłówku ELF punktu wejścia).

Linker dynamiczny pozostaje w pamięci po załadowaniu programu i możliwe
jest dalsze używanie jego funkcji w celu otwarcia dodatkowych bibliotek,
poszukiwania symboli, itp. używając funkcji ``dlopen``, ``dlsym``, i innych.
Funkcje te dostępne są przez łączenie z biblioteką ``libdl``.

Przydatne polecenia
===================

Kompilacja pliku źródłowego w trybie PIC::

    gcc -c x.c -fPIC

Uwaga: istnieją dwie różne opcje włączające PIC: ``-fpic`` oraz ``-fPIC``. Ich
dokładne znaczenie zależy od architektury. Jeśli się różnią, ``-fpic`` używa
krótszych sekwencji kodu, ograniczonych jednak do mniejszych programów
(np. limit 1021 wpisów GOT na SPARCu). Na i386 obie wersje generują
taki sam kod.

Kompilacja i linkowanie biblioteki dynamicznej::

    gcc x.c -o libx.so -Wl,-soname=libx.so -shared -fPIC

Zrzut kodu, tabeli sekcji, symboli, i innych danych o pliku z kodem::

    objdump -xtrds <plik>

Zrzut informacji o strukturach ELF::

    readelf -a <plik>

Zrzut tabeli symboli::

    nm <plik>

Zrzut tabeli symboli dynamicznych::

    nm -D <plik>

Lista bibliotek używanych przez program::

    ldd <program>

Thread-Local Storage
====================

TLS jest dość skomplikowanym rozszerzeniem ELFa i języka C. Jest to
mechanizm dodający nową klasę zmiennych do języka (obok zmiennych
lokalnych i globalnych): zmienne wątkowe. Deklaruje się je tak (poza
zakresem funkcji)::

    __thread int x;

Zmienne wątkowe zachowują się podobnie do zmiennych globalnych, lecz
każdy działający wątek ma ich własną instancję. Pełna implementacja
jest dość skomplikowana (ze względu na możliwość dynamicznego tworzenia
zarówno wątków, jak i modułów definiujących zmienne wątkowe), więc
ograniczymy się tutaj do ogólnego zarysu.

Zmienne wątkowe przechowywane są na etapie kompilacji w sekcjach ``.tdata``
i ``.tbss``, mających dodatkową flagę ``SHF_TLS``. Same zmienne mają zaś typ
``STT_TLS``. Przy linkowaniu, wszystkie takie sekcje w module zbierane są
w jedno miejsce i tworzony jest segment typu ``PT_TLS`` opisujący to miejsce.

W trakcie wykonania, zmienna wątkowa może być przechowywana w jednym
z dwóch miejsc:

 - główny blok TLS: zawiera wszystkie segmenty TLS należące do głównego
   programu i bibliotek załadowanych razem z nim
 - dodatkowe bloki TLS: zawierają pozostałe segmenty TLS (tzn. te od
   bibliotek załadowanych przez dlopen)

Wskaźnik na listę dodatkowych bloków jest przechowywany w głównym bloku
dla danego wątku. Wskaźnik na główny blok jest przechowywany w jakimś
rejestrze procesora (``%gs`` na i386). Bloki dodatkowe są alokowane leniwie.

Istnieją 4 modele dostępu do zmiennych wątkowych, używane w zależności
od sytuacji:

- global dynamic: najbardziej ogólny, ładuje z GOT uchwyt biblioteki
  zawierającej symbol i offset zmiennej w segmencie TLS tej biblioteki,
  wywołuje ``__tls_get_addr`` aby pobrać adres tego segmentu (być może
  go alokując)

- local dynamic: jak global dynamic, ale zakłada, że jesteśmy w tym
  samym module co zmienna - w przypadku dostepu do kilku zmiennych
  adres segmentu TLS jest pobierany tylko raz

- initial executable: używany w ogólnym przypadku w programach ``ET_EXEC``
  i innych sytuacjach dających gwarancję, że zmienna jest w głównym
  bloku TLS - ładuje z GOT offset zmiennej w głównym bloku TLS

- local executable: używany w programach ``ET_EXEC`` do dostępu do jego
  własnych zmiennych - po prostu wpisuje offset w głównym bloku TLS
  do kodu programu

Debugowanie i obsługa wyjątków: DWARF
=====================================

DWARF
-----

Formatem blisko związanym z ELFem jest format informacji debuggera DWARF
(Debugging With Attributed Record Formats). Definiuje on wiele specjalnych
sekcji (o nazwach zaczynających się od .debug), które zawierają informacje
przydatne do debugowania programu, np.:

- informacje o numerach linii (``.debug_loc``)
- informacje o formacie ramek stosu (``.debug_frame``)
- informacje o typach i lokalizacjach zmiennych (``.debug_info``)
- informacje o makrach użytych w programie (``.debug_macro``)

Takie informacje są dołączane przez kompilator tylko, gdy zostanie o to
poproszony (``-g``).

Obsługa wyjątków, mechanizm unwind
----------------------------------

Jednym z najbardziej skomplikowanych mechanizmów wymaganych do pełnej
implementacji C++ jest obsługa wyjątków. Taki mechanizm ma trzy zadania:

- przejść przez listę wszystkich ramek stosu z ustawioną obsługą wyjątków
- ustalić, która z nich zawiera właściwą funkcję obsługi
- odtworzyć stan rejestrów z odpowiedniej ramki i wywołać w niej kod
  obsługi wyjątków

Mechanizm stack unwinding (odwijania stosu) powstał, aby rozwiązać
pierwsze i trzecie zadanie. Używa on sekcji ``.eh_frame`` bardzo podobnej
(ale nie identycznej) do ``.debug_frame`` w celu przejścia przez cały stos
i odtworzenia informacji o stanie procesora w momencie każdego wywołania.
    
Literatura
----------

1. Linker and Libraries Guide, 2004, Sun Microsystems (rozdziały 7 i 8) -
   http://docs.oracle.com/cd/E19683-01/817-3677/817-3677.pdf
2. gabi: http://www.uclibc.org/docs/SysV-ABI.pdf
3. psABI-i386a: http://www.uclibc.org/docs/psABI-i386.pdf
4. man dlsym, dlopen
5. ELF handling for thread local storage: www.akkadia.org/drepper/tls.pdf
6. The DWARF debugging standard: http://www.dwarfstd.org/

.. Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
.. 26.02.2013, 05.03.2013
