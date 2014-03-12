=======================
Zajęcia 4: Moduły jądra
=======================

Data: 11.03.2014, 12.03.2014

.. contents::

.. toctree::
   :hidden:

   p1_hello/index
   p2_export/index
   p3_msg/index
   p4_kmalloc/index


Materiały dodatkowe
===================

- :ref:`04-p1-hello`
- :ref:`04-p2-export`
- :ref:`04-p3-msg`
- :ref:`04-p4-kmalloc`


Co to jest moduł?
=================

Moduł to relokowalny kod/dane, które mogą być wstawiane i usuwane z jądra
w czasie działania systemu. Moduł może odwoływać się do (eksportowanych)
symboli jądra tak, jakby był skompilowany jako cześć jądra oraz sam może
udostepniać (eksportować) symbole, z ktorych mogą korzystać inne moduły.
Moduł odpowiada za pewną określoną usługę w jądrze.


Programy i pliki związane z zarządzaniem modułami w Linuksie
============================================================

``insmod [opcje] nazwa_modulu.ko [parametry]``
    Ładuje podany plik modułu do jądra. Jeśli są podane parametry, to
    przekazuje je modułowi. Należy podać pełną ścieżkę do modułu - ``insmod``
    nie próbuje samemu szukać potrzebnego pliku.
    Najważniejsze opcje:

    ``-p``
        Symuluje załadowanie modułu, ale go nie ładuje.

    Parametry maja postać zmienna=wartosc np.::

        irq=5
        device_name="dev"
        insmod ne.ko io=0x300 irq=7

``modprobe nazwa_modulu [parametry]``
    Przyjazna użytkownikowi nakładka na ``insmod`` - ładuje moduł, samemu
    znajdując go w ``/lib/modules`` i ładując wszystkie potrzebne mu do
    dzialania moduły (czyli te, których używa). W tym celu wykorzystywane
    są zależnosci między modułami utworzone za pomoca depmod (patrz
    poniżej). Moduły szukane są standardowo w katalogu
    ``/lib/modules/xx.xx.xx`` (``xx.xx.xx`` oznacza aktualną wersję jądra)
    i podkatalogach net, sound, itd.

``depmod -a``
    Tworzy zależności pomiędzy modułami dla aktualnego jądra (moduły
    znajdują się w katalogu ``/lib/modules/xx.xx.xx``, gdzie ``xx.xx.xx``
    oznacza aktualną wersję jądra). Zależności zostana wpisane do pliku
    ``/lib/modules/xx.xx.xx/modules.dep``.

