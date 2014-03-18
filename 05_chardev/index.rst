=============================
Zajęcia 5: Urządzenia znakowe
=============================

Data: 18.03.2014, 19.03.2014

.. contents::

.. toctree::
   :hidden:

   p1_yatb/index
   zadanie



Materiały dodatkowe
===================

- :ref:`05-p1-yatb`
- :ref:`05-zadanie`


Krótkie wprowadzenie do programowania w przestrzeni adresowej jądra
===================================================================

Informacje o procesie
---------------------

Podczas pisania kodu, który będzie się wykonywał w przestrzeni
adresowej jądra należy cały czas pamiętać, że wątek wykonania jest
związany z pewnym procesem użytkownika, w imieniu którego jądro
wykonuje określone operacje. Korzystając z makra ``current``
(``asm/current.h``) możemy łatwo (i szybko) dotrzeć do wszystkich
informacji jakie jądro przechowuje o bieżącym procesie w strukturze
task_struct (``linux/sched.h``).

Wyjątkiem jest obsługa przerwań - o ile w trakcie wykonywania funkcji
obsługi przerwania sprzętowego, makro ``current`` może wskazywać na jakiś
proces, to nie należy się do niego odwoływać (nie ma on związku z tym
przerwaniem) jak również nie wolno przełączać się na inne procesy
(czyli nie można też wykonywać operacji blokujących).

Ćwiczenie:

Proszę zapoznać się z definicją struktury task_struct i spróbować wyjaśnić
znaczenie innych jej pól (``linux/sched.h``).

Proste blokady
--------------

Ze względu na częstą w jądrze konieczność używania danych wspólnych dla
wielu procesów, jedną z najważniejszych klas funkcji w jądrze stanowią
funkcję synchronizujące wywołanie procesów, m.in. blokady.

Najprostszym rodzaj blokad są zwykłe blokady (nazywane też mutexami).
Ten rozaj blokad jest zdefiniowany w ``linux/mutex.h``. Taka blokada
nie jest rekurencyjna i musi zostać zwolniona przez proces, którą ją
założył. Proces próbujący zablokować już zablokowaną blokadę będzie
spał (przechodząc w stan S lub D) aż blokada zostanie zwolniona.

Blokady tworzy się w jeden z następujących sposobów::

   /* dla blokad będących zmiennymi globalnymi */
   static DEFINE_MUTEX(blokada);

::

    /* dla blokad w strukturach alokowanych dynamicznie */
    struct mutex blokada2;
    /* ... */
    mutex_init(&blokada2);

Dostępne są między innymi następujące operacje::

    void mutex_lock(struct mutex *lock);
    int mutex_lock_interruptible(struct mutex *lock);
    int mutex_lock_killable(struct mutex *lock);
    int mutex_trylock(struct mutex *lock);

Funkcje zakładające blokadę. Pierwsza nie przyjmuje żadnych sygnałów
w czasie ewentualnego oczekiwania na zwolnienie blokady. Druga pozwala
na odebranie sygnału (zwraca ``-EINTR`` w przypadku pobudki wywołanej
sygnałem, ``0`` wpp.). Trzecia działa jak druga, ale jest przerywalna tylko
przez sygnału, które spowodują zabicie procesu. Trzecia funkcja nie
oczekuje na zwolnienie blokady - w przypadku gdy blokada jest już
zablokowana, zwraca ``-EAGAIN`` i nic nie robi, w przypadku udanego
zablokowania zwraca ``0``.

::

    void mutex_unlock(struct mutex *lock);

Zwalnia blokadę i budzi jeden z ewentualnych procesów oczekujących.

Pozostałe typy blokad i inne funkcje synchronizujące będą omówione
na następnych zajęciach.


Wymiana danych między przestrzenią adresową użytkownika i jądra
---------------------------------------------------------------

Aby z poziomu jądra odczytać/zapisać coś z/do przestrzeni pamięci
programów użytkownika należy posłużyć się następującymi funkcjami
(właściwie makrami):

