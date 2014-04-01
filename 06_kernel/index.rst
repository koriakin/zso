======================================
Zajęcia 6: Interfejsy wewnętrzne jądra
======================================

Data: 25.03.2014, 26.03.2014

.. contents::

.. toctree::
   :hidden:

   p1_scull/index


Materiały dodatkowe
===================

- :ref:`06-p1-scull`

Wprowadzenie
============

Na poprzednich zajęciach przedstawione zostały sterowniki urządzeń znakowych i
blokowych, ich reprezentacja plikowa i koncepcja tablic rozdzielczych
sterowników. Opisano sposób rejestracji i wyrejestrowania sterowników oraz
metody przydzielania numerów głównych, a także operacje na pliku
reprezentującym urządzenie. Na niniejszych zajęciach omówimy funkcjonalności
jądra przydatne przy pisaniu bardziej złożonych sterowników oraz przedstawimy
przykład takiego sterownika.


Wzajemne wykluczanie
====================

Synchronizacja w jądrze Linuksa wraz z jego rozwojem ciągle zyskuje na
ważności. Warto uważniej przeanalizować związane z tym zagadnienia opisane
tutaj oraz np. w rozdziale 5 [1].

Wszystkie z poniższych funkcji działają poprawnie zarówno na systemach
jedno- jak i wieloprocesorowych. Wiele z nich jest zaimplementowane
wewnętrznie w dwóch wersjach, zapewniających wyższą wydajność na systemach
jednoprocesorowych - np. implementacja spinlocków sprowadza się do
wyłączenia wywłaszczania jądra (lub zablokowania przerwań).


Zwykłe blokady
--------------

Przedstawione na poprzednich zajęciach.

Semafory systemowe
------------------

Semafory w Linuksie służą do synchronizacji procesów i reprezentuje je
struktura ``struct semaphore`` (zdefiniowana w ``asm/semaphore.h``).

Semafory można deklarować używając makr np::

   static DECLARE_SEMAPHORE_GENERIC(sem_ogolny, 11);

Dostępne są między innymi następujące operacje::

    void down(struct semaphore *sem)
    int down_interruptible(struct semaphore *sem)
    int down_trylock(struct semaphore *sem)

Funkcje opuszczające semafor systemowy. Warianty analogiczne do zwykłych
blokad.

::

    void up(struct semaphore *sem)

Funkcja podnosi semafor systemowy.

Linux udostępnia także dostępne semafory typu czytelnicy-pisarze typu
``struct rw_semaphore`` (``linux/rwsem.h``).

Ćwiczenie:

Proszę zapoznać się dostępnymi dla semaforów typu czytelnicy-pisarze
funkcjami: (``linux/rwsem.h``).

Blokady wirujące (spin locks)
-----------------------------

Blokady wirujące mają podobne działanie do zwykłych blokad, lecz używają
aktywnego oczekiwania zamiast blokowania procesu. Można ich przez to
używać w miejscach, gdzie blokowanie procesu byłoby niedopuszczalne
(przede wszystkim funkcje obsługi przerwań), lub gdy zwykłe blokady
znacznie zmniejszałyby wydajność (tj. przy małej ilości operacji
chronionych).

Należy pamiętać, że NIE można wykonywać żadnych(!) operacji blokujących
po zajęciu blokady wirującej (najważniejsze z funkcji, które nie
współpracują z blokadami wirującymi to: ``copy_to/from_user``, ``kmalloc``,
``down``, ``sleep_on``, ``mutex_lock``). Przed ich wywołaniem należy zwolnić
blokadę.

Podstawowe blokady wirujące
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Blokady wirujące (zdefiniowane w ``asm/spinlock.h``) w ogólnym przpadku
wykorzystuje się w nastepujący sposób::

    spinlock_t xxx_lock = SPIN_LOCK_UNLOCKED;
    unsigned long flags;

    spin_lock_irqsave(&xxx_lock, flags);
    /* ... sekcja krytyczna ... */
    spin_unlock_irqrestore(&xxx_lock, flags);

Powyższe wywołanie jest zawsze bezpieczne (bo blokuje przerwania na
lokalnym procesorze, a następnie przywraca pierwotną obsługę przerwań).