``/etc/modprobe.conf`` i/lub ``/etc/modprobe.d/*``
    Pliki sterujące zachowaniem ``modprobe`` i ``depmod``.  Tradycyjnie był
    jeden plik konfiguracyjny, obecnie ze względu na łatwość modyfikacji
    używa się katalogu ``/etc/modprobe.d/``, w którym umieszcza się pliki
    zawierające opcje. W ten sposób można łatwo dodać opcje, np. gdy
    instalujemy jakieś urządzenie, bez konieczności modyfikacji pliku.

    Najważniejsze polecenia:

    ``alias nazwa nazwa_modulu``
        Definiuje, że moduł nazwa_modulu ma byc załadowany, gdy zażąda się
        załadowania modułu nazwa, np. ::

            alias eth0 ne2k-pci

        powoduje załadowanie odpowiedniego modułu karty sieciowej gdy
        zażąda się załadowania modułu ``eth0``

    ``options nazwa_modulu opcje``
        Powoduje ustawienie podanych opcji przy każdym żądaniu załadowania
        modułu, np. ::

            options ne io=0x300 irq=10

    ``install nazwa_modulu polecenia...``

        Powoduje wykonanie polecenia powłoki zamiast ładowania danego
        modułu. Możliwe jest również załadowanie modułu lub kilku modułów
        przez polecenie, np. ::

            install foo /sbin/modprobe bar; /sbin/modprobe --ignore-install foo $CMDLINE_OPTS

        Opcja ``--ignore-install`` jest konieczna, by zapobiec zapętleniu
        przy ładowaniu modułu foo, powoduje zignorowanie opcji install.
        Parametr ``$CMDLINE_OPTS`` zostanie zastąpiony opcjami podanymi
        w wywołaniu modprobe lub dołączonymi za pomocą poleceń options.
        Polecenie install przydaje się również do innych sztuczek, np.
        ładowania firmware po załadowaniu modułu. Możliwe jest też
        załadowanie pierwszego pasującego modułu za pomocą konstrukcji::

            install probe-ethernet /sbin/modprobe e100 || /sbin/modprobe eepro100

        Pierwszy moduł, który się pomyślnie załaduje powoduje zaprzestanie
        dalszego sprawdzania. W tym wypadku jest to pierwszy pasujący moduł
        do karty sieciowej.

    ``blacklist nazwa_modulu``
        Powoduje, że moduł nie będzie automatycznie ładowany (np. przez
        udev), przydaje się w przypadku zabugowanych nieużywanych przez
        nas sterowników lub modułów do debugowania (np. ``evbug``)

``rmmod nazwa_modulu``
    Usuwa podany moduł z jądra (jeśli nie jest używany). ``nazwa_modulu`` to
    nazwa modułu, a nie nazwa pliku ``.ko``!

``lsmod``
    Wypisuje wszystkie załadowane moduły wraz z informacją od jakich innych
    modułów zależą (ten sam wynik daje ``cat /proc/modules``).


Tworzenie modułów cz. I
=======================

Niezbedne deklaracje w pliku .c
-------------------------------

- ``#include <linux/module.h>``: włączenie deklaracji dla modułów

Konstruktor i destruktor modułu
-------------------------------

Każdy moduł może definiować funkcję inicjującą moduł (konstruktor)
i zwalniająca moduł (destruktor). Standardowo funkcje te muszą być zdefiniowane
w następujący sposób::

    int funkcja_inicjujaca(void) {
        /* ... */
    }
    void funkcja_zwalniajaca(void) {
        /* ... */
    }
    module_init(funkcja_inicjujaca);
    module_exit(funkcja_zwalniajaca);

Funkcja inicjująca jest wywoływana przy ładowaniu modułu. Zwraca kod błędu,
jeżeli nie udalo się zainicjowac modułu, w przeciwnym przypadku 0.
Funkcja zwalniająca jest wywoływana przy usuwaniu modułu.

Zadaniem funkcji inicjującej jest "wpięcie" funkcjonalności dostarczanej
przez moduł w struktury jądra - na przykłąd sterownik urządzenia PCI będzie
w tej funkcji informował podsystem PCI o obsługiwanych urządzeniach i funkcjach,
które powinien wywołać w razie wykrycia pasującego urządzenia. Bez takiej
rejestracji, jądro nigdy nie wywoła kodu naszego modułu, więc moduły bez funkcji
inicjującej są użyteczne w zasadzie jedynie jako biblioteczki funkcji dla innych
modułów.

Zadaniem funkcji zwalniającej jest odwrócenie wszystkiego, co zrobiła funkcja
inicjująca i posprzątanie po całej działalności modułu. Jeżeli moduł ma funkcję
inicjującą, zawsze należy dostarczyć też funkcję zwalniającą - w przeciwnym
wypadku, jądro uzna, że nasz moduł nie obsługuje usuwania, i nie pozwoli wykonać
na nim ``rmmod``.

Czasami można spotkać starsze moduły używające funkcji o domyślnych nazwach
``init_module()`` i ``cleanup_module()``, bez deklarowania ich przez
``module_init()`` i ``module_exit()``. Nie jest to zalecane w obecnych wersjach
jądra.


Zależności między modułami
==========================

Korzystanie z symboli zdefiniowanych w jądrze
---------------------------------------------

Jądro udostępnia zestaw eksportowanych symboli widocznych dla modułów tak
jakby były skompilowane w jądrze (można je obejrzeć w pliku ``/proc/kallsyms``).

Korzystanie z symboli z innnego modułu
--------------------------------------