``put_user(kptr, ptr)``
    wpisanie bajtu/słowa/długiego słowa do pamięci programów użytkownika
    (spod adresu ``ptr``); makrodefinicja działa automagicznie - rozmiar
    określony jest na podstawie typu, na który wskazuje ``kptr``

``get_user(kptr, ptr)``
    j.w., ale odczytanie

Do kopiowania większych obszarów pamięci służą funkcje::

    unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
    unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);

Pierwsza umożliwia kopiowanie danych z przestrzeni adresowej
użytkownika do przestrzeni adresowej jądra, druga odwrotnie. Ogólnie
zachowują się jak ``memcpy``, jednak trzeba pamiętać, że w przypadku błędu
braku strony adresu w przestrzeni użytkownika mogą spowodować uśpienie
procesu aż do momentu ściągnięcia strony z pliku wymiany. Przed
kopiowaniem sprawdzana jest poprawność adresu w przestrzeni
użytkownika. Jeśli początek obszaru jest poprawny, ale dalsza część
nie, to kopiowany jest najdłuższy możliwy fragment.

Wartością zwracaną przez obie funkcje jest liczba NIE skopiowanych
bajtów - niezerowa oznacza wystąpienie błędu przy kopiowaniu.

Funkcje i odpowiadające im makrodefinicje zdefiniowane są w pliku
``asm/uaccess.h``. Należy zwrócić uwagę, iż funkcje dla bloków o
rozmiarze potęg dwójki są zoptymalizowane.


Wykorzystanie standardowej implementacji list
---------------------------------------------

Jądro zawiera efektywną implementację list dwukierunkowych. Lista
składa się z cyklicznie połączonych struktur ``list_head``, najczęściej
będących składowymi jakiejś większej struktury. Należy zauważyć,
że zarówno głowa listy jak i jej elementy są strukturami ``list_head`` -
wykrywanie końca listy następuje przez porównanie z adresem głowy.

Wszystkie operacje na listach dostępne są poprzez plik ``linux/list.h``.
Oto garść pojęć związanych z listami:

``list_head``
  struktura reprezentująca głowę (cząstkę) listy
``LIST_HEAD(lista)``
  makro definiujące i inicjalizujące zmienną z głową listy
``list_add(co, do_czego)``
  dodanie ``co`` na początek ``do_czego``
``list_add_tail(co, do_czego)``
  dodanie ``co`` na koniec ``do_czego``
``list_del(co)``
  usunięcie ``co`` z listy
``list_empty(lista)``
  sprawdzenie, czy lista jest pusta
``list_splice(co, przed_co)``
  sklejenie listy ``co`` oraz ``przed_co``
``list_for_each(kazdy, po_liscie)``
  iteracja zmiennej ``kazdy`` po każdym elemencie ``po_liscie``
``list_for_each_safe(kazdy, tymczasem, po_liscie)``
  iteracja zmiennej ``kazdy`` po każdym elemencie ``po_liscie`` w sposób
  bezpieczny ze względu na usuwanie elementów listy (w tym celu
  wykorzystuje się zmienną ``tymczasem``)
``list_entry(moja_lista, struktura, pole)``
  wyliczenie wskaźnika na początek struktury, ktorej pole typu ``list_head``
  jest ``moja_lista``

Po szczegóły odsyłam do kodu źródłowego.


Sterowniki urządzeń znakowych w Linuksie
========================================

O sterownikach urządzeń
-----------------------

Sterownik (podprogram obsługi) urządzenia to zbiór funkcji służących
zazwyczaj do komunikacji z urządzeniem zewnętrznym. Żeby umożliwić
programom użytkownika prosty mechanizm operacji na urządzeniu, sterownik
jest reprezentowany przez specjalny i-węzeł (plik) w systemie plików
(zazwyczaj w katalogu ``/dev``). Operacje na tym pliku (otwarcie, odczyt,
zapis, zamkniecie,...) są obsługiwane właśnie przez podprogram obsługi.
Przykładowo odczyt z tego pliku jest realizowany przez funkcję odczytu
dostarczaną przez sterownik, a nie tę standardową dla systemu plików.