Istnieje również uproszczona wersja, której możemy użyć, gdy dana blokada
NIGDY nie jest używana w funkcji obsługi przerwań::

    spinlock_t xxx_lock = SPIN_LOCK_UNLOCKED;

    spin_lock(&xxx_lock);
    /* ... sekcja krytyczna ... */
    spin_unlock(&xxx_lock);


Blokady wirujące typu czytelnicy-pisarze (reader-writer spinlocks)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ten typ blokady wirującej (zdefiniowane w ``asm/spinlock.h``) umożliwia
odczyt bez wyłączności i wyłączność na pełny dostęp do danych (np.
zapis)::

    rwlock_t xxx_lock = RW_LOCK_UNLOCKED;
    unsigned long flags;

    read_lock_irqsave(&xxx_lock, flags);
    /* .. sekcja krytyczna czytająca ... */
    read_unlock_irqrestore(&xxx_lock, flags);

    write_lock_irqsave(&xxx_lock, flags);
    /* .. sekcja krytyczna z wyłącznym dostępem ... */
    write_unlock_irqrestore(&xxx_lock, flags);

Sprawdza się przy skomplikowanych strukturach danych, gdy większość
operacji polega na przechodzeniu (odczycie) struktury. Tylko faktyczna
zmiana (zapis) wymaga wyłączności.

Istnieje również uproszczona wersja (bez blokowaniem przerwań) blokad
wirujących typu czytelnicy-pisarze do wykorzystania w sytacjach, gdy
użycie tej samej blokady nie może pojawić się w programie obsługi
przerwania::

    rwlock_t xxx_lock = RW_LOCK_UNLOCKED;

    read_lock(&xxx_lock);
    /* .. sekcja krytyczna czytająca ... */
    read_unlock(&xxx_lock);

    write_lock(&xxx_lock);
    /* .. sekcja krytyczna z wyłącznym dostępem ... */
    write_unlock(&xxx_lock);

Operacje atomowe
----------------

Istnieją typy ``atomic_t`` i ``atomic64_t``. Dla każdego z tych typów
zdefiniowane są niepodzielne operacje pozwalające na:

- zainicjowanie, np: ``atomic_set(&zmienna,wartosc);``
- zmianę, np: ``atomic_add(&zmienna, 12);``
- jednoczesną zmianę i testowanie czy nowa wartość zmiennej jest zerem,
   np: ``atomic64_sub_and_test(1L, &zmienna64);``

Po szczegóły odsyłam do pliku ``asm/atomic.h``.
Istnieją także bitowe operacje atomowe ``asm/bitops.h``.

Debugowanie blokad
------------------

Jądro posiada dość zaawansowany wbudowany kod sprawdzający poprawność
blokowania, nazywany lockdep. Pozwala on wykryć potencjalne zakleszczenia
przez analizę sekwencji blokad zajmowanych przez pojedyncze wątki jądra.
Zakleszczenie nie musi wystąpić, aby zostać wykryte - wystarczy, że każda
wymagana do zakleszczenia sekwencja została kiedykolwiek użyta przez jakiś
wątek.

Wykrywane klasy błędów to proste błędy w użyciu blokad (np. niezainicjowane
blokady, rekurencyjna blokada), zakleszczenia powodowane przez cykliczne
zależności blokad, oraz zakleszczenia powodowane przez nieprawidłowe użycie
blokad wewnątrz obsługi przerwań.

Debugowanie blokad należy jawnie włączyć w konfiguracji jądra, gdyż
spowalnia ono działanie systemu.


Operacje blokujące
==================

Kolejki oczekiwania
-------------------

Proces, który musi oczekiwać na zajście pewnego zdarzenia (np.
zakończenie operacji we/wy, czy pojawienie się danych w kolejce FIFO),
usuwany jest z kolejki procesów gotowych i umieszczany jest w kolejce
oczekiwania (waitqueues) - stan procesu zmienia się z ``TASK_RUNNING`` na
``TASK_INTERRUPTIBLE`` bądź ``TASK_UNINTERRUPTIBLE``. Strukturą reprezentującą
głowę listy, w której umieszcza się takie procesy, jest ``wait_queue_head_t``,
zdefiniowana w pliku ``linux/wait.h``.