Po załadowaniu modułu niektóre symbole mogą być dodane do symboli jądra i
stać się widocznymi dla następnie załadowanych modułów tak jak pozostałe
symbole jądra.

Symbole eksportowane przez kolejno ładowane moduły przechowywane są na
zasadzie stosu. Deklaracja w module ``C`` zmiennej eksportowanej o nazwie ``Z``
przykrywa poprzednią deklarację zmiennej eksportowanej ``Z`` z modułu ``A``
(załadowanego wcześniej). Wiązanie zmiennych odbywa się jednak podczas
ładowania modułu, więc deklaracja nowego adresu pod nazwą już
wykorzystywaną nie wpływa na moduły rezydujące w pamięci, tylko na te,
które będą załadowane po module eksportującym. Czyli moduł ``B``, załadowany
pomiędzy modułami ``A`` i ``C`` (eksportującymi symbol ``Z``), korzystający
z symbolu ``Z`` będzie widział zmienną eksportowaną z modułu ``A``, zaś moduł
``D`` załadowany po module ``C`` bedzie korzystał z tej z modułu ``C``.

Jeśli moduł ``B`` używa symbolu definiowanego przez moduł ``A``, to jest
od niego zależny, gdyż ``B`` nie może zostać załadowany zanim nie zostanie
załadowany moduł ``A``.

.. warning::
    Symbole zdefiniowane w jądrze nie mogą być zastąpione symbolem
    znajdującym się w module. Program wiążący symbole najpierw sprawdza, czy
    symbol został zdefiniowany w jądrze, a dopiero potem sprawdza moduły.

Automatyczne ładowanie potrzebnych modułów - kmod
-------------------------------------------------

Kmod to podsystem jądra zajmujący się ładowaniem modułów "na żądanie", tzn. gdy
wystąpi odwołanie do usługi związanej z danym modułem.

Gdy użytkownik zażąda dostępu do urządzenia, które jest obsługiwane przez moduł,
który nie jest załadowany, jądro zawiesza wykonanie programu i wykonuje funkcję
``request_module()`` żądając załadowania odpowiedniego modułu.  Funkcja ta jest
obsługiwana przez kmod i polega na wykonaniu programu (domyślnie
``/sbin/modprobe``, ale można to zmienić za pomocą ``/proc``) dla żądanego
modułu.

Przykład wykorzystania kmod:

Użytkownik próbuje zamontować partycję DOSa. Ponieważ obługa systemu plików
``msdos`` nie została wkompilowana w jądro, jądro żąda załadowania modułu
``msdos``.

Tworzenie modułów cz. II
========================

Licznik odwołań
---------------

Moduły zazwyczaj implementują obsługę urządzeń, których wykorzystanie wiąże
się z zajęciem pewnych zasobów. Licznik odwołań służy do kontroli, ile razy
zasoby modułu zostały zarezerwowane (i jeszcze nie zwolnione) przez inne
części jądra. W poprzednich wersjach jądra (2.4.x) licznik był zmieniany przez
sam moduł, jednak powodowało to race condition - taki licznik musi
być zmieniany przez coś zewnętrznego względem modułu. W jądrach 2.6
jest to realizowane przez ustawianie pól "właściciela" (``owner``) danej
struktury danych i jeśli zaczynamy z niej korzystać (np. montujemy system plików
implementowany przez moduł), wywoływana jest funkcja ``get_module()``,
która powoduje zwiększenie licznika odwołań do modułu i uniemożliwienie
odładowania go. Użycie symbolu wyeksportowanego przez moduł
w innym module również powoduje automatycznie uniemożliwienie odładowania.

Eksportowanie symboli
---------------------

Tylko symbole oznaczone makrem ``EXPORT_SYMBOL()`` są eksportowane
z modułu. Takie symbole powinny być zdefiniowane jako nie-static.
Wyeksportowany symbol może być używany przez inne moduły.

Przykład::

    EXPORT_SYMBOL(moja_funkcja)

