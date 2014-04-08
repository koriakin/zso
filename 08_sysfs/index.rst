================
Zajęcia 8: sysfs
================

Data: 08.04.2014, 09.04.2014

.. contents::

Wprowadzenie
============

W jądrze często pojawia się konieczność udzielenia dostępu do pewnych danych
o urządzeniu do przestrzeni użytkownika. Użycie do tego urządzeń znakowych
jest dość nieporęczne - urządzenie znakowe to dość "ciężki" obiekt, a dostęp
do niego odbywa się przez ograniczony interfejs ``read/write`` bądź niewygodny
interfejs ``ioctl``.

Pierwszym rozwiązaniem tych problemów w systemie Linux był system plików
``proc``, pozwalający łatwo stworzyć dużą liczbę plików specjalnych do
komunikacji z użytkownikiem. Miał on jednak sporo wad: przede wszystkim
brak w nim struktury (każdy wrzuca pliki gdzie mu się spodoba), a przesył
danyh wymaga kosztownego i delikatnego formatowania i parsowania strumienia
bajtów.

Aby rozwiązać problemy z systemem plików ``proc``, powstał system plików
``sysfs``. Ma on następujące cechy:

- każde urządzenie, sterownik, moduł itp. w systemie zrobione jest na bazie
  struktury ``kobject`` i automatycznie otrzymuje katalog w ``sysfs``
- katalogi w ``sysfs`` zorganizowane są hierarchicznie - w przypadku urządzeń,
  każde urządzenie jest podkatalogiem urządzenia, do którego jest podłączone
- relacje między obiektami reprezentowane są przez symlinki
- atrybuty obiektów reprezentowane są przez pliki
- jest standardowo zamontowany w ``/sys``

Aby udostępnić użytkownikowi dostęp do jakiejś funkcjonalności, należy dostać
się do swojej struktury ``kobject`` i podpiąć do niej atrybuty. W przypadku
urządzeń, struktura ``kobject`` jest polem ``kobj`` struktury ``device``.


Dodawanie atrybutów do urządzeń
===============================

Aby dodać atrybut (czyli plik reprezentujący jeden parametr) do obiektu,
należy stworzyć strukturę opakowującą strukturę ``attribute`` odpowiednią
dla danego typu obiektu. W przypadku urządzeń jest to ``device_attribute``::

    struct attribute {
        char *name;
        struct module *owner;
        mode_t mode;
    };

    struct device_attribute {
        struct attribute attr;
        ssize_t (∗show) (struct device ∗dev, char ∗buf);
        ssize_t (∗store) (struct device ∗dev, const char ∗buf, size_t count);
    };

I dodać ją do naszego urządzenia przez::

    int device_create_file(struct device ∗device, struct device_attribute ∗entry);
    void device_remove_file(struct device ∗dev, struct device_attribute ∗attr);

Funkcja ``show`` jest wywoływana przy odczycie atrybutu przez użytkownika i ma
do dyspozycji bufor o rozmiarze ``PAGE_SIZE``, do którego może zapisać wartość
atrybutu (i zwrócić jego rozmiar). Funkcja ``store`` jest wywoływana przy
zapisie atrybutu i otrzymuje jego kompletną wartość. W przeciwieństwie do
funkcji ``read``/``write``, funkcje te zawsze operują na całej wartości atrybutu
(nie trzeba obsługiwać częściowych odczytów/zapisów).


Atrybuty binarne
================

Czasami proste atrybuty nie wystarczają i konieczny jest eksport danych
binarnych, definiując własną implementację ``read``/``write`` jak przy
zwykłym pliku. Do tego służą atrybuty binarne::

    struct bin_attribute {
        struct attribute attr;
        size_t size;
        void ∗private;
        ssize_t (∗read) (struct kobject ∗, char ∗buf, loff_t off, size_t size);
        ssize_t (∗write) (struct kobject ∗, char ∗buf, loff_t off, size_t size);
        int (∗mmap) (struct kobject ∗, struct bin_attribute ∗attr, struct vm_area_struct ∗vma);
    };
    int sysfs_create_bin_file(struct kobject ∗kobj, struct bin_attribute ∗attr);
    int sysfs_remove_bin_file(struct kobject ∗kobj, struct bin_attribute ∗attr);


Relacje między obiektami
========================

Tworzenie symlinków w sysfs możliwe jest przez funkcje::

    int sysfs_create_link(struct kobject ∗kobj, struct kobject ∗target, char ∗name);
    void sysfs_remove_link(struct kobject ∗kobj, char ∗name);