Wybrane operacje:

``DECLARE_WAIT_QUEUE_HEAD(name)``
    przed użyciem ``wait_queue`` trzeba ją zainicjować
``wait_event(name, cond)``
    umieszczenie procesu w kolejce oczekiwania w stanie odpornym na sygnały
    (``TASK_UNINTERRUPTIBLE``) i spanie, aż cond będzie zachodzić
``wait_event_interruptible(name, cond)``
    j.w., ale przejście w stan ``TASK_INTERRUPTIBLE`` (nadejście sygnału
    spowoduje przejście w stan ``TASK_RUNNING``), zwraca 0 w przypadku zajścia
    ``cond``, ``-ERESTARTSYS`` w przypadku przerwania sygnałem
``wake_event_timeout(name,cond,timeout)``
    jak ``wait_event``, ale z budzeniem po upływie określonego czasu nawet jeśli
    ``cond`` nie zaszło
``wake_event_interruptible_timeout(name,cond,timeout)``
    j.w., ale przejście w stan ``TASK_INTERRUPTIBLE``
``wake_up(name)``
    budzi wszystkie czekające procesy bez ustawionej flagi ``WQ_FLAG_EXCLUSIVE``
    oraz jeden, który ma ustawioną. Uwaga: funkcja nie powoduje usunięcia
    procesu z kolejki oczekiwania - proces "usunie" się sam, po wznowieniu
    działania
``wake_up_interruptible(name)``
    j.w., ale tylko dla procesów w stanie ``TASK_INTERRUPTIBLE``
``wake_up_all(name)``
    wstawienie wszystkich procesów (będących w stanie ``TASK_INTERRUPTIBLE``
    bądź ``TASK_UNINTERRUPTIBLE``) znajdujących się w kolejce oczekiwania
    do kolejki procesów gotowych

Powyższe operacje zdefiniowane są w ``linux/sched.h`` (oprócz pierwszej,
zdefiniowanej w ``linux/wait.h``).

Używając kolejek oczekiwania należy zwrócić uwagę na uniknięcie wyścigów.
Jeżeli np. jesteśmy czytelnikiem i chcemy poczekać aż pisarz zapisze
jakiekolwiek dane do bufora zabezpieczonego blokadą, kod oczekujący
może wyglądać tak::

    /* definicje */
    DEFINE_MUTEX(blokada);
    DECLARE_WAIT_QUEUE_HEAD(kolejka);
    int poz_odczyt, poz_zapis;
    char *bufor;

    /* wczytanie bajtu */
    char znak;
    /* blokujemy */
    mutex_lock(&blokada);
    /* sprawdzamy, czy już mamy dane do przeczytania */
    while (poz_odczyt == poz_zapis) {
        /* nie mamy - musimy zdjąć blokadę, aby pisarz mógł cokolwiek
         * zapisać */
        mutex_unlock(&blokada);
        /* czekamy na zapis - warunek zapewnia, że nie będziemy czekać,
           jeśli pisarz uaktualnił poz_zapis między zdjęciem przez nas
           blokady, a dodaniem nas do kolejki - w przeciwnym przypadku
           wake_up pisarza nie objęło by naszego procesu i czekalibyśmy
           znacznie dłużej (być może na zawsze). Warunek jest sprawdzany
           po dodaniu do kolejki oczekiwania, ale przed samym
           oczekiwaniem. */
        wait_event(kolejka, poz_odczyt != poz_zapis);
        /* wait_event zapewnia tylko, że w pewnym momencie od wywołania
           zaszło poz_odczyt != poz_zapis, lecz jakiś inny wątek mógł
           w międzyczasie opróżnić bufor - nie możemy niczego zakładać
           póki nie sprawdzimy tego warunku trzymając blokadę - zakładamy
           więc blokadę i sprawdzamy jeszcze raz warunek while, do skutku */
        mutex_lock(&blokada);
    }
    /* udało się - możemy odczytać znak */
    znak = bufor[poz_odczyt++];
    mutex_unlock(&blokada);

    /* zapis bajtu */
    mutex_lock(&blokada);
    /* musimy zapewnić spełnienie warunku, na który czekają czytelnicy
     * PRZED obudzeniem czytelników */
    bufor[poz_zapis++] = znak;
    wake_up(kolejka);
    mutex_unlock(&blokada);