Rodzaje (sterowników) urządzeń
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- urządzenia znakowe (character devices) - umożliwiają dostęp do danych
  w dowolny sposób (syscalle wywoływane na pliku są przekazywane
  mniej-więcej bezpośrednio do funkcji sterownika)
- urządzenia blokowe (block devices) - umożliwiają dostęp do danych w
  porcjach (blokach), operacje na pliku przechodzą przez podsystem
  blokowy - używane do dysków twardych, dyskietek, płyt CD, pamięci
  flash, i innych podobnych urzadzeń.

Reprezentacja plikowa
~~~~~~~~~~~~~~~~~~~~~

Plik specjalny (i-węzeł) urządzenia w systemie plików zawiera m. in.
następujące informacje:

- flagę określająca typ urządzenia (``b`` - blokowe, ``c`` - znakowe)
- numer główny (major device number) - indeks podprogramu
  danego urządzenia w tablicy rozdzielczej
- numer drugorzędny (minor device number) - liczba przekazywana
  podprogramowi obsługi podczas operacji na urządzeniu

Zwykle numer drugorzędny jest używany w podprogramie obsługi urządzenia
dla rozróżniania urządzeń obsługiwanych przez ten podprogram. W pliku
``Documentation/devices.txt`` (w źródłach jądra) znajduje się lista numerów
przypisanych standardowym urządzeniom.

Przydatne makra (zdefiniowane w pliku ``linux/kdev_t.h``):

``MAJOR(inode->i_rdev)``
  zwraca numer główny urządzenia związanego z i-węzłem.
``MINOR(inode->i_rdev)``
  zwraca odpowiedni numer drugorzędny.

Tablice rozdzielcze sterowników
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W systemie istnieją dwie tablice podprogramów obsługi urządzeń jedna
dla urządzeń blokowych, druga dla znakowych. Tablice te nazywane są
tablicami rozdzielczymi. Każdemu podprogramowi obsługi odpowiada
pozycja w odpowiedniej tablicy. Podprogram obsługi jest identyfikowany
parą (rodzaj, liczba). Rodzaj określa, w której z dwóch tablic, a
liczba, pod którym indeksem, znajduje się opisująca go struktura
``device_struct``.  W strukturze tej znajduje się wskaźnik do nazwy
podprogramu obsługi oraz wskaźnik do odpowiadającej mu struktury
operacji na urządzeniu.

Dołączanie nowych sterowników do systemu
----------------------------------------

Numery główne (zamówienie lub przydział dynamiczny)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pliki specjalne, reprezentujące urządzenia, są zazwyczaj umieszczone w
katalogu ``/dev``. Wywołując ``ls -l`` w tym katalogu można zobaczyć listę
dostępnych plików specjalnych. Literka ``c`` lub ``b`` przed listą
uprawnień określa rodzaj urządzenia. Dwa numery, tuż przed datą
ostatniej modyfikacji, oznaczają odpowiednio numer główny (major) i
drugorzędny (minor) urządzenia.

Komendą tworzącą plik specjalny, który będzie reprezentować urządzenie,
jest mknod. Przy wywołaniu należy  podać 4 argumenty::

    mknod /dev/nazwa_pliku typ major minor

gdzie typ to literka ``c`` lub ``b``, a major i minor to odpowiednie numery
związane z urządzeniem. Plik specjalny pozostaje w systemie plików aż
do skasowania go komendą ``rm``.

Dodanie nowego sterownika urządzenia wiąże się z przypisaniem mu numeru
głównego (major).
W kodzie jądra typ ``dev_t`` (``linux/types.h``) jest używany do przechowywania
zarówno numeru głównego jak i numeru drugorzędnego. W pliku ``linux/kdev_t.h``
znajduje się zbiór przydatnych makr, z których wymienimy tylko:

``MAJOR(dev_t)``
  zwraca numer główny
``MINOR(dev_t)``
  zwraca numer poboczny
``MKDEV(int major, int minor)``
  koduje numer główny i poboczny w wartość typu ``dev_t``.