Istnieje dodatkowa wersja makra: ``EXPORT_SYMBOL_GPL()``, która powoduje
wyeksportowanie symbolu, ale taki symbol może być używany
tylko przez moduły, który licencja (określona makrem ``MODULE_LICENSE()``)
jest zgodna z licencją jądra, np. jest to ``"GPL"``.

Parametryzacja modułów
----------------------

Można zadeklarować, że określona zmienna będzie zawierała parametr, ktory
może zostać zmieniony przy ładowaniu modułu. Nazwa parametru jest taka sama
jak nazwa zmiennej.

W czasie ładowania modułu w miejsce podanych zmiennych zostaną wstawione
wartości podane przez użytkownika (jeśli je poda), np. ::

    insmod modul.ko irq=5

podstawi w miejsce zmiennej ``irq`` wartosc 5.

Do deklaracji, że pewna zmienna ma być wykorzysta jako parametr modułu służy
makro::

    module_param(zmienna, typ, uprawnienia)

Typami moga być: ``byte``, ``short``, ``ushort``, ``int``, ``uint``, ``long``,
``ulong``, ``charp``, ``bool``, ``invbool``. Typ ``charp`` jest używany
do przekazywania napisów (``char *``).  Typ ``invbool`` oznacza parametr
``bool``, który jest zaprzeczeniem wartości.

Można definiować własne typy parametrów, trzeba wówczas zdefiniować również
funkcje ``param_get_XXX``, ``param_set_XXX`` i ``param_check_XXX``.

Uprawnienia oznaczają uprawnienia, które zostaną nadane parametrowi w ``sysfs``.
Należy je ustawić na 0.

Każdy parametr powinien posiadać opis. Opis parametru można potem odczytać wraz
z opisem całego modułu za pomocą programu ``modinfo``, dzięki czemu moduł niesie
ze sobą opis użycia. Opis nadaje się za pomocą makra ``MODULE_PARM_DESC``::

    MODULE_PARM_DESC(zmienna, opis)

Przykłady::

    int irq = 7;
    module_param(irq, int, 0);
    MODULE_PARM_DESC(irq, "Irq used for device");

    char *path="/sbin/modprobe";
    module_param(path, charp, 0);
    MODULE_PARM_DESC(path, "Path to modprobe");

Użycie::

    printk(KERN_INFO "Using irq: %d", irq);
    printk(KERN_INFO "Will use path: %s", path);

Aby zadeklarować tablicę parametrów trzeba użyć innej funkcji::

    module_param_array(zmienna, typ, wskaznik_na_licznik, uprawnienia)

Wszystkie pola poza ``wskaznik_na_licznik`` mają takie same znaczenie
jak w ``module_param()``. ``wskaznik_na_licznik`` zawiera wskaźnik do zmiennej
do której wpisana zostanie liczba elementów tablicy. Jeśli nie interesuje nas
liczba argumentów, można podać ``NULL``, ale wtedy trzeba rozpoznawać, czy
argument jest czy, nie na podstawie jego zawartości, co nie jest wskazane.
Maksymalna liczba elementów tablicy jest określona przez deklarację tablicy,
np. jeśli zadeklarujemy jej rozmiar na 4, to użytkownik będzie mógł przekazać
maksymalnie 4 elementy. W opisie parametru tablicowego zwyczajowo umieszcza się
w nawiasach kwadratowych maksymalną liczbę parametrów.

Przykład::

    int num_paths = 2;
    char *paths[4] = {"/bin", "/sbin", NULL , NULL};
    module_param_array(paths, charp, &num_paths, 0);
    MODULE_PARM_DESC(paths, "Search paths [4]");

Użycie::

    int i;
    for (i=0; i<num_paths; ++i)
        printk(KERN_INFO "Path[%d]: %s\n", i, paths[i]);

W obecnej wersji jądra opis parametrów jest umieszczany w sekcji ``.modinfo``
pliku typu ELF.

Inne informacje o module
------------------------

Następujące makra pozwalają opisać moduł:

Autor modułu::

    MODULE_AUTHOR("Imie Nazwisko <email>");