Czekanie na zakończenie - wait for completion
---------------------------------------------

Jest to atomowa operacja czekania na zakończenie wykonywania pewnej
operacji. Powstała (zupełnie niedawno) w związku z tym, że użycie do
tego semaforów powodowało race conditions. Chodziło o sytuacje typu:
proces czekający dysponuje semaforem zainicjalizowanym na 0 i w pewnym
momencie wykonuje na nim operację ``down()``, która blokuje go, aż inny
proces, gdy nadejdzie odpowiednia pora, wykona ``up()`` i go odblokuje.

Oto typowe wykorzystanie mechanizmu "czekania na zakończenie"::

    struct completion event;

    init_completion(&event);

    /* .. przekaż wskaźnik do event temu, kto ma budzić .. */

    wait_for_completion(&event);

Ten budzący wywołuje::

    complete(&event)

gdy nadejdzie odpowiednia pora.

Implementacja jest w pliku ``kernel/sched.c``, zaś sama struktura jest
zadeklarowana w ``linux/completion.h``.

Indywidualne tworzenie urządzeń znakowych
=========================================

Na poprzednich zajęciach została przedstawiona funkcja ``register_chrdev``,
jednocześnie alokująca i przypinająca zakres numerów urządzeń znakowych.
Czasami przydatny jest jednak bardziej elastyczny interfejs.

Alokacją (ale nie podponaniem) numerów urządzeń zajmują się funkcje
``register_chrdev_region`` i ``alloc_chrdev_region``::

    int register_chrdev_region(dev_t first, unsigned int count, const char
        *name);
    int alloc_chrdev_region(dev_t *first, unsigned int count, const char
        *name);

Pierwszy parametr jest początkowym numerem urządzenia (major:minor),
drugi jest ilością minorów do alokacji, a ostatni jest nazwą sterownika.
Funkcja ``register_chrdev_region`` rejestruje podane z góry numery, zaś
funkcja ``alloc_chrdev_region`` sama znajduje jakiś wolny numer. Obie funkcje
zwracają 0 w przypadku sukcesu, kod błędu w przeciwnym przypadku.

Przy usuwaniu sterownika, należy zwrócić tak zaalokowane numery przez::

    void unregister_chrdev_region(dev_t first, unsigned int count);

Indywidualne urządzenia są reprezentowane przez strukturę ``struct cdev``. Aby
utworzyć urządzenie, należy stworzyć i zainicjować tą strukturę, wypełnić
ją danymi, po czym zarejestrować urządzenie przez wywołanie ``cdev_add``.

Inicjalizacja struktury odbywa się jedną z dwóch funkcji, w zależności
od potrzeb::

    struct cdev *cdev_alloc(void);
    void cdev_init(struct cdev *cdev, const struct file_operations *fops);

``cdev_init`` inicjalizuje istniejącą już strukturę ``cdev`` (która może być
np. elementem jakiejś większej struktury), zaś ``cdev_alloc`` alokuje nową
strukturę ``cdev`` i ją inicjalizuje. W przypadku ``cdev_init`` mamy też
możliwość podania od razu struktury ``file_operations`` (w przypadku
``cdev_alloc`` trzeba ją potem wpiąć ręcznie przez pole ``ops``).

Po zainicjalizowaniu struktury i wypełnieniu jej pola ``ops`` należy ją wpiąć
do systemu przez wywołanie ``cdev_add``::

    int cdev_add(struct cdev *p, dev_t dev, unsigned count);

Drugi parametr jest początkowym numerem major:minor, pod który struktura
ma być podpięta, zaś trzeci jest ilością kolejnych minorów do podpięcia.
Funkcja zwraca 0 w przypadku sukcesu. Po wywołaniu funkcji, urządzenie
jest już widoczne w systemie plików i może być użyte przez użytkownika.

Aby odpiąć urządzenie, należy wywołać ``cdev_del``::

    void cdev_del(struct cdev *p);

Jeśli struktura ``cdev`` była stworzona przez ``cdev_alloc``, struktura zostanie
automatycznie zwolniona. Jeśli natomiast była inicjowana przez ``cdev_init``,
zwolnienie jej jest sprawą sterownika.