Ponieważ wiele numerów głównych jest już przypisanych istniejącym
urządzeniom, wybranie nieprzydzielonego numeru dla nowego urządzenia
może być zadaniem trudnym (mimo, że w jądrze 2.6 zwiększono liczbę
dostępnych numerów). Indeks zarejestrowanych numerów znajduje się
w pliku ``Documentation/devices.txt`` w dystrybucji Linuksa. Jednak
najczęściej korzysta się z możliwości dynamicznego przydziału numeru
głównego. Można to osiągnać wpisując przy rejestracji sterownika ``0``
zamiast właściwego numeru głównego. Przydzielony numer zostanie zwrócony
przez funkcję rejestrującą.

W czasach sprzed dynamicznego przydziału numerów tworzenie plików urządzeń
było zadaniem użytkownika - pliki te były zazwyczaj tworzone "na zapas"
(np. pliki dla 4 dysków po 63 partycje, mimo użycia tylko kilku) przez
twórcę dystrybucji. We współczesnych systemach pliki te są zazwyczaj
tworzone na żądanie (przy załadowaniu sterownika do urządzenia) przez
program ``udev``, otrzymujący informacje o nowych urządzeniach od jądra.
Aby zostać zauważonym przez ``udev``, sterownik urządzenia musi zarejestrować
swoje urządzenie w hierarchii sysfs (będzie o tym na następnych
zajęciach).


Rejestracja (sterowników) urządzeń
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Do rejestracji (sterowników) urządzeń znakowych i blokowych służą
odpowiednio::

    int register_chrdev_region(dev_t first, unsigned int count, const char
        *name);
    int register_chrdev(unsigned int major, const char * name, struct
        file_operations *fops)
    int register_blkdev(unsigned int major, const char * name, struct
        block_device_operations *bdops);

Operacje te rejestrują urządzenie znakowe/blokowe o numerze głównym
``major``, nazwie ``name`` i operacjach na urządzeniu ``fops``/``bdops``.
Nazwa to napis wyświetlany w pliku ``/proc/devices``.
Pierwsza funkcja pozwala na rejestrację urządzenia znakowego począwszy
od ``first`` (kodującego ``major`` i ``minor``). Jeżeli wielkość przedziału ``count`` jest
duża to może on obejmować także następny numer główny (major).


Wyrejestrowanie sterownika
~~~~~~~~~~~~~~~~~~~~~~~~~~

Do wyrejestrowywania (sterowników) urządzeń służą odpowiednio::

    int unregister_chrdev(int major,char *name)
    int unregister_blkdev(int major,char *name)

Przed wyrejestrowaniem urządzenia sprawdzana jest poprawność obu
argumentów.

.. warning::

    Usunięcie modułu sterownika przed wyrejestrowaniem urządzenia
    może mieć bardzo poważne konsekwencje. Próba obejrzenia ``/proc/devices``
    będzie wywoływała błąd (oops), bo system, starając się wyświetlić nazwy
    urządzeń, będzie odwoływał się do regionów pamięci usuniętego modułu
    (wskaźnik name). Aby unikać takich sytuacji należy wypełniać pole ``owner``
    w strukturze ``file_operations``.

Operacje na pliku specjalnym sterowników urządzeń znakowych
-----------------------------------------------------------

Wykorzystywany jest mechanizm "obiektowy" - sterownik dostarcza operacje
na "sobie samym".

Pola struktury file_operations (operacje dla m.in. urządzeń znakowych)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Struktura ``file_operations`` (zdefiniowana w ``linux/fs.h``) umożliwia
podanie zestawu funkcji, które będą wykonywane przy operacjach na pliku
specjalnym związanym z urządzeniem::

    struct file_operations {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t,
            loff_t *);
        int (*unlocked_ioctl) (struct file *, unsigned int,
            unsigned long);
        int (*compat_ioctl) (struct file *, unsigned int,
            unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*release) (struct inode *, struct file *);

        ... /* niektóre pola zostały pominięte*/
    };

Standardowo pole ``owner`` powinno być zainicjalizowane na ``THIS_MODULE``
(zdefiniowane w ``linux/module.h``)
Umożliwia to jądru automatyczne zarządzanie licznikiem odwołań do modułu.

Pola struktury file
~~~~~~~~~~~~~~~~~~~

