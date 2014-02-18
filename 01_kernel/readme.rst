===========================
Zajęcia 1: kompilacja jądra
===========================

.. contents::


Lektura
=======

Kernel-HOWTO (niestety, przestarzałe)
Kernel-Build-HOWTO
/usr/src/linux/README
/usr/src/linux/Documentation/*
info grub

Przygotowanie źródeł jądra
==========================

- Sklonować repozytorium gita z git.kernel.org (repozytorium
  linux/kernel/git/linux-stable) lub ściągniąć i rozpakować
  paczkę w wersji 3.13.3 z ftp.kernel.org bądź ftp.icm.edu.pl/pub/Linux/kernel/.
  W przypadku użycia gita, przejść na taga v3.13.3.
- Ustawić dowiązanie symboliczne /usr/src/linux na nowe źródła
  (jeśli to ze źródeł tego jądra będziemy chcieli później korzystać)

Informacje o wersjach jądra - X.Y.ZZ.ŹŹ(-abc)
=============================================

Dawniej (wersje jądra przed 2.6)
--------------------------------

Wówczas numery były postaci X.Y.ZZ(-abc).

Pierwsza liczba (X) - główny numer wersji, zmieniany przy radykalnych
zmianach w budowie jądra. Pierwsze wersje Linuksa miały (krótko) numer 0,
następnie przez długi czas 1; obecna główna wersja to 3.

Druga liczba (Y) - linia bądź seria jądra, parzyste liczby oznaczają wersję
'stabilną'; nieparzyste - 'rozwojową'. Wersja rozwojowa ma numer wyższy od
stabilnej, wiec 2.5.x jest wersją rozwojową dla linii 2.4.x.
W wersjach rozwojowych testowane są nowe koncepcje, które później są
ewentualnie przenoszone do wersji stabilnych. Wersje stabilne są też stale
poprawiane, więc w pewnym sensie także się rozwijają.

Trzecia liczba (ZZ), to wersja jądra w danej linii, cały numer wyznacza
'wydanie' (release).

Dodatkowo, wydania mogą być oznaczane różnymi przyrostkami, przede wszystkim
określającymi rodzaje łatek na nie nałożonych lub numer wersji testowej.
Przyrostek "pre" oznacza wersję beta, np. 2.4.20pre7 oznacza 7 wersję beta
kernela 2.4.20 . Po zakończeniu testowania ten kernel otrzyma numer 2.4.20,
bądź przekształci się w 2.4.20pre8, jeżeli Linus nie zatwierdzi wersji jako
oficjalnego wydania.

Zdarzają się też przyrostki oznaczające jądra tworzone równolegle do
"oficjalnego" (zatwierdzanego przez Linusa Torvaldsa) jądra, np.
arca - tworzone przez Andree Arcangeli,
ac - tworzone przez Alana Coxa, np. 2.0.36-ac12.

Numery wersji dla jądra 2.6 i 3
-------------------------------

Począwszy od serii 2.6 zrezygnowano z "dużych" serii rozwojowych o numerach
nieparzystych na rzecz bardziej ciągłego rozwoju. W serii 2.6, numery wersji
są postaci 2.6.XX.YY. Trzecia liczba (XX) oznacza wersję jądra w danej
linii, jest ona zmieniana gdy dodane zostaną nowe sterowniki bądź nowa
funkcjonalność. Nowe wydanie jądra różniące się od poprzedniego wyłącznie
poprawkami błędów mają numer różniący się czwartą liczbą (YY). Oczywiście
istnieją też jądra wydawane przez kogoś innego niż Linus Torvalds, zazwyczaj
ich nazwa kończy się jakimś przyrostkiem.

W lipcu 2011 została wydana wersja 3.0, rozpoczynająca nową serię. W tej
serii, numery wersji są postaci 3.XX.YY, gdzie XX i YY oznaczają to samo
co w serii 2.6. Zmiana ta nie wiązała się z żadną nową funkcjonalnością,
a z 20 rocznicą powstania Linuksa - wersja 3.0 nie różni się bardziej od
2.6.40 niż 2.6.40 od 2.6.39. Przejście na nowy system numeracji zostało
przeprowadzone ze względu na nieadekwatność starego systemu do nowego
modelu rozwoju jądra - numery wersji były po prostu za długie.

Pobieranie źródeł jądra
=======================

Najprostszym sposobem zdobycia źródeł jądra jest pobranie skompresowanego
pliku .tar ze strony http://kernel.org/, będącej oficjalnym archiwum wydań
jądra. Oprócz kompletnych paczek ze źródłemi, publikowane są również łaty
umożliwiające zaktualizowanie wcześniej ściągniętej paczki do nowszej wersji.

Trochę bardziej skomplikowanym, lecz znacznie elastyczniejszym sposobem
zdobycia źródeł jest użycia gita. Pozwala to na pracę na najświeższym kodzie
(nie zawartym jeszcze w żadnym oficjalnym wydaniu), a także niemal
natychmiastowe przemieszczanie się między wszystkimi dotychczasowmi
wersjami zawartymi w historii (od 2.6.12). Użycie gita jest też wymagane
w przypadku wysyłania własnych zmian do włączenia do oficjalnej wersji
jądra.

Istnieje wiele repozytoriów gita ze źródłami Linuksa - każdy podsystem jest
rozwijany w swoim własnym repozytorium, które jest następnie łączone
z głównym repozytorium, gdy nadchodzi czas na wydanie nowej wersji. Główne
repozytorium, należące do Linusa Torvaldsa i używane jako podstawa do nowych
wydań jądra, znajduje się pod adresem ::

  git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

Wszystkie "duże" wydania (2.6.XX, 3.XX) oraz wydania release candidate są
w nim dostępne jako tagi (v2.6.27, v3.1-rc3, itd). Wydania będące poprawkami
błędów (2.6.XX.YY, 3.XX.YY) są natomiast tworzone w osobnym repozytorium,
dostępnym pod adresem ::

  git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git

Jeśli chcemy pracować tylko nad zewnętrznymi modułami jądra, nie modyfikując
istniejącego kodu, posiadanie kompletnych źródeł nie jest konieczne.
Wystarczą nagłówki jądra, zainstalowane najczęściej w /usr/src/
i podlinkowane w /lib/modules/<wersja>/build. Muszą to jednak być prawdziwe
nagłówki jądra, a nie te przeznaczone do użytku przez libc. Wersja jądra,
z której pochodzą te nagłówki musi się również dokładnie zgadzać z wersją
jądra, pod którą będą używane skompilowane moduły. Niektóre dystrybucje
pakują takie nagłówki w osobnej paczce, nazwanej np. linux-headers.

Dystrubucje często posiadają również źródła jądra w standardowym repozytorium
pakietów - takie źródła zazwyczaj zawierają jednak łaty nakładane przez
dystrybucję i nie są identyczne z oficjalnym wydaniem.

Nakładanie łat (patches) na źródła jądra
========================================

Zamiast instalowania całego jądra można nałożyć łaty (pliki o nazwie
np. patch-xx.xx.xx.gz) na stare poleceniem patch, np. ::

  cd /usr/src/linux
  gzip -cd patch.xx.xx.xx.gz | patch -p1

Można też użyć do tego skryptu patch-kernel, znajdującego się w katalogu
scripts (automatycznie aplikuje łaty znalezione w katalogu, z którego został
wywołany).

Łaty 'oficjalne' o nazwie np. 'patch-2.6.17.14.gz' działać będą z poprzednim
wydaniem (względem nazwy określonej w nazwie patcha), czyli wspomniana łata
działać będzie z jądrem 2.6.17.13. Łaty nieoficjalne; np.
'patch-2.6.11-ac4.gz' zazwyczaj odnoszą się to tego samego wydania co w
nazwie.

Przed zainstalowaniem nowego jądra należy zapoznać się z plikiem
linux/Documentation/Changes, który zawiera wymagania odnośnie wersji
kompilatora, zainstalowanych pakietów, wersji bibliotek, itp. i upewnić się,
że stosowne wersje ma się zainstalowane.

Struktura źródeł jądra
======================

Zawartość głównego katalogu
---------------------------

Documentation
  katalog zawierający dokumentacje, w szczególności należy się zapoznać
  z plikiem CodingStyle
arch
  kod źródłowy zależny od platformy sprzętowej
block	
  funkcje warstwy urządzeń blokowych (ang. kernel block layer)
crypto	
  funkcje kryptograficzne (a także kompresja/dekompresja)
drivers
  sterowniki urządzeń
firmware
  kod pomocniczy ładowany przez niektóre sterowniki urządzeń na kontrolowane
  przez nie urządzenia
fs
  systemy plików
include
  pliki nagłówkowe
init
  niezależna od platformy część inicjalizacji systemu
ipc
  IPC (komunikacja międzyprocesowa System V)
kernel
  rdzeń jądra zarządzanie procesami, przerwania, DMA, czas
lib
  procedury pomocnicze (np. wypisywanie na ekran, rozpakowywanie spakowanego
  jądra)
mm
  zarządzanie pamięcią
net
  protokoły sieciowe
samples
  przykłady użycia niektórych wewnętrznych interfejsów jądra
scripts
  skrypty (np. do konfiguracji)
security
  kod związany z bezpieczeństwem (LSM - Linux Security Modules)
sound	
  sterowniki kart dźwiękowych oraz kod obsługujący dźwięk (ALSA)
usr	
  programy pomocnicze; aktualnie gen_init_cpio służący do tworzenia ramdysku
  ładowanego razem z jądrem systemu.
virt
  kod związany z wirtualizacją (KVM)

arch - kod zależny od platformy sprzętowej
------------------------------------------

Kod dla procesorów Intela znajduje się w katalogu arch/x86. Istnieje
możliwość skompilowania jądra dla innego procesora niż ten, na którym pracujemy,
jakkolwiek wymaga to (poza źródłami Linuksa) także kompilatora dla danej
platformy, działającego na naszym systemie (tzw. cross-compiler).

nagłówki w /usr/include i /usr/src/linux/include
------------------------------------------------

Niektóre katalogi z nagłówkami w /usr/include odpowiadają nagłówkom źródeł
Linuksa. Są to katalogi 'asm', 'linux' oraz 'scsi'. W najnowszych wersjach
biblioteki libc, pliki te, skopiowane z wersji stabilnej Linuksa, dostarczane
są wraz z tą biblioteką. Należy jednak uważać aby przy kompilowaniu modułów dla
jądra używać nagłówków z bieżącego jądra (-I/usr/src/linux(-wersja)/include),
lub po prostu zastąpić kopie katalogów dowiązaniami do źródeł.

Konfiguracja jądra
==================

Konfigurację jądra uruchamia się poleceniem make config (wersja tekstowa),
make menuconfig (tekstowa z menu) lub make xconfig (X-owa). Jeżeli chcemy jako
domyślne przyjąć ustawienia naszej poprzedniej konfiguracji
(zawartej w pliku .config) i odpowiadać tylko na nowe (w tej wersji jądra)
pytania, to możemy użyć polecenia make oldconfig.

Najprostsza w obsłudze jest wersja 'menuconfig' bądź 'xconfig', choć w tej
ostatniej częściej zdarzają się błędy.

Poniższe punkty opisują istotne elementy konfiguracji (tytuły odpowiadają
elementom głównego menu w 'menuconfig').

General setup
-------------

Ta część konfiguracji kontroluje kluczowe komponenty jądra systemu Linux.
Najważniejsze opcje to:

- 'Prompt for development and/or incomplete code/drivers' - udostępnia
  inne opcje, normalnie zablokowane, umożliwiające wykorzystanie
  niestabilnych, nowych i eksperymentalnych elementów jądra.
  Ponieważ na zajęciach używamy sterowników virtio, musimy włączyć tą opcję.
- 'Support for paging of anonymous memory (swap)' - obsługa pamięci wirtualnej
  na dysku
- 'System V IPC' - obsługa komunikacji międzyprocesowej
- 'Initial RAM filesystem and RAM disk (initramfs/initrd) support' - umożliwia
  wystartowanie Linuksa z ramdysku ładowanego przed uruchomieniem np. przez
  LILO, co pozwala na załadowanie sterowników dla dysków SCSI dostępnych
  tylko jako moduły lub też start systemu z urządzeń software-RAID.
- 'Initramfs source file(s)' - lista plików do załączenia w ramdysku

Loadable module support
-----------------------

.. note:: 

   Moduł - część kodu jądra, która może być ładowana lub usuwana z jądra na
   życzenie. Wszystkie części jądra, które nie są potrzebne przy starcie
   systemu i nie są ciągle używane w trakcie pracy systemu powinny być
   skompilowane jako moduły. Nawet wiele części potrzebnych przy starcie
   systemu może być modułami, musimy tylko użyć ramdysku (initial ramdisk).

Konfiguruje wsparcie dla modułów jądra. W zależności od potrzeb, możemy
włączyć lub wyłączyć ładowanie modułów (Enable loadable module support),
umożliwić usuwanie modułów (Module unloading, Forced module unloading),
umożliwić automatyczne ładowanie modułów przez kernel (Automatic kernel
module loading) oraz umożliwić ładowanie modułów skompilowanych dla innych
wersji jądra dzięki umieszczaniu w nich dodatkowych informacji o potrzebnych
funkcjach (Module versioning support). Możemy też umieścić w każdym module
sumę kontrolną (Source checksum for all modules).

Processor type and features
---------------------------

Umożliwia skonfigurowanie wsparcia i optymalizacji jądra dla danego
procesora (Processor family) -- w razie niepewności dla architektury i386
bezpiecznymi rozwiązaniami są 386 (zawsze) i 586 (dla procesorów od pentium
w górę) bądź Pentium (dla procesorów Intela od pentium w górę). Uwaga - w
przypadku wybrania nieprawidłowej wartości jądro może nie działać bądź
działać błędnie. Ważniejsze opcje dostępne w tym menu to także:

- 'Subarchitecture Type (PC-compatible)' - pozwala wybrać zwykły komputer typu
  PC lub jedną z kilku specjalnych maszyn z procesorami x86,
- 'High Memory Support' - wsparcie dla systemów z >1GB pamięci,
- 'Math emulation' - emulacja jednostki 387 dla starych procesorów bez FPU,
- 'MTRR (Memory Type Range Register) support' - obsługa rejestrów
  dostępu do pamięci, umożliwiająca ustawienie szyny PCI/AGP w tryb
  "write-combining", co może znacznie przyspieszyć aplikacje graficzne.
- 'Symetric multi-processing support' - wsparcie dla wielu procesorów
- 'SMT (Hyperthreading) scheduler support' - polepszenie właściwości planisty
  dla systemów z procesorem Pentium 4 z obsługą HT.
- 'Preemption Model' oraz 'Preempt The Big Kernel Lock' - pozwalają wybrać,
  czy jądro może być wywłaszczane.
- 'Local APIC support on uniprocessors' - wsparcie dla zaawansowanych
  kontrolerów przerwań dostępnych w nowszych płytach głównych. Opcja ta
  dostępna jest tylko, jeśli SMP jest wyłączone (dla systemów SMP APIC i tak
  zawsze jest włączone).

Power management options (ACPI, APM)
------------------------------------

Wybór obsługiwanych metod oszczędzania energii - w tym obsługa ACPI, APM
oraz zmian prędkości procesora w trakcie działania systemu.

Bus options (PCI etc.)
----------------------

Wybór obsługiwanych szyn systemowych oraz ich parametrów. Dla współczesnych
pecetów warto skonfigurować obsługę szyny PCI.

Executable file formats
-----------------------

Obsługa formatów plików wykonywalnych. Bez obsługi formatu ELF z
tradycyjnymi dystrybucjami Linuksa zbyt wiele nie da się zrobić.

Networking
----------

Zazwyczaj nie daje się, całe szczęście, wyłączyć obsługi sieci (Networking
support), bo bez tego niewiele by działało. Oprócz opisanego poniżej menu
Networking options można tu skonfigurować obsługę różnych metod komunikacji
- przez podczerwień, Bluetooth,  Wi-Fi.

Networking options
~~~~~~~~~~~~~~~~~~

Menu to zawiera konfigurację komponentów sieciowych i protokołów.
Najważniejsze z nich to:

'Packet socket'
  bezpośredni dostęp do urządzeń sieciowych.
'Unix domain sockets'
  gniazda Unixowe, umożliwiające komunikację międzyprocesową w sposób zbliżony
  do komunikacji sieciowej. Gniazda takie są wykorzystywane np. przez
  X-Windows, PostgreSQL.
'TCP/IP Networking'
  obsługa protokołu TCP/IP - bardzo ważne. Ale trudne do niewybrania.
'The IPv6 protocol'
  obsługa nowej wersji protokołu TCP/IP. Obecnie jeszcze niekonieczna, ale
  za jakiś czas zapewne nie będzie się można bez tego obejść.
'Network packet filtering framework (Netfilter)'
  filtrowanie i modyfikowanie pakietów (firewall, NAT).

Device Drivers
--------------

Różne ustawienia dotyczące sterowników pogrupowane w wielu menu. Zawartość
ważniejszych i mniej ważnych za to opisanych dawno temu menu jest opisana
poniżej.

W przypadku budowania jądra na potrzeby zajęć najlepiej jest wybrać tylko
niezbędne sterowniki - przyspieszy to znacząco proces budowy jądra.

Generic Driver Options
~~~~~~~~~~~~~~~~~~~~~~

Ogólne ustawienia, zwłaszcza dotyczące udostępniania sterowników
wymagających wgrywania firmware.

Parallel port support
~~~~~~~~~~~~~~~~~~~~~

Konfiguruje obsługę portu równoległego (drukarkowego). Do poprawnego
działania drukarki zazwyczaj potrzebne jest włączenie opcji:
'Parallel port support', 'PC-style hardware', 'Multi-IO cards'.
Dodatkowo przydatne jest włączenie 'IEEE 1284 transfer modes', umożliwi to
obsługę ECP i EPP, znacznie podwyższających szybkość transmisji.

Block Devices
~~~~~~~~~~~~~

Najważniejsze opcje to:

'Normal floppy disk support'
  wsparcie dla stacji dyskietek
'Loopback device support'
  pseudourządzenie umożliwiające stworzenie 'urządzenia' blokowego którego
  zawartość przechowywana jest w zwykłym pliku.
'RAM block device support'
  wsparcie dla RAM dysków.
'Packet writing on CD/DVD media'
  umożliwia zapis dysków CD/DVD

Wszystkie te opcje równie dobrze działają jako moduły jak i urządzenia
wbudowane w jądro.

ATA/IDE/MFM/RLL support
~~~~~~~~~~~~~~~~~~~~~~~

Stary sterownik dysków ATA, jego użycie nie jest obecnie zalecane.

SCSI support
~~~~~~~~~~~~

Włącza przede wszystkim obsługę magistrali SCSI, ale także umożliwia obsługę
dysków SATA (Serial ATA), stacji dysków podłączanych przez USB, nagrywarek
IDE w trybie symulacji SCSI, dysków Iomega ZIP etc. Do działania tych
urządzeń należy włączyć opcje 'SCSI disk support', 'SCSI CD-ROM support',
'SCSI generic support'. Dodatkowo w menu 'SCSI low-level drivers' można
włączyć obsługę sprzętowego kontrolera SCSI (o ile takowy jest
potrzebny/zainstalowany).

Serial ATA and Parallel ATA drivers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Obsługa urządzeń dyskowych ATA i SATA, nowszy sterownik. Należy tu też
wybrać posiadany kontroler ATA bądź SATA. Opcje 'AHCI SATA Support' oraz
'Generic ATA support' obsługują większość urządzeń, lecz mogą mieć mniejszą
funkcjonalność niż specjalistyczny sterownik. Ten sterownik jest zrobiony
na bazie warstwy SCSI - aby skorzystać z dysku bądź napędu optycznego,
należy również włączyć wsparcie dla odpowiedniego typu urządzenia w menu
SCSI.

Multiple devices driver support (RAID and LVM)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

'RAID support'
  włącza obsługę programowego RAID, umożliwiającego
  wykorzystanie wielu dysków jako jeden, co może zwiększyć wydajność
  i bezpieczeństwo operacji dyskowych.
'Device mapper support'
  obsługa niskopoziomowego manadżera woluminów, z
  którego korzystają programy umożliwiającego definiowanie zbiorów
  urządzeń (volume group) a na nich logicznych dysków (woluminów) dla
  uproszczenia zarządzania dyskami w dużych systemach.

Opcja 'RAID support' przydatna jest także w systemach domowych, pod
warunkiem posiadania co najmniej dwóch dysków twardych - w takiej sytuacji
tryb RAID-0 umożliwia dwukrotny wzrost wydajności operacji dyskowych.

Network device support
~~~~~~~~~~~~~~~~~~~~~~

Umożliwia kompilację sterownika karty sieciowej ('Ethernet (10 or
100Mbit)', 'Ethernet (1000 Mbit)'), bezprzewodowej karty sieciowej
('Wireless LAN') i obslugę PPP ('PPP (point-to-point protocol) support'),
jak również wielu innych typów kart sieciowych oraz protokołów.
Te cztery opcje jednak będą najczęściej wykorzystywane. W przypadku protokołu
PPP należy również skompilować (można jako moduły) wszystkie dodatkowe
opcje dostępne po włączeniu obsługi PPP, może oprócz tych eksperymentalnych.

Input device support
~~~~~~~~~~~~~~~~~~~~

Wsparcie (ogólne) dla urządzeń wejściowych. Jeżeli chcemy korzystać z
myszki, klawiatury, joysticka lub podobnych urządzeń, należy włączyć tę
opcję (szczęśliwie trudno ją wyłączyć) oraz odpowiedni moduł. Obsługa
wielu urządzeń USB znajduje się w menu 'HID Devices' (patrz niżej).

Character devices
~~~~~~~~~~~~~~~~~

Najważniejsze opcje to 'Virtual Terminal' oraz 'Support for console on
Virtual Terminal', umożliwiające korzystanie z konsoli Linuksa. Obie są
domyślnie niewidoczne i włączone (wyłączyć je można tylko dla systemów
wbudowanych). Ważne jest również 'Unix98 PTY support' (także niewidoczne i
włączone) włączające obsługę urządzeń terminali zdalnych alokowanych
dynamicznie przez system. W systemach biurkowych przydaje się również
włączenie 'Enhanced Realtime Clock Support', wykorzystywane przez aplikacje
multimedialne.

Graphics support
~~~~~~~~~~~~~~~~

Wybór obsługiwanych urządzeń graficznych (framebuffer) oraz w menu 'Console
display driver support' obsługa konsoli. 'VGA text console' umożliwia
obsługę konsoli na urządzeniu typu VGA. Znajdują się tu również przyspieszane
sprzętowo sterowniki graficzne (Direct Rendering Manager). W przypadku
użycia przyspieszenia sprzętowego z urządzeniem na magistrali AGP, należy
włączyć obsługę '/dev/agpgart (AGP support)' oraz chipsetu odpowiedniej
płyty głównej.

HID Devices
~~~~~~~~~~~

Wsparcie dla urządzeń typu HID - przede wszystkim klawiatury i myszy
podłączane przez USB.

USB support
~~~~~~~~~~~

Tu można wybrać obsługiwane chipsety oraz urządzenia podłączane przez
magistralę USB. 'USB Mass Storage support' umożliwia korzystanie z urządzeń
pamięci masowej, w tym dysków i pamięci flash. Wiele urządzeń USB znajduje
się w innych kategoriach - np. karty sieciowe w 'Network device support'.

Pozostałe sekcje ze sterownikami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W pozostałych menu (przede wszystkim warte uwagi: 'Sound', 'Multimedia
devices' - wsparcie dla kart TV) można skonfigurować różnorakie urządzenia
znajdujące się w systemie.  Zazwyczaj można je spokojnie skompilować jako
moduły, ponieważ nie są one potrzebne do startu systemu.

File systems
------------

Umożliwia włączenie obsługi różnorakich systemów plików. Najważniejsze to
system który używany jest na partycji startowej systemu (najczęściej ext2
bądź ext3) - musi on być wkompilowany w jądro bądź załączony na ramdysku.
Pozostałe systemy plików można skompilować jako moduły. Ważna jest również
obsługa 'Virtual Memory file system support', konieczny jest '/proc' (oba z
'Pseudo filesystems'), 'Filesystem in Userspace support' to FUSE, a 'sysfs
file system support' (zazwyczaj niewidoczny i domyślnie włączony) wymagany
do działania 'Unix98 PTY support'. Pozostałe systemy plików można
skompilować w zależności od potrzeb.

Kompilacja jądra - Kbuild
=========================

Kbuild jest systemem budowania Linuksa. Składa się on z odpowiednio
przygotowanych Makefile'i.

Tak jak zawsze przy Makefile'ach używa się go następująco::

  make <opcje> <cel> <opcjonalne zmienne dla Kbuild>

Użyteczną opcją do make jest -j<N> - spowoduje zrównoleglenie kompilacji aż do
<N> procesów jednocześnie (proszę nie nadużywać tego na students).

make clean
  Usuwa pliki związane ze starą wersja jądra (np. skompilowane pliki .o)

make bzImage
  Kompiluje jądro i umieszcza je w katalogu arch/xxx/boot (gdzie
  xxx to nazwa platformy, u nas x86) pod nazwa bzImage. Jądro tak utworzone jest
  skompresowane  (rozpakowuje się przy starcie systemu).

make modules
  Kompiluje części jądra, które skonfigurowaliśmy jako moduły.
  Następnie należy je zainstalować poleceniem make modules_install (są
  instalowane w katalogu /lib/modules/xx.xx.xx (xx.xx.xx to numer wersji
  jądra).

make all
  Ma takie działanie, jak make bzImage razem z make modules.

make modules_install
  Instaluje moduły do katalogu /lib/modules/<wersja>/ i wywołuje depmod w celu
  stworzenia informacji o zależnościach. Jeżeli podana jest zmienna
  INSTALL_MOD_PATH, instaluje w $INSTALL_MOD_PATH/lib/modules/<wersja>/

make help
  Pokazuje wszystkie opcje make

make mrproper
  Czyści dokładnie źródła (łącznie z konfiguracją!), usuwa też zależności,
  moduły, itp.

make install
  Instaluje jądro i dodaje je do konfiguracji bootloadera, nie zawsze działa
  zgodnie z oczekiwaniami

make htmldocs
  Kompiluje dokumentację w formacie DocBook do formatu HTML

make pdfdocs
  Kompiluje dokumentację w formacie DocBook do formatu PDF

make rpm
  Tworzy pakiet RPM z jądrem (przydatne w systemie RedHat).

Zmienne
-------

Niektóre parametry kompilacji jądra można podawać przez zmienne programu
make (dopisując ZMIENNA=wartość na koniec polecenia make). Te najważniejsze:

V=1
  verbose, Kbuild wypisze dokładniej co robi
ARCH=<arch>
  wymuszenie architektury <arch>, np. i386 (może być przydatne gdy
  kompiluje się na amd64)
EXTRA_CFLAGS=<flagi>
  podczas kompilacji flagi <flagi> będą dodane do wywołań
  gcc (użyteczne może być podanie -g, żeby mieć symbole)
INSTALL_MOD_PATH=<ścieżka>
  instaluje moduły w podanej lokalizacji

Ćwiczenie
=========

Skonfigurować i skompilować jądro i moduły, a następnie umieścic jądro w
katalogu /boot.

Należy pamiętać o:

- użyciu parametru ARCH=x86 przy każdym wywołaniu make
- włączeniu wszystkich opcji koniecznych do wystartowania systemu:

   - odznaczyć 64-bit kernel
   - włączyć eksperymentalne sterowniki (General setup -> Prompt for development and/or incomplete code/drivers)
   - obsługa szyny PCI (Bus options -> PCI support)
   - obsługa formatu ELF (Exectable file formats -> Kernel support for ELF binaries)
   - obsługa virtio (Virtualization -> PCI driver for virtio devices)
   - obsługa virtio balloon (Virtualization -> Virtio balloon driver)
   - obsługa gniazd lokalnych (Network support -> Networking options -> Unix domain sockets)
   - obsługa protokołu IPv4 (Network support -> Networking options -> TCP/IP networking)
   - sterownik virtio block (Device drivers -> Block devices -> Virtio block driver)
   - sterownik virtio net (Device drivers -> Network device support -> Virtio network driver)
   - sterownik virtconsole (Device drivers -> Character devices -> Virtio console)
   - system plików ext4 (File systems -> The Extended 4 (ext4) filesystem)
   - system plików proc (File systems -> Pseudo filesystems -> /proc file system support)
   - system plików FUSE (File systems -> FUSE (Filesystem in Userspace) support)

- ... (patrz punkty 6.*)

Programy ładujące
=================

Programy ładujące (bootloadery) ładują system operacyjny. Przy starcie
komputera BIOS ładuje do pamięci MBR (master boot record) i przekazuje mu
sterowanie. W MBR znajduje się tablica partycji i kod, który może być kodem
ładującym system (np. kod ładujacy DOS). Ma on za zadanie załadowac boot
sektor z odpowiedniej partycji i przekazac mu sterowanie. Jeśli jednak
chcemy zorganizować współistnienie kilku systemów na jednym dysku,
umieszczamy w MBR kod startera systemu, który pozwala wybrać przy starcie,
który boot sektor ma byc wczytany. Przykładami takich programów ładujących
(dla systemu Linux na architekturze x86) są:
 
- grub: pozwala na uruchamianie systemu z dysku twardego, bezpośrednio
  obsługuje szeroki zakres systemów plików i tablic partycji. Oprócz jądra
  systemu Linux wspiera również standard multiboot, FreeBSD/OpenBSD/NetBSD,
  oraz ładowanie innych bootloaderów (tzw. chain-loading). Posiada dość
  rozbudowaną linię poleceń pozwalającą m.in. na przeglądanie systemu plików
  i modyfikację opcji uruchamianego systemu
- grub2: bardziej modularny i rozbdowany następca gruba, wypierający już oryginał
- syslinux: w zasadzie pakiet kilku lekkich bootloaderów:

  - syslinux: pozwala na uruchomienie systemu Linux z systemu plików FAT
    lub NTFS, rzadko stosowany
  - isolinux: uruchamia system z płyt CD używających system plików ISO 9660
  - pxelinux: uruchamia system przez sieć, używając środowiska PXE

- lilo: niegdyś bardzo popularny bootloader napisany dla systemu Linux, dziś
  już przestarzały

GRUB
====

GRUB dostępny jest poprzez dwa interfejsy: dający więcej możliwości
interfejs linii komend bądź proste menu. Zwykle to drugie wystarcza;
po starcie GRUB szuka pliku konfiguracyjnego, jeśli go znajdzie, wyświetla
pozycje z tego pliku (odpowiadające obrazom Linuksa bądź innym systemom)
w formie pozycji menu. Pozycje menu można edytować (ale zmiany mają wpływ
tylko na dane uruchomienie, nie są zapamiętywane) - przejście do trybu
edycji odbywa się przez wciśnięcie 'e' (i w ten sposób być może naprawić
błędy z pliku konfiguracyjnego). Z menu można również przejść do linii
komend poprzez wciśnięcie 'c' (powrót przez ESC). Jeśli nasz GRUB jest
chroniony hasłem, przejście do trybu edycji czy linii komend możliwe jest
tylko po wcześniejszym naciśnięciu 'p' i wprowadzeniu hasła.

Plikiem konfiguracyjnym GRUB-a jest /etc/grub.conf. Plik składa się
z wpisów dla kolejnych obrazów jądra czy innych systemów operacyjnych.
Kolejne wpisy są numerowane poczynając od 0.

Najważniejszymi komendami wchodzącymi w skład wpisu są:
- TITLE nazwa_obrazu - nazwa, która będzie widoczna w menu. Rozpoczyna wpis;
wpis kończy się z końcem pliku lub z wystąpieniem kolejnej komendy title
(zaczynającej następny wpis)
- ROOT root_device - gdzie będziemy szukać obrazu
- ROOTNOVERIFY root_device - urządzenie nie zostanie zamontowane, a komenda
służy zwykle podaniu położenia innego boot loadera, co pozwala na tzw.
chain-loading i ładowanie np. Windowsów
- KERNEL plik_z_obrazem [opcje_dla_obrazu]

Istotnymi komendami wchodzącymi ponadto w skład grub.conf są:

- DEFAULT=numer_wpisu - który wpis będzie wybrany domyślnie, defaultem dla
  defaultu jest 0, czyli pierwszy wpis.
- TIMEOUT=ile_sekund - GRUB będzie czekał ile_sekund na wybór systemu, jeśli
  to nie nastąpi, zacznie ładować domyślny.

Znakiem '#' rozpoczynają się linie komentarza.


Plik grub.conf mógłby wyglądać np. tak::

    # Note that you do not have to rerun grub after making changes to this file
    # NOTICE:  You have a /boot partition.  This means that
    #          all kernel and initrd paths are relative to /boot/, eg.
    #          root (hd0,0)
    #          kernel /vmlinuz-version ro root=/dev/hda2

    default=1
    timeout=3

    title dos
    	rootnoverify (hd0,0) # ustawia GRUB root device, bez montowania
    	makeactive
    	chainloader +1	     # ładuje boot loader
    title linux
            root (hd0,4)
            kernel /vmlinuz ro root=/dev/hda5 hdc=ide-scsi
    title nowe
    	root (hd0,4)
    	kernel /vmlinuz-2.2.17 ro root=/dev/hda5 hdc=ide-scsi

GRUB 2
======

GRUB 2 jest następcą GRUBa, o większej modularności i funkcjonalności.
Interfejs menu jest dość podobny do GRUBa, ale znacznie zmieniona
została konfiguracja.

Plikiem konfiguracyjnym GRUBa 2 jest ``/boot/grub/grub.cfg``, ale nie należy
go modyfikować bezpośrednio - jest on generowany przez program grub-mkconfig
na podstawie skryptów w katalogu ``/etc/grub.d`` i konfiguracji w pliku
``/etc/default/grub``.

Dzięki skryptom konfiguracyjnym, nie trzeba ręcznie tworzyć wpisów
konfigracyjnych dla każdego jądra - wystarczy umieścić jądro jako
``/boot/vmlinux-<wersja>`` lub ``/boot/vmlinuz-<wersja>``, a ewentualny
initramfs jako /boot/initrd.img-<wersja>. Opcje przekazywane zainstalowanemu
jądru można ustawić w pliku ``/etc/default/grub``.

Po zmianie konfiguracji należy wydać polecenie::

  grub-mkconfig -o /boot/grub/grub.cfg

Aby zainstalować GRUBa 2 po raz pierwszy należy wydać polecenie::

  grub-install /dev/<dysk>

Ćwiczenie
=========

1. Zmodyfikować plik ``/etc/grub.conf``, tak żeby umożliwiał załadowanie nowo
   stworzonego obrazu.
2. Zrestartować komputer, poprosić bogów o wsparcie i uruchomić nowo utworzone
   jądro.

.. Autor: Staszek Paśko, aktualizacja: 1.05.2003
.. Grzegorz Marczyński, aktualizacja: 18.10.2004
.. Maria Fronczak (marys@mimuw.edu.pl), aktualizacja: 22.10.2005
.. Łukasz Sznuk, aktualizacja: 07.11.2006
.. Marcin Kościelnicki, aktualizacja: 09.02.2012, 18.02.2012, 18.02.2013, 19.02.2013
