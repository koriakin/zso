====
QEMU
====

Praca na zajęciach będzie odbywała się w środowisku wirtualnym z użyciem qemu.

Przed rozpoczęciem pracy, należy utworzyć własny obraz dysku na podstawie
bazowego obrazu przygotowanego na zajęcia, używając polecenia::

  qemu-img create -f qcow2 -o backing_file=/home/students/inf/PUBLIC/SO/2013-2014/01_kernel/zso2014.img zso2014_cow.img

Studenci pracujący na własnych komputerach mogą ściągnąć bazowy obraz
(skompresowany programem xz) pod adresem::

  http://students.mimuw.edu.pl/ZSO/PUBLIC-SO/2013-2014/01_kernel/zso2014.img.xz

Tak utworzony obraz jest obrazem copy-on-write: przechowuje tylko zmiany
w stosunku do bazowego obrazu, oszczędzając miejsce na dysku w przypadku
istnienia wielu kopii. Choć użycie COW nie zmniejszy użycia dysku w przypadku
pracy na własnym komputerze, jest wciąż zalecane, ze względu na możliwość
łatwego przywrócenia oryginalnego stanu w razie problemów z systemem plików.

Aby móc użyć współdzielenia plików między maszyną wirtualną a hostem
(zalecane), należy również utworzyć katalog hshare.


qemu należy uruchomić poleceniem::

  qemu-system-i386 <opcje>

Opcje qemu do użycia na zajęciach:

-drive file=zso2013_cow.img,if=virtio
    Podłącza obraz dysku twardego przez wirtualne urządzenie blokowe.
    Wymagana.

-enable-kvm
    Włącza sprzętową wirtualizację z użyciem KVM, znacznie przyspieszając
    działanie. Wymaga wsparcia ze strony systemu operacyjnego hosta:
    sterownik kvm-intel lub kvm-amd musi być załadowany, a qemu musi
    mieć dostęp do urządzenia /dev/kvm. Niestety nie działa w labach.
    Zalecana dla studentów używających własnych komputerów.

-smp <liczba procesorów>
    Ustawia liczbę procesorów w wirtualnej maszynie. Zalecane jest
    ustawienie tej opcji na liczbę procesorów hosta, o ile mamy go na
    wyłączność.

-net nic,model=virtio -net user
    Tworzy wirtualną sieć ethernetową, podłącza do maszyny wirtualnej
    za pomocą wirtualnego urządzenia sieciowego, oraz podłącza do stosu
    TCP/IP hosta przez maskaradę. Zalecana.

-m 512M -balloon virtio
    Pozwala na dynamiczną alokację pamięci przez gościa, do limitu 512MB.
    Zalecana.

-fsdev local,id=hshare,path=hshare/,security_model=none -device virtio-9p-pci,fsdev=hshare,mount_tag=hshare
    Podłącza katalog hshare/ na hoście przez protokół 9p do maszyny
    wirtualnej, pozwalając na łatwe zamontowanie go. Używając obrazu
    przygotowanego na zajęcia, tak wyeksportowany katalog będzie
    automatycznie widoczny w systemie gościa jako /host. Zalecana.

-chardev stdio,id=cons,signal=off -device virtio-serial-pci -device virtconsole,chardev=cons
    Podłącza do gościa terminal, na którym działa qemu. Niestety,
    ze względu na ograniczenia protokołu virtio, informacja o rozmiarze
    terminala nie jest automatycznie przekazywana do gościa. Aby używać
    pełnoekranowych programów w ten sposób, należy poprawić to ręcznie,
    wykonując wewnątrz systemu gościa komendę:

    stty rows <ilość wierszy> cols <ilość kolumn>

    Właściwą ilość wierzy i kolumn dla terminala można poznać wpisując
    na systemie hosta komendę:

    stty size

    Zalecana.

-display none
    Wyłącza graficzny interfejs qemu. Zalecana w przypadku pracy przez sieć.
    Konieczne jest wtedy użycie opcji virtconsole (lub innej opcji dającej
    możliwość zalogowania się do maszyny).

-kernel <plik> -append <opcje>
    Uruchamia bezpośrednio jądro linuxa z podanego pliku z podanymi
    opcjami, zamiast przechodzić przed standardowy proces bootowania.
    Czasem przydatna.

-gdb tcp::<port>
    Pozwala na podłączenie się do qemu przez gdb (polecenie target remote
    localhost:<port>) i debugowanie w ten sposób jądra. Czasem przydatna.

-S
    W połączeniu z opcją -gdb, powoduje uruchomienie qemu we wstrzymanym
    stanie, pozwalając na ustawienie breakpointów itp. przez gdb przed
    uruchomieniem systemu.