Należy zauważyć, że ``cdev_del`` tylko odpina urządzenie z tablicy urządzeń,
ale nie gwarantuje, że nikt już go nie używa - wcześniej otwarte
deskryptory plików dalej będą działać (choć jeśli jesteśmy w ``module_exit``,
mamy gwarancję że takich deskryptorów nie ma). W przypadku implementacji
np. urządzenia, które powinno obsługiwać hot-unplug trzeba samemu
zapewnić np. zliczanie referencji.

Automatyczne tworzenie plików urządzeń
======================================

Od jądra w wersji 2.6.13 istnieje alternatywna metoda zarządzania
plikami urządzeń w katalogu ``/dev`` - demon ``udev``. Demon ten działa
w przestrzeni użytkownika, oczekując na wiadomości od jądra o nowo
załadowanych urządzeniach, tzw. ueventy. W momencie otrzymania takiej
informacji, wykonywane są modyfikowalne przez użytkownika skrypty udeva
oraz tworzony jest plik urządzenia.

Podstawowymi zaletami są:

- możliwość tworzenia/usuwania specjalnych plików w katalogu ``/dev`` przy
  inicjalizacji/usuwaniu sterownika urządzenia;
- sterownik może ustalić nazwę, własność i uprawnienia pliku, zaś
  programy z przestrzeni użytkownika mogą zmieniać jedynie własność i
  uprawnienia;
- nie ma konieczności przypisywania urządzeniu numeru głównego (major) i
  zajmowania się numerami drugorzędnymi (minor).

W efekcie można uniknąć wywoływania skryptu, który podczas ładowania
modułu utworzy odpowiednie pliki specjalne.

Szczegóły udev można poznać w [2].

Rejestracja urządzenia znakowego
--------------------------------

Aby urządzenie zostało zauważone przez udeva, musi zostać utworzony
odpowiadający mu węzeł w hierarchii urządzeń jądra. Najpierw należy
stworzyć klasą urządzeń dla naszego sterownika, używając ``class_create``::

    struct class *class_create(struct module *owner, const char *name);

Następnie tworzymy właściwe węzły urządzeń:::

    struct device *device_create(struct class *cls, struct device *parent,
                   dev_t devt, void *drvdata,
                   const char *fmt, ...);

``parent`` wskazuje na urządzenie, do którego nasze urzadzenie jest podłączone
- katalog w sysfs odpowiadający naszemu urządzeniu będzie podkatalogiem
katalogu podanego urządzenia. W przypadku sterowników urządzeń znakowych
odpowiadających np. urządzeniom PCI, parent będzie ustawiony na pole ``dev``
struktury ``pci_device``. Można ustawić ten parametr na ``NULL``, aby otrzymać
urządzenie najwyższego poziomu.

``devt`` powinno być ustawione na ``MKDEV(major, minor)`` naszego urządzenia
znakowego. ``drvdata`` może służyć do przekazywania dodatkowych informacji
prywatnych dla naszego sterownika (przydatne jeśli np. chcemy stworzyć
pliki w sysfs do kontroli naszego urządzenia). ``fmt`` i dalsze parametry
przekazywane są do ``sprintf`` w celu stworzenia nazwy urządzenia, która
pojawi się w ``/dev``.

Aby wyrejestrować urządzenie, należy wykonać::

    void device_destroy(struct class *cls, dev_t devt);
    void class_destroy(struct class *cls);

Wszystkie te funkcje dostępne są w ``linux/device.h``.

Kody błędów
===========

Kody błędów w kernelu zwracane są na jeden z dwóch sposobów:

- w przypadku funkcji zwracających ``int`` (lub inny typ całkowity), jako
  ujemne wartości
- w przypadku funkcji zwracających wskaźnik, jako specjalne wartości
  (będące ujemnymi kodami błędów przerzutowanymi na wskaźnik)
- w przypadku funkcji zwracających wskaźnik, jako ``NULL`` (z kodem błędu
  implikowanym przez typ funkcji - np. ``-ENOMEM`` dla ``kmalloc``)

