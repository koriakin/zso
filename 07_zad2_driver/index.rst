=========================
Zajęcia 7: Magistrala PCI
=========================

Data: 01.04.2014, 02.04.2014

Zajęcia przeniesione do sali 4070 (wtorek) i 5820 (środa).

.. toctree::
   :hidden:

   zadanie


Materiały dodatkowe
===================

- :ref:`07-zadanie`


Wstęp
=====

PCI (Peripherial Component Interconnect) jest magistralą powszechnie
używaną do podłączania urządzeń sprzętowych do procesora. Oryginalne PCI
opiera się na 32-bitowej szynie równoległej współdzielonej między wieloma
urządzeniami, pracującej z częstotliwością 33MHz. Później wprowadzono
wiele innych interfejsów sprzętowych, które są programowo zgodne z PCI,
choć mają zupełnie różny interfejs elektryczny i protokół niskopoziomowy.
Do interfejsów widzianych programowo jako PCI zaliczają się m.in.
oryginalne PCI, PCI-X (stary serwerowy wariant PCI z większymi gniazdami),
AGP, CardBus (używany w laptopach), PCI Express (interfejs z zupełnie nową
warstwą fizyczną i protokołem, na bazie szeregowych jednokierunkowych
łączy P2P zamiast szyny wspólnego dostępu), ExpressCard (elektrycznie
zgodny z PCIE), HyperTransport (interfejs między procesorami AMD a chipsetem),
a także wewnętrzne interfejsy wielu chipsetów.

Magistrala PCI jest zorganizowana hierarchicznie - pojedyncza "domena" PCI
składa się z jednej lub więcej szyn PCI (PCI bus). Szyny PCI mają 8-bitowe
identyfikatory (bus id). Każda szyna zawiera do 32 fizycznych urządzeń
(identyfikowanych 5-bitowym device id), a każde fizyczne urządzenie może
posiadać do 8 "funkcji" (identyfikowanych 3-bitowym function id). Główna
szyna PCI, o identyfikatorze 0, jest podłączona do procesora (zwanego
hostem) przez tzw. host bridge. Pozostałe szyny są podłączone do szyn wyżej
w hierarchii przez tzw. mostki PCI-PCI (PCI-to-PCI bridge). Czasami
(szczególnie w systemach wieloprocesorowych) zdarza się wiele głównych szyn
PCI.

W czasach oryginalnego PCI, często wystarczała jedna szyna w całym
komputerze, a fizyczne urządzenie (pojedynczy device id) odpowiadało
pojedynczemu gniazdu PCI (lub chipowi wlutowanemu w płytę główną),
z kilkoma numerami zarezerwowanymi dla funkcji samego chipsetu. Z biegiem
czasu dodano gniazdo AGP (jako osobną szynę), a także wyodrębniono
interfejs między mostkiem północnym i południowym (np. HyperTransport)
jako osobną szynę. Obecnie szyna PCIE, ze względu na swoją architekturę
peer-to-peer, traktuje każde gniazdo (a także wnętrza tzw. switchy PCIE)
jako osobną szynę.

Mostki PCI-PCI są w większości przezroczyste - każda para urządzeń
znajdujących się w tej samej domenie PCI może komunikować się ze sobą, nie
musząc nawet wiedzieć czy są na tej samej szynie.

W przeważającej większości przypadków obecne komputery zawierają tylko
jedną domenę PCI, lecz czasem (najczęściej przy architekturach innych niż
x86 i sprzęcie z wyższej półki) można spotkać maszyny, w których każdy
slot PCI znajduje się w swojej własnej domenie. Pozwala to m.in.
na zabezpieczenie urządzeń przed skutkami awarii pozostałych oraz kontrolę
dostępu.

Każde urządzenie PCI w systemie można więc zidentyfikować jednoznacznie
przez połączenie numeru domeny, szyny, urządzenia fizycznego i funkcji.
Ze względu na rzadkie stosowanie wielu domen, numer domeny często bywa
pomijany. Należy zauważyć, że przez "urządzenie PCI" rozumie się zazwyczaj
funkcję fizycznego urządzenia, a nie fizyczne urządzenie.