Struktura ``file`` (zdefiniowana w ``linux/fs.h``) reprezentuje w jądrze otwarty
plik. Jest tworzona przez jądro w momencie wywołania ``open`` i przekazywana do
wszystkich operacji na pliku, aż do ostatniego wywołania ``close`` (czyli
momentu, kiedy wywołane zostanie ``release``). Warto zauważyć, że otwarty plik
(struktura ``file``) to co innego, niż plik na dysku reprezentowany przez
strukturę ``inode``. ::

    struct file {
        mode_t                  f_mode;
        loff_t                  f_pos;
        unsigned int            f_flags;
        struct file_operations  *f_op;
        void                    *private_data;

        ... /* niektóre pola zostały pominięte*/
    };

Pole ``f_mode`` pozwala określić, czy plik jest otwarty do odczytu
(``FMODE_READ``), zapisu (``FMODE_WRITE``) lub obu. Pola tego nie trzeba
sprawdzać w funkcjach ``read`` i ``write``, bo jądro wykonuje taki test przed
wywołaniem odpowiedniej funkcji sterownika.

Pole ``f_pos`` określa pozycję do pisania lub odczytu. Sterownik może
odczytywać wartość pola, ale nie powinien go zmieniać.

Flagi ``f_flags`` wykorzystuje się głównie do sprawdzenia, czy operacja ma
być blokująca, czy nie (``O_NONBLOCK``).

Pole ``f_op`` określa zestaw funkcji implementujących operacje na pliku.
Pole to jest ustawiane (na standardowe operacje zarejestrowane razem z
urządzeniem) przez jądro raz przy wywołaniu ``open``, a później nie jest ani
modyfikowane, ani sprawdzane. Taka konstrukcja umożliwia zrealizowanie
wielu zachowań sterownika w zależności od numeru drugorzędnego (minor
number) bez narzutu systemowego przy każdym wywołaniu operacji na pliku.

Wskaźnik ``private_data`` jest ustawiany na ``NULL`` przy otwieraniu pliku.
Sterownik może wykorzystać ten wskaźnik dla własnych celów (wtedy jest
odpowiedzialny za zwolnienie pamięci przydzielonej na rzecz tego pola).

Operacja open - otwarcie pliku
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototyp tej funkcji wyglada następująco::

    int open(struct inode *inode, struct file *filp)

Operacja ``open`` umożliwia sterownikowi przeprowadzenie czynności
przygotowawczych przed innymi operacjami. Zazwyczaj wykonuje się
nastepujące kroki:

- sprawdzenie błędów związanych z urządzeniem (np. sprawdzenie, czy
  urządzenie jest gotowe);
- inicjalizacja urządzenia, gdy jest otwierane po raz pierwszy;
- identyfikacja numeru drugorzędnego (``MINOR(inode->i_rdev)``) i, jeśli
  jest to konieczne, podmiana zestawu operacji wskazywanej przez ``f_op``;
- przydzielenie pamięci na dane związane z urządzeniem, inicjalizacja
  struktur danych oraz przypisanie wskaźnika ``private_data``;
- w starszych wersjach jądra zadaniem operacji ``open`` było również
  zwiększenie licznika odwołań do modułu, jednak obligatoryjne obecnie
  ustawienie pola ``owner`` struktury ``file_operations`` (na ``THIS_MODULE``)
  powoduje, że system sam dba o zwiększenie licznika

Operacja release - zamknięcie pliku
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototyp tej funkcji wyglada następująco::

    int release(struct inode *inode, struct file *filp)

Operacja ``release`` (wywoływana przy ostatnim ``close`` na otwartym pliku)
służy do wykonania czynności odwrotnych do tych w operacji open. Zazwyczaj
są to:

- zwolnienie pamięci ``private_data``;
- zamknięcie urządzenia, gdy jest to ostatnie wywołanie ``release``;
- w starszych wersjach jądra zadaniem operacji release było również
  zmniejszenie licznika odwołań do modułu

Operacja read - odczytanie danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototyp tej funkcji wyglada następująco::

    ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp)