Opis modułu::

    MODULE_DESCRIPTION("Modul implementujacy...");

Licencja rozprowadzania modułu::

    MODULE_LICENSE("Licencja");

Następujące rodzaje licencji są rozpoznawane jako wolne oprogramowanie zgodne
z licencją GPL:

- ``"GPL"`` - GNU Public License v2 lub późniejsza),
- ``"GPL and additional rights"`` - prawa GNU Public License v2 + dodatkowe
- ``"Dual BSD/GPL"`` - GNU Public License v2 lub licencja BSD do wyboru
- ``"Dual MPL/GPL"`` - GNU Public License v2 lub Mozilla do wyboru

Licencja typu ``"Proprietary"`` oznacza produkt zamknięty.

Opis modułu wraz z opisem parametrów można uzyskać za pomocą programu modinfo.
Przykładowy wydruk::

    $ modinfo msg_mod.ko
    filename:       msg_mod.ko
    license:        GPL
    vermagic:       2.6.15-27-386 preempt 486 gcc-4.0
    depends:
    srcversion:     2E55EFDB5F1152C9259E0F7
    parm:           msgs:array of charp
    parm:           msg:My message (charp)

Wersje modułów i jądra
----------------------

Ze względu na możliwość niezgodności kodu modułów z kodem jądra, gdy pochodzą
z różnych wersji systemu, moduł powinien być od nowa skompilowany z każdą nową
wersją jądra, do której będzie dołączany. Każdy moduł deklaruje symbol
``__module_kernel_version``. Program ``insmod``, przed załadowaniem modułu,
porównuje go z aktualną wersją jądra (i może odmówić załadowania modułu
w przypadku niezgodności wersji). Symbol jest umieszczony w sekcji ``.modinfo``
formatu ELF.

W celu zapewnienia większej przenośności modułów między wersjami jądra dodano
wersjonowanie symboli. Jeśli wersjonowanie symboli jest włączone i moduł
korzysta z niego, wówczas każdy symbol posiada wersję, będącą skrótem CRC
definicji jego struktury (w C). W ten sposób można z dużą dozą pewności uznać,
że jeśli definicja się nie zmieniła to moduł będzie działać. Jeśli wszystkie
symbole w jądrze są zgodne z wersjami użytymi w module, to moduł może zostać
załadowany na innym jądrze niż na tym, na którym został skompilowany. Jeśli
moduł nie obsługuje wersji symboli, wówczas może być załadowany tylko
na dokładnie tej wersji jądra, dla której został skompilowany.

.. warning::
    To, że nie została zmieniona definicja symbolu nie daje
    *gwarancji*, że nie została zmieniona semantyka używania, dlatego
    dobrą praktyką jest rekompilacja modułu dla każdej nowej wersji jądra
    oraz oczywiście analiza, czy nowa wersja jądra nie wymaga dodatkowych
    czynności, które moduł powinien wykonać. Takie zmiany nie powinny
    występować w stabilnych interfejsach jądra (np. przykład z binfmt_misc
    pochodzący z jądra 2.4.18 został bez problemu przekompilowany
    na jądrze 2.6.17.13 bez zmiany funkcjonalności), ale w przypadku
    szybko zmieniających się części jądra może taka konieczność zainstnieć.

Ładowanie modułów na żądanie - kmod
-----------------------------------

Jeśli w module ma być wykorzystywane ładowanie modułów na żądanie to należy
dołączyć::

    #include <linux/kmod.h>

Doładowanie modułu jest możliwe dzięki funkcji::

    int request_module(const char *module_name)

Ładowanie modułów na żądanie nie może być wykorzystane w kodzie modułu do
automatycznego dołączania modułów, które eksportują symbole wykorzystywane
przez ten moduł, bo przed uruchomieniem funkcji inicjalizującej (w której
możemy zażądać dodania potrzebnego modułu) wszystkie symbole muszą być już
związane. Ładowaniem modułów od których zależy dany moduł zajmie się
program modprobe, pod warunkiem, że moduł zostanie umieszczony
razem z innymi modułami w katalogu ``/lib/modules/wersja_jądra`` oraz
że zostanie uruchomione polecenie ``depmod``.