Aby wyświetlić wszystkie urządzenia PCI w systemie można użyć komendy
``lspci``.  ``lspci -t`` spowoduje wyświetlenie drzewa urządzeń w postaci
hierarchicznej.  Przykładowa magistrala PCI (z komputera klasy nettop
z PCIE) może wyglądać tak::

    [szyna:fizyczne_urządzenie.funkcja Klasa urządzenia: Opis urządzenia]
    00:00.0 Host bridge: NVIDIA Corporation MCP79 Host Bridge (rev b1)
    00:00.1 RAM memory: NVIDIA Corporation MCP79 Memory Controller (rev b1)
    00:03.0 ISA bridge: NVIDIA Corporation MCP79 LPC Bridge (rev b2)
    00:03.1 RAM memory: NVIDIA Corporation MCP79 Memory Controller (rev b1)
    00:03.2 SMBus: NVIDIA Corporation MCP79 SMBus (rev b1)
    00:03.3 RAM memory: NVIDIA Corporation MCP79 Memory Controller (rev b1)
    00:03.5 Co-processor: NVIDIA Corporation MCP79 Co-processor (rev b1)
    00:04.0 USB controller: NVIDIA Corporation MCP79 OHCI USB 1.1 Controller (rev b1)
    00:04.1 USB controller: NVIDIA Corporation MCP79 EHCI USB 2.0 Controller (rev b1)
    00:08.0 Audio device: NVIDIA Corporation MCP79 High Definition Audio (rev b1)
    00:09.0 PCI bridge: NVIDIA Corporation MCP79 PCI Bridge (rev b1)
    00:0a.0 Ethernet controller: NVIDIA Corporation MCP79 Ethernet (rev b1)
    00:0b.0 SATA controller: NVIDIA Corporation MCP79 AHCI Controller (rev b1)
    00:10.0 PCI bridge: NVIDIA Corporation MCP79 PCI Express Bridge (rev b1)
    00:15.0 PCI bridge: NVIDIA Corporation MCP79 PCI Express Bridge (rev b1)
    01:00.0 VGA compatible controller: NVIDIA Corporation ION VGA (rev b1)
    02:00.0 Network controller: Atheros Communications Inc. AR9285 Wireless Network Adapter (PCI-Express) (rev 01)

Mamy tutaj 4 szyny PCI:

- ``00``: wewnętrzna szyna chipsetu, podłączona przez ``00:00.0`` do procesora
- ``01``: wewnętrzna szyna chipsetu do komunikacji z zintegrowanym procesorem
  graficznym, podłączona przez ``00:10.0`` do szyny ``00``.
- ``02``: szyna PCIE (z gniazdem mini-PCIE) do podłączenia karty wifi,
  podłączona przez ``00:15.0`` do szyny ``00``.
- ``03``: zwykła szyna PCI bez podłączonych urządzeń, podłączona przez
  ``00:09.0`` do szyny ``00``.

W większych maszynach często można znaleźć nawet kilkanaście szyn PCI.

Szyna PCI pozwala urządzeniom na wykonywanie tzw. transakcji między sobą.
Dowolne urządzenie (wliczając hosta) może wykonać transakcję na dowolnym
urządzeniu (wliczając hosta). Dostępne typy transakcji to:

- odczyt i zapis pamięci - adresowane przez podanie docelowego adresu
  pamięci
- odczyt i zapis portów IO - adresowane przez podanie docelowego adresu
  portu
- odczyt i zapis pamięci konfiguracyjnej - adresowane przez podanie
  identyfikatora urządzenia docelowego i offsetu w pamięci konfiguracyjnej
- przerwanie - adresowane zawsze do hosta, urządzenie może zgłaszać do 4
  różnych przerwań


Pamięć konfiguracyjna
=====================

Najważniejszą innowacją PCI w stosunku do starszych szyn, takich jak ISA
czy VESA local bus, było wprowadzenie automatycznego wykrywania
i konfigurowania urządzeń - w starszych szynach, użytkownik musiał ręcznie
powiedzieć systemowi operacyjnemu, jakie urządzenia są obecne w systemie
i zapewnić brak konfliktów zasobów między nimi. Czesto wymagało
to rozwiązań takich, jak ustawianie zworek czy przełączników DIP. Również
sterowniki należało ręcznie konfigurować. W magistrali PCI problemy te
rozwiązano przez wprowadzenie pamięci konfiguracyjnej.