Zadaniem operacji read jest przepisanie pewnej porcji danych z przestrzeni
adresowej jądra pod wskazany adres (``buff``) w przestrzeni adresowej
użytkownika (por. opis ``copy_to_user``). Należy również wykonać zwiększenie
znacznika pozycji pliku (``*offp``). Zazwyczaj jest on (ale nie zawsze!)
ustawiony na ten sam adres pamięci, co ``filp->f_pos``.

Wartość zwracana przez tę funkcję będzie interpretowana następująco:

- wartość większa od zera oznacza liczbę przepisanych bajtów; jeśli jest
  równa wartości argumentu przekazanego do wywołania systemowego ``read``, to
  oznacza pełen sukces, jeśli zaś mniejsza, to oznacza, że tylko część
  danych została przekazana - należy się wtedy spodziewać, że program
  powtórzy wywołanie systemowe (takie jest np. standardowe zachowanie
  funkcji bibliotecznej ``fread``)
- jeśli wartość jest równa ``0`` to został osiągnięty koniec pliku;
- wartość ujemna oznacza błąd (por. ``linux/errno.h``)

Operacja write - zapisanie danych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototyp tej funkcji wyglada następująco::

    ssize_t write(struct file *filp, const char __user *buff, size_t count,
                    loff_t *offp)

Zadaniem operacji ``write`` jest przepisanie pewnej porcji danych z
przestrzeni adresowej użytkownika (``buff``) do przestrzeni adresowej jądra
(por. opis ``copy_from_user``), analogicznie do ``read``.

Operacja ``write``, tak jak ``read``, może zapisać mniej danych niż zażądano.
Podobnie jak i ``read``, należy odpowiednio przesunąć pozycję w pliku
(``*offp``).

Wartość zwracana przez tę funkcję będzie interpretowana następująco:

- wartość większa od zera - analogicznie do ``read``;
- wartość ujemna oznacza błąd

Operacja llseek - zmiana pozycji pliku
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototyp tej funkcji wyglada następująco::

    loff_t llseek(struct file *filp, loff_t off, int whence)

Operacja ``llseek`` implementuje wywołania systemowe ``lseek`` i ``llseek``.
Domyślnym działaniem jądra, gdy operacja ``llseek`` nie jest
wyszczególniona w operacjach sterownika, jest zmiana pola ``f_pos``
struktury file. W przypadku, gdy urządzenie nie dostarcza możliwości
zmiany pozycji pliku należy zdefiniować operację pustą. W jądrze jest
w tym celu dostępna gotowa funkcja ``no_llseek``, zawsze zwracająca ``-ESPIPE``.

Operacja ioctl - wywołanie komend specyficznych dla urządzenia
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prototypy funkcji wygladają następująco::

    long (*unlocked_ioctl) (struct file *filp, unsigned int cmd,
                    unsigned long arg);
    long (*compat_ioctl) (struct file *filp, unsigned int cmd,
                    unsigned long arg);

Funkcja ``unlocked_ioctl`` odpowiada wywołaniom ``ioctl`` przez "główną"
architekturę jądra. Nazwa jest zaszłością historyczną z czasów big
kernel locka - niegdyś sterowniki urządzeń wymagające big kernel locka
wypełniały pole ``ioctl``, podczas gdy nowsze lub skonwertowane sterowniki
używające własnych blokad używały ``unlocked_ioctl``. W obecnych wersjach
jądra big kernel lock i pole ``ioctl`` już nie istnieją.

Funkcja ``compat_ioctl`` odpowiada wywołaniom ``ioctl`` przez programy
użytkownika w trybie zgodności z 32-bitową wersją architektury -
np. programy na architekturę i386 pod jądrem na architekturę x86_64.
W przypadku, gdy struktury przekazywane przez ``ioctl`` nie zawierają
pól o rozmiarze zależnym od architektury, można ustawić oba pola
na tą samą funkcję.

Pierwszy argument odpowiada deskryptorowi pliku przekazanemu
przez wywołanie systemowe. Argument ``cmd`` jest dokładnie taki, jak w
wywołaniu systemowym. Opcjonalny argument ``arg`` jest przekazywany w
postaci liczby typu ``unsigned long`` bez względu na typ użyty przy
wywołaniu systemowym.