Dostarczane są nastepujące makra (``linux/err.h``) pomagające w obsłudze tych
kodów:

``IS_ERR_VALUE(x)``
    prawda jeśli ``x`` (liczba całkowita) jest kodem błędu (czyli ma wartość
    -4096..-1)
``void *ERR_PTR(long error)``
    konwertuje kod błędu z liczby na wskaźnik
``long PTR_ERR(const void *ptr)``
    konwertuje w odwrotnym kierunku
``long IS_ERR(const void *ptr)``
    prawda jeśli wskaźnik jest kodem błędu
``long IS_ERR_OR_NULL(const void *ptr)``
    prawda jeśli wskaźnik jest kodem błędu lub NULLem
``void *ERR_CAST(const void *ptr)``
    konwertuje kod błędu ze wskaźnika na wskaźnik (przydatne w wypadku różnych
    typów wskaźników)

Inne przydatne funkcje kernela
==============================

Biblioteka jądra zawiera wiele innych gotowych funkcji, które mogą się
okazać przydatne przy pisaniu najróżniejszych sterowników.
Z przydatniejszych można wymienić:

``linux/idr.h``
    mapa ``int`` w ``void *`` z dynamiczną alokacją identyfikatorów
``linux/kref.h``
    łatwe zliczanie referencji
``linux/bitmap.h``
    efektywne tablice bitów
``linux/btree.h``
    B-drzewa
``linux/bug.h``, ``asm-generic/bug.h``
    raportowanie krytycznych błędów w kodzie sterownika wynikających z wad kodu
    oraz ostrzeżeń (coś w rodzaju ``assert``)
``linux/circ_buf.h``
    bufory cykliczne
``linux/hash.h``
    Proste funkcje hashujące
``linux/kernel.h``
    Różne proste funkcje:

    ``ALIGN(x,a)``
        wyrównuje ``x`` w dół do wielokrotności ``a`` (``a`` musi być potęgą dwójki)
    ``PTR_ALIGN(p, a)``
        jak wyżej, ale na wskaźnikach
    ``IS_ALIGNED(x, a)``
        sprawdza, czy ``x`` już jest wyrównane
    ``ARRAY_SIZE(arr)``
        rozmiar tablicy ``arr``
    ``DIV_ROUND_UP(n,d)``
        ``n/d``, zaokrąglając w górę
    ``roundup(x, y)``
        zakrągla ``x`` w górę do wielokrotności ``y``
    ``upper_32_bits(x), lower_32_bits(x)``
        jak w nazwie
    ``might_sleep()``
        oznacza miejsce, w którym kod może spać, pomaga w debugowaniu (rzuca
        błąd jeśli debugownie spinlocków jest włączone, a trzymany jest spinlock)
    ``min(x,y), max(x,y)``
        jak w nazwie
    ``clamp(val, min, max)``
        ``val`` przycięte do zakresu [``min``, ``max``]

``linux/kobject.h``
    ogólny typ obiektowy ze zliczaniem referencji i widocznością w sysfs (na
    ich podstawie jest zrobiony m.in. ``cdev`` oraz ``device``)
``linux/parser.h``
    prosty parser do opcji
``linux/rbtree.h``
    drzewa czerwono-czarne


Literatura
----------

1. A. Rubini, J. Corbet, G. Kroah-Hartman, Linux Device Drivers, 3rd edition,
   O'Reilly, 2005. (http://lwn.net/Kernel/LDD3/)
2. http://webpages.charter.net/decibelshelp/LinuxHelp_UDEVPrimer.html
3. Książki podane na stronie przedmiotu: http://students.mimuw.edu.pl/ZSO/

..
    ==============================================================================
    Autor: Grzegorz Marczyński (g.marczynski@mimuw.edu.pl)
    Aktualizacja: 2003-03-13
    Aktualizacja: 2004-10-20 Stanisław Paśko (sp@mimuw.edu.pl)
    Aktualizacja: 2005-10-22 Piotr Malinowski (malinex@mimuw.edu.pl)
    Aktualizacja: 2006-11-16 Radek Bartosiak (kedar@mimuw.edu.pl) linux 2.6
    Aktualizacja: 2012-03-18 Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
    Aktualizacja: 2013-04-02 Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
    ==============================================================================