Każde urządzenie PCI posiada tzw. pamięć konfiguracyjną, o rozmiarze od 64
do 256 (oryginalne PCI, AGP, ...) lub do 4096 bajtów (PCI-X, PCIE). Pamięć
ta służy do wykrycia typu, funkcjonalności oraz wymagań urządzenia PCI oraz
przygotowania go do działania przez BIOS lub system operacyjny. Nie jest to
zwykła pamięć - jej komórki w zależności od przeznaczenia mogą być
zapisywalne, tylko do oczytu, mieć tylko niektóre bity zapisywalne, nie
istnieć w ogóle, lub mieć jeszcze bardziej złożone zachowanie. Zawiera
m.in.:

- 16-bitowe identyfikatory producenta i modelu urządzenia (vendor id,
  device id) - pozwalają na zidentyfikowanie konkretnego modelu
- 16-bitowe identyfikatory producenta i modelu "podsystemu" (subsystem
  ids) - pozwalają na zidentyfikowanie konkretnej karty lub płyty głównej
  używającej danego urządzenia
- 24-bitowy identyfikator klasy urządzenia
- informacje o przerwaniach zgłaszanych przez urządzenie
- rejestry do konfiguracji zakresów pamięci i portów IO używanych przez
  urządzenie

Zawartość pamięci konfiguracyjnej można wyświetlić w stanie surowym
używając ``lspci -xxxx``, lub w formie przyjaznej użytkownikowi używając
``lspci -vvv``. Dostęp do większości pamięci konfiguracyjnej wymaga uprawnień
roota.

Rejestracja sterownika PCI w Linuksie
=====================================

Linuksowy podsystem PCI sam zajmuje się przydzieleniem właściwego
sterownika do urządzenia - sterownik nie powinien samemu przeszukiwać
urządzeń dostępnych w systemie, a jedynie zarejestrować się w podsystemie
PCI, przekazując mu listę obsługiwanych typów urządzeń. Odbywa się to
przez utworzenie struktury ``pci_driver``::

    struct pci_driver {
    	char *name;
    	const struct pci_device_id *id_table;
    	int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);
    	void (*remove) (struct pci_dev *dev);
    	int  (*suspend) (struct pci_dev *dev, pm_message_t state);
    	int  (*resume) (struct pci_dev *dev);
    	void (*shutdown) (struct pci_dev *dev);
    	/* ... */
    };

Pole ``id_table`` powinno być ustawione na wskaźnik do tablicy obsługiwanych
urządzeń, złożonej z następujących struktur::

    struct pci_device_id {
    	__u32 vendor, device;
    	__u32 subvendor, subdevice;
    	__u32 class, class_mask;
    	kernel_ulong_t driver_data;
    };

Pola ``vendor`` i ``device`` odpowiadają identyfikatorom producenta i urządzenia,
które sterownik chce obsługiwać. Pola ``subvendor`` i ``subdevice`` odpowiadają
identyfikatorom podsystemu. Każde z tych pól może zostać zastąpione przez
``PCI_ANY_ID``, aby zignorować dane pole przy dopasowywaniu urzadzeń do
sterownika. Pola ``class`` i ``class_mask`` pozwalają na zawężenie urządzeń do
urządzeń danej klasy lub podklasy - urządzenie będzie pokazywane
sterownikowi tylko, gdy ``(klasa_urządzenia & class_mask) == class``.
W przypadku, gdy nie chcemy filtrować urządzeń przez klasę wystarczy
wpisać tam ``0``.

Dla większości sterowników wystarczy tworzyć takie struktury makrem
``PCI_DEVICE()``, pobierającym tylko identyfikatory producenta i urządzenia
(podsystem i klasa będą ignorowane przy dopasowywaniu). Tablica powinna być
zakończona strukturą zawierającą same zera.

Strukturę ``pci_driver`` należy zarejestrować przez wywołanie
``pci_register_driver``, a wyrejestrować (przy wyładowaniu modułu) przez
``pci_unregister_driver``::

    int pci_register_driver(struct pci_driver *);
    void pci_unregister_driver(struct pci_driver *dev);

W momencie, gdy podsystem PCI znajdzie urządzenie spełniające nasze
kryteria, wywoła na nim funkcję ``probe()`` z przekazanej struktury. Funkcja
ta powinna zainicjować urządzenie i przygotować je do użytku przez
użytkownika. Pozostałe funkcje to:

- ``remove``: jest wykonywana, gdy należy odpiąć sterownik od urządzenia
  - przy wyrejestrowywaniu sterownika, bądź operacji hot-unplug.