Zazwyczaj implementacja operacji ``ioctl`` zawiera po prostu konstrukcję
switch wybierającą odpowiednie zachowanie w zależności od wartości
argumentu ``cmd``. Różne komendy są reprezentowane różnymi numerami, którym
zazwyczaj nadaje się nazwy korzystając z definicji preprocesora. Program
użytkownika powinien mieć możliwość włączenia pliku nagłówkowego z
deklaracjami (zazwyczaj tego samego, który jest używany przy kompilacji
modułu sterownika).

Do twórcy sterownika należy ustalenie wartości liczbowych
odpowiadających komendom interpretowanym przez sterownik. Najprostszy
wybór, przypisujący kolejne małe wartości poszczególnym komendom,
niestety ogólnie nie jest dobrym rozwiązaniem. Komendy powinny być
unikalne w skali systemu, żeby uniknąć błędów, gdy poprawną komendę
wysyłamy do niepoprawnego urządzenia. Taka sytuacja może nie występować
zbyt często, ale jej konsekwencje mogą by poważne. Przy różnych
komendach dla wszystkich ``ioctl``, w przypadku pomyłki, zostanie zwrócone
``-EINVAL`` zamiast wykonania niezamierzonej akcji.

W ustalaniu wartości liczbowych dla komend pomocne mogą byc następujące
makra (zdefiniowane w ``asm/ioctl.h``):

``_IO(type,nr)``
    komenda ogólnego przeznaczenia (bez argumentu)
``_IOR(type,nr,dataitem)``
    komenda z zapisem w przestrzeni użytkownika
``_IOW(type,nr,dataitem)``
    komenda z odczytem z przestrzeni użytkownika
``_IOWR(type,nr,dataitem)``
    komenda z zapisem i odczytem

Oznaczenia:

``type``
    unikatowy numer dla sterownika (8 bitów, wybrany po przejrzeniu
    ``Documentation/ioctl-number.txt``) - numer magiczny
``nr``
    kolejny numer komendy (8 bitów)
``dataitem``
    struktura związana z komendą; rozmiar podanej struktury zazwyczaj
    nie może byc większa niż 16kb-1 (zależy to od wartości ``_IOC_SIZEBITS``).
    Kodowanie rozmiaru struktury zapisywanej/odczytywanej jako parametr może
    się przydać do wyłapywania programów skompilowanych z nieaktualnymi
    wersjami sterownika i pozwala uniknąć np. pisania poza buforem.

Przykład::

    #define DN_SETCOUNT    _IOR(0,3,int)

Operacje na pliku specjalnym sterowników urządzeń blokowych
-----------------------------------------------------------

Operacje sterowników urządzeń blokowych ustawia się w sposób podobny do
przypadku urządzeń znakowych. Z uwagi na rzadką potrzebę tworzenia takiego
sterownika oraz znaczny stopień związanych z nim komplikacji, niniejsze
zajęcia pominą szczegóły jego konstrukcji.


Literatura
----------

1. A. Rubini, J. Corbet, G. Kroah-Hartman, Linux Device Drivers, 3rd edition,
   O'Reilly, 2005. (http://lwn.net/Kernel/LDD3/)
2. Książki podane na stronie przedmiotu: http://students.mimuw.edu.pl/ZSO/


..
    =============================================================================
    Autor: Grzegorz Marczyński (g.marczynski@mimuw.edu.pl)
    Aktualizacja: 2003-03-13
    Aktualizacja: 2004-10-20 Stanisław Paśko (sp@mimuw.edu.pl)
    Aktualizacja: 2005-10-22 Piotr Malinowski (malinex@mimuw.edu.pl)
    Aktualizacja: 2006-11-16 Radek Bartosiak (kedar@mimuw.edu.pl) - linux 2.6
    Aktualizacja: 2012-03-18 Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
    Aktualizacja: 2013-03-25 Marcin Kościelnicki (m.koscielnicki@mimuw.edu.pl)
    =============================================================================