W przypadku użycia qemu z interfejsem graficznym, możliwy jest dostęp do
konsoli monitora qemu przez Ctrl+Alt+2 (powrót przez Ctrl+Alt+1). Ta konsola
pozwala m.in. na podłączanie nowych urządzeń wirtualnych w trakcie pracy qemu
oraz wymuszenie wyłączenia/resetu systemu (polecenia system_reset i quit).


Użycie innych maszyn wirtualnych niż qemu nie jest możliwe - jedno z zadań
zaliczeniowych będzie wymagało napisania sterownika urządzenia dostarczonego
w postaci zmodyfikowanej wersji qemu.


Środowisko wewnątrz obrazu
==========================

Systemem zainstalowanym na obrazie jest debian 6.0.6, z drobnymi
modyfikacjami. Można się zalogować używając loginu root i hasła root.
W przypadku potrzeby użycia nieuprzywilejowanego konta (np. do testów), można
użyć konta zso2013 (hasło zso2013).

Obraz zawiera jedną partycję typu ext3 z zainstalowanym bootloaderem grub2
i jądrem w wersji 2.6.34.8 (czyli takim, jakie będzie używane na zajęciach).

Przydatne polecenia:
 - apt-get install <nazwa paczki> - instalacja oprogramowania
 - apt-cache search <ciąg znaków> - przeszukiwanie bazy oprogramowania
 - screen - multiplekser terminali, przydatny przy pracy przez virtconsole,
   przydatne kombinacje klawiszy:
   - Ctrl-A Ctrl-C: tworzy nowy podterminal
   - Ctrl-A Ctrl-W: lista podterminali
   - Ctrl-A <cyfra>: przechodzi do podterminala o numerze 0-9, odpowiednio
   - Ctrl-A Ctrl-N: przechodzi do następnego podterminala
   - Ctrl-A Ctrl-P: przechodzi do poprzedniego podterminala
   - Ctrl-A ?: lista przypisań klawiszy
 - poweroff - zamyka system i wyłącza qemu

Jeśli qemu zostało uruchomione z odpowiednią opcją -net, system będzie miał
dostęp do sieci zewnętrznej. System hosta jest dostępny pod adresem IP
10.0.2.2. Dostęp z sieci zewnętrznej do sieci gościa nie jest jednak możliwy
(aby to umożliwić, należałoby użyć -net tap zamiast -net user, co wymaga praw
roota i jest znacznie bardziej skomplikowane). Zamiast tego zalecane jest
użycie opcji virtio-9p-pci (do współdzielenia plików) i virtconsole (dla
obsługi kopiuj/wklej i sensownego terminala).

Jeśli została użyta opcja virtio-9p-pci, katalog współdzielony (hshare/ na
systemie hosta) jest dostępny jako /host. Należy jednak zauważyć, że protokół
9p nie obsługuje poprawnie pełnej semantyki UNIXowego systemu plików, co może
powodować problemy przy niektórych zastosowaniach. Niemożliwa jest m.in.
kompilacja jądra wewnątrz tak zamontowanego katalogu, ze względu na wadliwą
obsługę wywołania rename().

W przypadku użycia virtconsole, należy pamiętać o ustawieniu poprawnego
rozmiaru terminala. Jeśli używamy cały czas tego samego rozmiaru terminala,
warto dodać odpowiednie wywołania stty na przykład do pliku .bash_profile.

Jeżeli nie ma możliwości użycia qemu z obsługą KVM, warto wykonywać zadania
wymagające większej mocy obliczeniowej (np. kompilacja jądra) na komputerze
hosta.


Ostrzeżenia
===========

qemu bez KVMa jest *wolne*. System może startować 1-2 minuty (w porównaniu do
kilku sekund z KVMem). Co więcej, używany system nie wypisuje nic na
virtconsole do momentu pełnego uruchomienia - brak żadnego wyjścia przez
dłuższy czas w przypadku użycia -display none nie znaczy więc, że system się
zawiesił.

Używanie qemu na studentsie, a szczególnie wykonywanie na nim czasochłonnych
czynności, jest bardzo kiepskim pomysłem - jest to bardzo wolne, a co więcej
zostanie po godzine siłowo przerwane przez limit czasu procesora. Lepiej użyć
któregoś z komputerów w labach, albo własnej maszyny.

sshowanie się na wirtualną maszynę jest możliwe, z użyciem forwardowania
portu 22, lecz należy pamiętać o uprzedniej zmianie hasła na kontach root
i zso2013.

Używając obrazu copy-on-write, należy uważać, aby nigdy nie modyfikować
bazowego obrazu - jakakolwiek zmiana spowoduje, że obraz copy-on-write staje
się bezużyteczny. Najlepiej ustawić na nim uprawnienia 444 zaraz po
ściągnięciu i dekompresji.

Na stacjach laboratoryjnych KVM nie działa w 32-bitowej wersji qemu - zamiast
polecenia qemu-system-i386 należy użyć qemu-system-x86_64 (z tymi samymi
parametrami). System działający w środku pozostaje 32-bitowy.