- ``suspend``: wykonywana, gdy urządzenie powinno zostać wprowadzone w stan
  niskiego poboru energii - np. przy przechodzeniu komputera w stan
  uśpienia.  Należy w niej zapamiętać stan urządzenia, gdyż zostanie on
  zniszczony przez jego wyłączenie.
- ``resume``: wykonywana, przy powrocie ze stanu niskiego poboru energii
  - powinna odwrócić operację ``suspend``
- ``shutdown``: wykonywana przy wyłączaniu systemu

Urządzenie PCI jest reprezentowane przez strukturę ``pci_dev``. Sterownik może
przypiąć do tej struktury wskaźnik na swoje prywatne dane::

    void pci_set_drvdata(struct pci_dev *pdev, void *data);
    void *pci_get_drvdata(struct pci_dev *pdev);

Obszary pamięci i portów IO
===========================

Najważniejszym i najczęstszym typem transakcji na szynie PCI są dostępy do
pamięci. Przestrzeń adresowa szyny PCI jest 64-bitowa (choć nie wszystkie
urządzenia są w stanie używać tak dużych adresów - często można spotkać
urządzenia ograniczone np. do niskich 4GB przestrzeni adresowej). Zakresy
tej przestrzeni są przypisywane urządzeniom przez pamięć konfiguracyjną
- każde urządzenie może posiadać do 6 rejonów pamięci, tzw. BARów (od Base
Address Register - komórek w pamięci konfiguracyjnych, którymi się je
ustawia). Rozmiar rejonu jest zawsze potęgą dwójki i jest ustalany przez
urządzenie, zaś jego początek jest ruchomy i ustawia go BIOS lub system
operacyjny.

Szyna PCI pozwala na wykonywanie zapisów i odczytów różnego rozmiaru -
8-bitowych, 16-bitowych, lub 32-bitowych. Najczęściej rejony przestrzeni
adresowej pamięci są używane przez urządzenia jako tzw. memory-mapped IO
- adresy w tym rejonie mają specjalne znaczenie i dostępy do nich są
używane do kontroli urządzenia. Takie specjalne komórki pamięci nazywa się
rejestrami wejścia/wyjścia. Często się zdarza, że semantyka takich
dostępów jest bardzo różna od dostępów do zwykłej pamięci - w szczególności
odczyty mogą być znaczące. Zdarzają sie np. sprzętowe kolejki FIFO, gdzie
zapis pod dany adres powoduje dopisanie wartości do kolejki, a odczyt
pobranie wartości i usunięcie jej. Należy również uważać, aby przy
dostępach do rejestrów używać zapisu/odczytu o rozmiarze zgodnym
z rozmiarem rejestru - zapis bajtu do 32-bitowego rejestru może mieć
nieprzewidywalne skutki.  Należy również zawsze być świadomym porządku
bajtów, którego używa urządzenie - urządzenia PCI prawie zawsze są
little-endian, nawet jeżeli są używane w maszynach używających porządku
big-endian.

W Linuksie podsystem PCI załatwia za nas mapowanie rejonów pamięci urządzeń
do wirtualnej przestrzeni adresowej jądra. Aby ich użyć, musimy najpierw
włączyć obsługę tych rejonów na danym urządzeniu (i wyłączyć przy odpinaniu
od urządzenia)::

    int pci_enable_device(struct pci_dev *dev);
    void pci_disable_device(struct pci_dev *dev);

Musimy też zarezerwować dostęp do urządzenia, aby żaden inny sterownik nie
konfliktował z nami::

    int pci_request_regions(struct pci_dev *pdev, const char *drv_name);
    void pci_release_regions(struct pci_dev *);

Następnie należy zmapować konkretny BAR do naszej przestrzeni adresowej::

    void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long maxlen);
    void pci_iounmap(struct pci_dev *dev, void __iomem *p); 

Drugi parametr ``pci_iomap`` jest numerem BARu (rejonu), zaś trzeci to
ograniczenie na rozmiar zmapowanego obszaru - jeżeli jest niezerowy
i mniejszy od rozmiaru danego rejonu, zostanie zmapowany tylko jego
początek.