Biblioteki
----------

Wewnątrz jądra nie można korzystać z żadnych bibliotek znanych z przestrzeni
użytkownika, nawet standardowej biblioteki C. Jądro posiada jednak własną
biblioteczkę podstawowych funkcji, zawierającą wiele funkcji znanych ze
standardowej biblioteki C lub bardzo do nich podobnych, między innymi:

- większość funkcji znanych ze ``string.h`` (``memcpy``, ``strcmp``, ``strcpy``,
  ...)
- ``kstrto[u](int|l|ll)``: funkcje konwersji ze stringów w liczby, podobne
  do standardowych ``strto*``, lecz z innym interfejsem
- ``malloc``/``free``/``calloc``: nie istnieją, zastąpione przez ``kmalloc``,
  ``vmalloc``, i kilka innych alokatorów pamięci w zależności od potrzeb
- ``snprintf``/``sscanf``: działają podobnie do zwykłych, ale posiadają inny
  zestaw formatów (np. ``%pI4`` drukuje adres IPv4)
- ``bsearch``: jak w standardzie C
- ``sort``: jak standardowy ``qsort``, ale trzeba mu jeszcze przekazać funkcję
  swapującą dwa elementy

Funkcje te są zawarte w innych nagłówkach niż zwykle: musimy użyć np.
``linux/string.h``, ``linux/bsearch.h``, itd.

Zastosowanie modułów
====================

Moduły najczęściej są wykorzystywane jako części jądra implementujące
sterowniki urządzeń (to zastosowanie modułów bedzie omawiane na następnych
zajęciach), systemy plików, czy protokoły sieciowe.

Ćwiczenia wprawkowe
===================

- Zbadać doświadczalnie maksymalny rozmiar, który można zaalokować za
  pomocą ``kmalloc``.
- Przerobić kmalloc_mod tak, by działał dla większych buforów (za pomocą
  ``vmalloc``).
- Znaleźć i wyjaśnić dziurę bezpieczeństwa w jednym z kodów przykładowych.
  Zastanowić się nad konsekwencjami tego typu błędów w kodzie jądra.

Literatura
==========

1. ``man insmod``, ``rmmod``, ``lsmod``, ``modprobe``, ``depmod``, ``modinfo``
2. ``man nm``, ``objdump``
3. A. Rubini, J. Corbet "Linux Device Drivers" 2nd Edition, O'Reilly 2001,
   rozdział II i XI - http://www.xml.com/ldd/chapter/book
4. Peter Salzman, Ori Pomerantz "The Linux Kernel Module Programming Guide",
   2001 - http://www.faqs.org/docs/kernel
5. http://tldp.org/HOWTO/Module-HOWTO/
6. http://tldp.org/LDP/lkmpg/2.6/html/index.html
7. ``Documentation/kbuild/makefiles.txt``, ``modules.txt``


..
    ==========================================================================
    Autor: Grzegorz Marczyński (g.marczynski@mimuw.edu.pl)
    Aktualizacja: 2004-10-19
    ==========================================================================
    Maria Fronczak (marys@mimuw.edu.pl)
    Aktualizacja: 23.10.2005
    ==========================================================================
    Krzysztof Lichota (lichota@mimuw.edu.pl)
    Aktualizacja do jądra 2.6.17.13: 8.11.2006
    Poprawka /proc/ksyms na /proc/kallsyms: Jan Urbański
    ==========================================================================
    Krzysztof Lichota (lichota@mimuw.edu.pl)
    Aktualizacja do jądra 2.6.33.4, przeróbka na Slackware i dodanie p4_kmalloc:
    21.03.2011
    ==========================================================================
    Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
    Drobne poprawki, dziura, konstruktory/destruktory, podstawowe funkcje.
    09.02.2012
    ==========================================================================
    Marek Dopiera (dopiera@mimuw.edu.pl)
    Przepisanie makefile-i.
    12.03.2012
