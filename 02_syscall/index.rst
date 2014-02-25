===========================
Zajęcia 2: interfejsy jądra
===========================

.. toctree::
   :hidden:

   zadanie


Materiały dodatkowe
===================

- :ref:`02-zadanie`


Wywołania systemowe w systemie Linux
====================================

Co to jest wywołanie systemowe?
-------------------------------

Wywołania systemowe (syscalle) są mechanizmem pozwalającym procesom
działającym w przestrzeni użytkownika zażądać wykonania pewnych funkcji
przez jądro systemu.

Syscalle są głównym mechanizmem komunikacji procesów użytkownika ze
światem zewnętrznym - jedyne, co może zrobić program nie używający syscalli
to zawieszenie się lub spowodowanie błędu wykonania.

Mechanizm wywoływania syscalli z poziomu użytkownika jest różny dla każdej
architektury, a czasem nawet dla procesorów i/lub wersji jądra w ramach
jednej architektury. Zazwyczaj polega to na wykonaniu specjalnie do tego
celu zaprojektowanej instrukcji assemblera.

Syscalle identyfikowane są numerami. Ich dokładna lista oraz numerowanie
zależy od architektury - można ją obejrzeć w pliku ``/usr/include/asm/unistd.h``
lub ``arch/*/include/asm/unistd.h``

Przed wywołaniem syscalla, należy umieścić jego numer oraz parametry
w pewnych ustalonych rejestrach procesora. Po wywołaniu, wynik lub kod
błędu jest również dostępny w rejestrze.

Wartość zwracana z jądra w zakresie [-4095,-1] oznacza błąd i jest
zanegowanym standardowym kodem błędu (lista w ``asm-generic/errno.h``
i ``asm-generic/errno-base.h``). Pozostałe wartości oznaczają sukces, a ich
znaczenie zależy od syscalla.

Wywołania systemowe wykonywane przez program można podejrzeć używając
programu strace (np. ``strace ls``). Należy zauważyć, że strace pokazuje
wszystkie syscalle wykonywane przez proces - w tym te używane przez
dynamiczny linker w celu załadowania programu i jego bibliotek.

Wywołania systemowe na architekturze x86
----------------------------------------

Istnieją 3 mechanizmy wywołań systemowych na architekturze x86:

- przerwanie ``0x80`` (dostępne na wszystkich procesorach)
- instrukcja ``sysenter`` (dostępne na procesorach intela od Pentium Pro)
- instrukcja ``syscall`` (dostępne na procesorach AMD od K6)

Instrukcje sysenter/syscall zostały wprowadzone w późniejszych procesorach
ze względu na kiepską wydajność przerwań na procesorach x86.

W przypadku użycia przerwania 0x80, wywołanie systemowe wygląda następująco:

- numer syscalla przekazywany jest w ``eax``
- parametry przekazywane są w rejestrach: ``ebx``, ``ecx``, ``edx``, ``esi``,
  ``edi``, ``ebp`` (w tej kolejności); syscalle wymagające więcej parametrów
  mają specjalne konwencje
- syscall jest wywoływany przez instrukcję ``int $0x80``
- wynik syscalla znajdzie się w rejestrze ``eax``

Wywołania przez ``syscall``/``sysenter`` są dość podobne, lecz trochę bardziej
skomplikowane.

Wywołania systemowe na architekturze x86_64
-------------------------------------------

Jedynym natywnym 64-bitowym mechanizmem wywołań systemowych na architektrze
x86_64 jest instrukcja ``syscall``. Wywołanie odbywa się następująco:

- numer syscalla przekazywany jest w ``rax``
- parametry przekazywane są w: ``rdi``, ``rsi``, ``rdx``, ``r10``, ``r8``, ``r9``
- wywoływana jest instrukcja ``syscall``
- zawartość ``rcx`` i ``r11`` jest zniszczona przez jądro jako skutek uboczny
  syscalla
- wynik znajduje się w ``rax``

Mechanizmy VDSO oraz vsyscall
-----------------------------

Ze względu na istnienie wielu mechanizmów syscalli na architekturze x86
i potrzebę wybrania odpowiedniego dla danej maszyny, wprowadzono mechanizm
VDSO. VDSO jest małą biblioteką dzieloną dostarczaną przez jądro,
zawierającą odpowiednią dla danego procesora funkcję wywołującą syscalla.
Jądro ma przygotowane kilka wersji tej biblioteki (``int 0x80``, ``syscall``,
``sysenter``) i wybiera odpowiednią w czasie działania.

Architektura x86_64 nie wymaga wyboru mechanizmu syscalli, lecz wprowadzono
na niej ulepszony mechanizm wykonania pewnych syscalli (``clock``, ``time``,
``get_cpu``). Te syscalle mają zoptymalizowane wersje nie wymagające przejścia
procesora w tryb jądra (czytają one jedynie zmienne globalne jądra,
w specjalny sposób udostępnione do odczytu przestrzeni użytkownika).
Ten mechanizm również używa bloku kodu eksportowanego przez jądro do
przestrzeni użytkownika, nazwanego vsyscall.

Kod VDSO oraz vsyscall zawiera również implementację funkcji ``sigreturn``
oraz ``rt_sigreturn``, używanych przy powracaniu z funkcji obsługi sygnałów
w przestrzeni użytkownika.

Wywołania systemowe w libc
==========================

Większość wywołań systemowych ma swoje "opakowania" (wrappery)
w standardowej bibliotece C (libc). Są to funkcje, których jedynym
zadaniem jest przeniesienie parametrów w odpowiednie miejsce, wywołanie
odpowiedniego syscalla, i zwrócenie wyniku. Należy zauważyć, że jądro
i libc mają różne konwencje przekazywania informacji o błędzie - jądro
zwraca zanegowany kod błędu (np. ``-EINVAL``) bezpośrednio z syscalla,
podczas gdy funkcje biblioteczne w przypadku błędu zwracają zawsze -1,
a kod błędu przekazują w zmiennej globalnej errno (kod błędu w errno
*nie* jest zanegowany).

Uproszczona wersja opakowania syscalla write (pomijam vdso, wielowątkowość
errno, cancellation point) może wyglądać na przykład tak::

    .global write
    write:
    pushl %ebx              # zachowanie ebx
    movl $4, %eax           # numer syscalla
    movl 8(%esp), %ebx      # parametr 1 - fd
    movl 12(%esp), %ecx     # parametr 2 - buf
    movl 16(%esp), %edx     # parametr 3 - len
    int $0x80               # syscall
    cmpl $-4096, %eax       # czy błąd?
    jna out                 # jak nie błąd, to wyjście
    neg %eax                # -EINVAL -> EINVAL etc.
    movl %eax, errno        # ustawienie errno
    movl $-1, %eax          # wartość zwracana to -1
    out:
    popl %ebx               # odtworzenie ebx
    ret

Nie wszystkie wywołania systemowe odpowiadają bezpośrednio funkcjom
bibliotecznym, z wielu powodów:
 
- wiele syscalli ma kilka wersji z parametrami różnych wielkości (głównie
  te dotyczące uidów/gidów, pidów, offsetów w plikach, etc.). Starsze
  wersje z mniejszymi parametrami są zachowywane w ramach zgodności ze
  starszymi wersjami libc. Przykładami są syscall ``getuid`` (16-bitowy uid)
  i ``getuid32`` (32-bitowy uid) oraz ``lseek`` (32-bitowy offset)
  i ``_llseek`` (64-bitowy offset). Istniejące wersje syscalli zależą mocno
  od architektury - np. 64-bitowe architektury nigdy nie miały syscalli
  z 32-bitowymi offsetami.
- niektóre syscalle (``ipc``, ``socketcall``) w rzeczywistości mają wiele
  podfunkcji z różnymi parametrami (``shmat``, ``shmctl``, ``msgctl``,
  ``socket``, ``connect``, ``bind``, ``listen`` ...). Każda z tych podfunkcji
  ma swoją własną funkcję w libc.
- wiele syscalli ma semantykę zmodyfikowaną przez bibliotekę wątków
  (o tym poniżej)
- syscall wymaga specjalnej interwencji ze strony libc (``vfork``, ``clone``,
  etc.)
- bo tak

Istnieje również funkcja syscall, pozwalająca wykonać bezpośrednio dowolne
wywołanie systemowe. Przydaje się ona przy wywoływaniu syscalli nie
posiadających własnych wrapperów w libc. Implementacja write używająca
tej funkcji może wyglądać na przykład tak::

    ssize_t write(int fd, const char *buf, size_t len) {
            return syscall(SYS_write, fd, buf, len);
    }

Przegląd ważniejszych syscalli
==============================

Kontrola procesów
-----------------

Te syscalle związane są z zarządzaniem procesami. Należy zauważyć, że
jądro Linuxa oraz standard POSIX (w tym biblioteka wątków pthreads)
używają różnych definicji procesu: procesy w jądrze odpowiadają POSIXowym
wątkom. Tutaj używamy POSIXowej definicji.

``noreturn _exit()``
  kończy wątek
``noreturn exit_group()``
  kończy proces
``pid_t getpid()``
  zwraca identyfikator obecnego procesu
``pid_t gettid()``
  zwraca identyfikator obecnego wątku
``int fork()``
  tworzy nowy proces będący kopią obecnego (ale tylko
  z jednym wątkiem); jest to specjalny przypadek syscalla clone
``int clone(int (*fn)(void *), void *newstack, int flags, void *arg, ...)``
  tworzy nowy proces lub wątek, dość skomplikowana funkcja
``pid_t waitpid(pid_t pid, int *stat_loc, int options)``
   czeka na zdarzenie (wyjście, zatrzymanie, etc.) w procesie potomnym
``int execve(const char *path, char *const *argv, char *const *envp)``
  uruchamia nowy program w obecnym procesie, zastępując obecny
``long ptrace(int request, pid_t pid, void *addr, void *data)``
  wykonuje wiele operacji związanych ze śledzeniem innych procesów: pozwala
  zatrzymać dany proces, wykonywać go instrukcja po instrukcji, czytać
  i pisać jego przestrzeń adresową oraz rejestry, itp.. Używany m.in.
  przez gdb oraz strace.

Obsługa plików
--------------

Na poziomie syscalli, otwarte pliki są identyfikowane tzw. deskryptorami
plików, czyli małymi dodatnimi liczbami całkowitymi. Deskryptory 0-2
standardowo odpowiadają standardowemu wejściu, wyjściu, oraz wyjściu
błędów. Pozostałe deskryptory rzadko mają konkretnie zdefiniowane role.

Ważniejsze syscalle z tej grupy:

``ssize_t read(int fd, void *buf, size_t len)``
  czyta z pliku fd do bufora; zwraca ilość przeczytanych bajtów. 0 oznacza
  koniec pliku (nie jest to uznawane za błąd). Liczba dodatnia, ale mniejsza
  niż len oznacza częściowy odczyt - może to być spowodowane błędem lub końcem
  pliku na tej pozycji, lub po prostu brakiem większej ilości dostępnych
  danych w danym momencie.
``ssize_t write(int fd, const void *buf, size_t len)``
  pisze do pliku, działa podobnie do read
``int open(const char *fname, int flags, mode_t mode)``
  otwiera plik, zwraca deskryptor
``int close(int fd)``
  zamyka plik; może zwrócić błąd w przypadku problemu w opróżnianiu buforów
  jądra
``int ioctl(int fd, int request, void *arg)``
  wykonanie specjalnej operacji na pliku. Zestaw dostępnych operacji jest
  bardzo zależny od pliku. W większości stosuje się tylko do plików będących
  urządzeniami. Przykładowe operacje specjalne:

  - zmiana ustawień terminala (wykonywane na pliku terminala)
  - zmiana głośności (wykonywane na pliku urządzenia karty dźwiękowej)
  - przeczytanie informacji o producencie i parametrach fizycznych
    (wykonywane na pliku urządzenia dysku twardego)

``int poll(struct pollfd *fds, int nfds, int timeout)``
  czeka na zajście jednego z podanych zdarzeń na jednym z podanej listy
  deskryptorów, przydatne gdy program może dostawać wejście z wielu źródeł

Obsługa pamięci
---------------

``void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)``
  tworzy nowy obszar pamięci. Jeśli ``flags`` zawiera ``MAP_ANONYMOUS``, jest to
  po prostu nowy blok pamięci. W przeciwnym wypadku, obszar będzie podłączony
  do podanego pliku - czytanie z obszaru da nam zawartość pliku. Jeśli ``flags``
  zawiera ``MAP_SHARED``, zapis do obszaru spowoduje również zapis do pliku;
  w przeciwnym przypadku (``MAP_PRIVATE``), zapis do obszaru spowoduje stworzenie
  nowej kopii danych z pliku i modyfikację tylko tej kopii.  Jeśli przekazana
  jest flaga ``MAP_FIXED``, obszar znajdzie się pod zadanym adresem; w przeciwnym
  przypadku jądro poszuka jakiegoś wolnego adresu.
``void munmap(void *addr, size_t len)``
  kasuje podany obszar pamięci
``int mprotect(void *addr, size_t len, int prot)``
  zmienia prawa dostępu do danego obszaru pamięci
``void *brk(void *addr)``
  skraca lub rozszerza segment sterty procesu

Należy zauważyć, że w systemie Linux (jak i wielu innych UNIXach) istnieją
dwie metody na alokację "zwykłej" pamięci: ``mmap`` z opcją ``MAP_ANONYMOUS`` oraz
``brk``. Zwykły ``malloc()`` z libc w standardowej konfiguracji używa tej drugiej.

futex
-----

Tradycyjne metody implementacji mutexów w przestrzeni użytkownika wymagały,
dla uniknięcia aktywnego oczekiwania, utworzenia np. nienazwanego pipe'a
dla każdego mutexu, używanego do obudzenia czekających procesów. Takie
podejście ma szereg wad:

- mutexy są drogie: użycie dwóch deskryptorów plików na każdy mutex
  wymagający aktywnego oczekiwania, sama struktura mutex również musi być
  całkiem duża
- implementacja mutexów międzyprocesowych (np. w pamięci współdzielonej)
  jest bardzo ciężka

W jądrze 2.6 dodano syscall ``futex`` (fast userspace mutex), pozwalający na
znaczne uproszczenie implementacji mutexów::

  int futex(int *uaddr, int op, int val, const struct timespec *timeout, int *uaddr2, int val3);

Ten syscall, podobnie jak ``socketcall``, jest w rzeczywistości opakowaniem
na kilka podfunkcji (wybieranych przez parametr op):

- ``FUTEX_WAIT``: atomowo sprawdza, czy ``*uaddr == val`` i zasypia, jeśli tak.
  Jeśli timeout jest podany, zasypia na co najwyżej taki okres czasu,
  w przeciwnym wypadku zasypia na czas nieokreślony.
- ``FUTEX_WAKE``: budzi co najwyżej ``val`` procesów czekających przez
  ``FUTEX_WAIT`` na adresie ``uaddr``.
- ... i kilka innych, bardziej skomplikowanych

Użycie futexów pozwala na uniknięcie problemów z tradycyjną implementacją:

- syscall ``futex`` jest wywoływany tylko, gdy mutex jest już zajęty. Nie ma
  stałego zużycia zasobów: zasoby jądra są wykorzystywane tylko wtedy, gdy
  wątek faktycznie czeka na zwolnienie mutexa
- mutex jest bardzo małą strukturą (dla podstawowego wariantu wystarczy
  pojedynczy ``int``)
- mutexy działają między procesami bez żadnej specjalnej obsługi - jądro
  używa fizycznego adresu przy porównaniach i poprawnie obsłuży odwołania
  do tego samego miejsca przez różne adresy w różnych procesach itp.

Sygnały
=======

Sygnały są mechanizmem przekazywania informacji o asynchronicznych lub
synchronicznych zdarzeniach do procesu użytkownika. Mechanizm ten jest
bardzo podobny do mechanizmu przerwań na poziomie jądra.

Istnieje około 32 sygnałów o ustalonym przeznaczeniu (lista jest w pewnym
stopniu zależna od architektury), oraz 32 sygnałów czasu rzeczywistego,
które mogą być wykorzystane przez użytkownika do dowolnego celu.
Ważniejsze sygnały to:

Sygnały wysyłane przez jądro, powodowane przez błędy procesora:

- ``SIGSEGV``: informuje o naruszeniu mechanizmów ochrony, najczęściej
  odwołanie do złego obszaru pamięci
- ``SIGILL``: informuje o wykonaniu nieprawidłowej instrukcji maszynowej
- ``SIGBUS``: informuje o błędzie dostępu do pamięci z innego powodu niż
  nieprawidłowy adres czy brak uprawnień. Dość ciężko go otrzymać
  na x86. Na innych architekturach często powodowany np. przez dostęp
  do niewyrównanego adresu słowa.
- ``SIGFPE``: floating point exception, oryginalnie informował o błędzie
  w obliczeniach zmiennoprzecinkowych, później wykorzystany również
  do błędów arytmetycznych na liczbach całkowitych (dzielenie przez 0)
- ``SIGTRAP``: informuje o trafieniu w breakpoint, wykorzystywany przy
  debugowaniu programów

Sygnały służące sterowaniu procesami (wysyłane przez inne procesy):

- ``SIGTERM``: informuje proces, że ma zakończyć działanie
- ``SIGKILL``: siłowo kończy działanie procesu
- ``SIGSTOP``: zatrzymuje działanie procesu (z możliwością kontynuacji)
- ``SIGCONT``: kontynuuje wykonanie procesu
- ``SIGCHLD``: informuje o zmianie stanu procesu potomnego

Sygnały związane z obsługą terminali:

- ``SIGHUP``: informuje o odłączeniu się terminala (np. zamknięcie okna
  xterm, przerwanie sesji ssh)
- ``SIGINT``: informuje o wciśnięciu Ctrl-C
- ``SIGQUIT``: informuje o wciśnięciu Ctrl-\
- ``SIGTSTP``: informuje o wciśnięciu Ctrl-Z
- ``SIGTTIN``: informuje o próbie czytania z terminala kontrolującego
  bez bycia w grupie pierwszego planu
- ``SIGTTOU``: informuje o próbie pisania do terminala kontrolującego
  bez bycia w grupie pierwszego planu
- ``SIGWINCH``: informuje o zmianie rozmiaru terminala

Inne sygnały:

- ``SIGABRT``: informuje o wystąpieniu nieprzewidzianego błędu w programie
  (nieudany assert itp.) i konieczności jego siłowego zamknięcia
- ``SIGPIPE``: informuje o próbie pisania do pipe'a lub socketa, którego
  drugi koniec został zamknięty
- ``SIGUSR1``, ``SIGUSR2``: bez ustalonego przeznaczenia, przeznaczone dla
  użytkownika
- ``SIGIO``: informuje o zakończeniu asynchronicznego IO, lub możliwości
  wykonania IO, jeśli program zażądał wcześniej takiej informacji

Każdy sygnał ma przypisaną akcję, która będzie wykonana gdy zostanie
on dostarczony. Jest to jedna z:

- ignorowanie: nic się nie stanie
- wykonanie funkcji: zostanie wykonana funkcja dostarczona przez
  użytkownika
- akcja domyślna, zależna od sygnału:
  - ignorowanie (``SIGCHLD``, ``SIGWINCH``)
  - zatrzymanie procesu (``SIGSTOP``, ``SIGTSTP``, ``SIGTTIN``, ``SIGTTOU``)
  - kontynuacja procesu (``SIGCONT``)
  - zabicie procesu (``SIGTERM``, ``SIGKILL``, ``SIGINT``, ...)
  - zabicie procesu ze zrzutem pamięci (``SIGSEGV``, ``SIGQUIT``, ...)

Akcja przypsana większości sygnałów może być zmieniona syscallami
``signal`` (prosty interfejs, lecz małe możliwości) lub ``sigaction`` (znacznie
większe możliwości). Sygnały, których akcji nie można zmienić, to ``SIGKILL``
oraz ``SIGSTOP``. Ponadto, choć można zmienić akcję ``SIGCONT``, i tak będzie on
powodował kontynuację procesu jako dodatek do wywołania przypisanej akcji.

Oprócz zmiany akcji przypisanej sygnałom, można również zablokować ich
dostarczanie syscallem ``sigprocmask``. Sygnał blokowany nie jest tym samym
co sygnał ignorowany - sygnał ignorowany zostanie wyrzucony, podczas gdy
sygnał blokowany będzie czekał w kolejce do momentu odblokowania.

Oprócz sygnałów wysyłanych przez jądro, każdy sygnał może być również
ręcznie wysłany przez użytkownika. Sygnały mogą być wysyłane do całych
procesów (lub grup procesów) przez syscall ``kill``, lub do pojedynczych
wątków wewnątrz własnego procesu przez syscall ``tkill`` (lub odpowiadającą
mu funkcję ``pthread_kill``). Wysłanie sygnału do procesu powoduje
dostarczenie go do dowolnie wybranego wątku w tym procesie.

Obsługa sygnałów
----------------

Funkcje obsługi sygnałów są ustawiane przez syscall ``sigaction`` i mają
jeden z następujących typów:

 - ``void func(int signum)``
 - ``void func(int signum, siginfo_t *info, ucontext_t *ctx)``

``signum`` jest numerem sygnału, który przyszedł. W przypadku użycia drugiego
typu, ``info`` jest strukturą zawierającą informację o sygnale - np. jego
źródło (błąd procesora, wysłanie przez użytkownika, terminal, itp.)
i szczegóły (adres błędu w przypadku ``SIGSEGV``, pid procesu wysyłającego,
itp.). ``ctx`` jest wskaźnikiem na strukturę zawierającą pełen stan rejestrów
procesora przed wejściem do procedury obsługi sygnału.

W momencie nadejścia sygnału z przypisaną funkcją obsługi, jądro przerywa
pracę danego procesu. Jeśli proces wykonuje akurat blokujące wywołanie
systemowe i jest w stanie przerywalnego snu, zostaje ono przerwane.
W zależności od semantyki wybranej przy ustawianiu funkcji obsługi,
tak przerwane wywołanie albo zwraca kod błędu ``EINTR``, albo zostanie
zrestartowane przy powrocie z obsługi sygnału.

Po przerwaniu pracy procesu i ewentualnego syscalla, jądro zapisuje stan
rejestrów procesora na stos użytkownika (zazwyczaj jest to "zwykły" stos
programu, ale możliwe jest ustawienie oddzielnego stosu na sygnały przez
syscall ``sigaltatack``). Po zapisaniu stanu rejestrów, jądro zapisuje na
stosie (lub w rejestrach) parametry do funkcji obsługi przerwania oraz
adres powrotny. Ze względu na konieczność posprzątania po obsłudze sygnału
i przywrócenia dokładnego stanu procesora sprzed jej wywołania, ten adres
powrotu wskazuje na specjalną pseudo-funkcję ``sigreturn`` będącą częścią
bloku VDSO/vsyscall. Po powrocie z funkcji obsługi sygnału, ``sigreturn``
wywołuje syscall ``sigreturn``, który zajmuje się właściwym sprzątaniem.

Zwykły powrót z funkcji obsługi sygnału nie jest jedyną metodą jej
opuszczenia - niekiedy przydatne jest użycie wywołania ``siglongjmp`` lub
wykorzystanie informacji ze struktury ucontext do odwinięcia stosu.

Obsługa sygnałów to bardzo delikatny mechanizm, gdyż bardzo ciężko
jest kontrolować, w którym miejscu kodu przyjdzie sygnał (np. nie możemy
zagwarantować, że wolno nam użyć malloca w obsłudze sygnału - ten sygnał
mógł przerwać wywołanie malloca z głównego programu). Z tego powodu
funkcje obsługi sygnałów często ograniczają się do ustawienia pojedynczej
zmiennej, która jest regularnie sprawdzana przez główny program.


Literatura
----------

1. Sekcja 2 manuala, szczególnie: syscall, futex, sigaction
2. Żródła VDSO/vsyscall w arch/x86/vdso
3. Lista syscalli w asm/unistd.h
4. Ulrich Drepper "Futexes Are Tricky", 2011 - `http://www.akkadia.org/drepper/futex.pdf`
5. man 7 signal

.. Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
.. 24.02.2013, 25.02.2013