Zwracany wskaźnik jest specjalnego typu ``__iomem`` i nie należy z niego
korzystać bezpośrednio - dostęp do niego może wymagać specjalnych operacji
na niektórych architekturach obsługiwanych przez Linuksa. Zamiast tego
należy używać specjalnych funkcji::

    unsigned int ioread8(void __iomem *);
    unsigned int ioread16(void __iomem *);
    unsigned int ioread16be(void __iomem *);
    unsigned int ioread32(void __iomem *);
    unsigned int ioread32be(void __iomem *);

    void iowrite8(u8, void __iomem *);
    void iowrite16(u16, void __iomem *);
    void iowrite16be(u16, void __iomem *);
    void iowrite32(u32, void __iomem *);
    void iowrite32be(u32, void __iomem *);

Funkcje bez ``be`` w nazwie wykonują dostęp w porządku little-endian, funkcje
z ``be`` wykonują dostęp w porządku big-endian.

Oprócz rejonów pamięci, PCI obsługuje również rejony portów IO. Jest to
pozostałość z czasów wczesnej architektury IBM PC i magistrali ISA. Porty
IO nie są wykorzystywane w nowych urządzeniach i nie będziemy się tu nimi
zajmować.

DMA
===

DMA (Direct Memory Access) zachodzi, gdy urządzenie samo wykonuje
transakcje dostępu do pamięci na adresach zmapowanych do RAMu komputera.
Jest to podejście przeciwstawne do tzw. PIO (Programmed Input/Output),
czyli ręcznego przesyłania danych z/do urządzenia przez procesor
wykonujący operacje na jego rejestrach IO. Przy przesyłaniu danych przez
DMA, procesor przekazuje urządzeniu tylko adres bufora na dane,
a urządzenie samo zajmuje się resztą.

DMA w magistrali PCI jest obsługiwane natywnie - każde urządzenie może
wykonać transakcję dostępu do pamięci. Należy jednak pamiętać, że choć
zazwyczaj adresy używane na szynie PCI są równoważne fizycznym adresom
używanym przez procesor, nie na wszystkich systemach jest to prawdą.
Zdarzają się systemy, gdzie dostępy do pamięci przychodzące z szyny PCI
przechodzą przez tzw. IOMMU - układ stronicowania dla urządzeń.
Przeprowadza on translację adresów podobną do translacji adresów
wirtualnych przez procesor. Należy więc rozróżniać fizyczny adres danego
obszaru pamięci i tzw. adres DMA (zwany również bus address), czyli ten
pod którym będzie go widziało urządzenie. Układy IOMMU są często używane
na systemach klasy serwerowej oraz na architekturach innych niż x86.

Kolejną ważną rzeczą przy korzystaniu z DMA jest maksymalny rozmiar adresu
obsługiwany przez urządzenie - wiele urządzeń może generować np. tylko
32-bitowe adresy, co może uniemożliwiać dostęp do częsci pamięci komputera
(chyba, że mamy IOMMU, który potrafi zaradzić temu problemowi).

W Linuksie większość spraw załatwia za nas podsystem DMA. Aby urządzenie
mogło wykonywać DMA, najpierw należy włączyć możliwość wykonywania
transakcji przez urządzenie::

    void pci_set_master(struct pci_dev *dev);

Należy również poinformować podsystem DMA o rozmiarze adresu obsługiwanym
przez urządzenie, wykonując obie z poniższych funkcji::

    int pci_set_dma_mask(struct pci_dev *dev, u64 mask);
    int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask);

Jako parametr mask należy przekazać ``DMA_BIT_MASK(ilość bitów adresu)``.

Istnieje wiele sposobów na alokację pamięci do DMA, w zależności od
potrzeb.  Najprostszym jest ``dma_alloc_coherent``::

    void *dma_alloc_coherent(struct device *dev, size_t size,
    		dma_addr_t *dma_handle, gfp_t flag);
    void dma_free_coherent(struct device *dev, size_t size, void *cpu_addr,
    		dma_addr_t dma_handle);

Funkcja ``dma_alloc_coherent`` zwraca adres zaalokowanego bufora w przestrzeni
wirtualnej jądra, zaś do parametru ``dma_handle`` zwraca adres DMA tego bufora
w przestrzeni adresowej widzianej przez nasze urządzenie. Jako pierwszy
parametr tych funkcji należy podać ``&pdev->dev``, gdzie ``pdev`` jest wskaźnikiem
na strukturę ``pci_dev``.

Przed zwolnieniem pamięci DMA należy zawsze pamiętać o zapewnieniu, że
urządzenie przestało już z niej korzystać.

Przerwania
==========

Przerwania są mechanizmem pozwalającym urządzeniem informować procesor
o interesujących zdarzeniach. Urządzenie PCI może być podłączone do
tzw. linii przerwania. Urządzenie może zażądać przerwania na linii, do
której jest podłączone. Kiedy to nastąpi, procesor przerywa swoje obecne
zajęcie (chyba, że jest już zajęty obsługą innego, odpowiednio ważnego
przerwania), zapisuje swój stan, i wykonuje funkcję przypisaną do danej
linii przerwania. Ta funkcja powinna ustalić, które urzadzenie zgłosiło
przerwanie (ze względu na małą liczbę linii przerwań, PCI dopuszcza
współdzielenie ich między różnymi urządzeniami), obsłużyć dane zdarzenie
i wrócić do poprzednio wykonywanego kodu. Funkcja musi zapewnić, że po
powrocie urządzenie przestanie zgłaszać przerwanie - w przeciwnym
wypadku procesor może bez końca wykonywać tą funkcję.

W Linuksie obsługą przerwań zajmuje się kod architektury. Aby przy
przerwaniu wykonała się funkcja naszego sterownika, należy zarejestrować
ją przez ``request_irq``::

    typedef irqreturn_t (*irq_handler_t)(int irq, void *dev);
    int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
    		const char *name, void *dev);
    void free_irq(unsigned int irq, void *dev);

W powyższych funkcjach ``irq`` jest numerem linii przerwania w systemie. Dla
urządzeń PCI można go uzyskać z pola ``pdev->irq``. Parametr ``flags`` należy
natomiast ustawić na ``IRQF_SHARED``. Parametr ``dev`` należy ustawić na jakiś
unikatowy wskaźnik specyficzny dla naszego urządzenia - zostanie on
przekazany naszej funkcji.

Ponieważ linie przerwań bywają współdzielone, może być wiele funkcji
obsługi przerwań zarejestrowanych na jednej linii, a wywołanie funkcji
niekoniecznie oznacza, że to jej urządzenie spowodowało przerwanie.
Funkcja powinna sprawdzić, czy przerwanie pochodzi z jej urządzenia
i poinformować o tym system, zwracając ``IRQ_HANDLED`` w przypadku pomyślnego
obsłużenia przerwania lub ``IRQ_NONE`` w przypadku, gdy urządzenie nie
zgłaszało przerwania.

W funkcjach obsługi przerwań należy pamiętać o ograniczeniach - głównie
braku możliwości blokowania procesu. Sekcje krytyczne używane wewnątrz
przerwania muszą być oparte o spinlocki (używane przez ``spin_lock_irqsave``).

Przed usunięciem funkcji obsługi przerwania należy zapewnić, że urzadzenie
nie będzie już zgłaszać przerwań. Zaniedbanie tego może spowodować
załamanie systemu - jądro będzie bez przerwy starało się obsłużyć linię
przerwania, nie mając do tego żadnej działającej funkcji.

Nowsze urządzenia PCI (w tym wszystkie PCIE) obsługują alternatywny
mechanizm dostarczania przerwań, zwany MSI (Message Signalled Interrupts).
W MSI zamiast linii przerwań używa się zwykłych transakcji zapisu do
pamięci pod adres kontrolera przerwań. Zgłaszane przerwanie zależy od
wartości zapisanej przez urządzenie, czyli przerwania MSI nie są
współdzielone - funkcja obsługi przerwania może mieć pewność, że to jej
urządzenie je zgłosiło. Nie będziemy tu jednak opisywać szczegółowo obsługi
MSI.

Dokumentacja
============

Wiele z powyższych interfejsów jądra jest udokumentowanych w katalogu
``Documentation``. W szczególności warto zajrzeć do:

- ``Documentation/PCI/pci.txt``
- ``Documentation/DMA-API.txt``
- ``Documentation/DMA-API-HOWTO.txt``
- ``Documentation/PCI/MSI-HOWTO.txt`` - dla nowszego sprzętu z MSI
- A. Rubini, J. Corbet, G. Kroah-Hartman, Linux Device Drivers, 3rd edition,
  O'Reilly, 2005. (http://lwn.net/Kernel/LDD3/) - rozdziały 9, 10, 12.
