===============
Zajęcia 9: proc
===============

Data: 15.04.2014, 16.04.2014

.. toctree::
   :hidden:

   p1_seqfile/index
   zadanie


Materiały dodatkowe
===================

- :ref:`09-p1-seqfile`
- :ref:`09-zadanie`


Wprowadzenie
============

Na ostatnich zajęciach omawiany był system plików ``sysfs``, służący do
eksportowania obiektów jądra i ich atrybutów do przestrzeni użytkownika.
Tym razem poznamy jego poprzednika - system plików ``proc``. Choć użycie
``sysfs`` jest zalecane do wszystkich nowych interfejsów jądra, wiele
interfejsów jest wciąż opartych o ``proc``.


Funkcje używane przy tworzeniu i usuwaniu plików proc
=====================================================

Należy pamiętać o dołączeniu pliku nagłówkowego::

    #include <linux/proc_fs.h>

Pliki (w tym katalogi) w drzewie ``proc`` są reprezentowane przez strukturę
``proc_dir_entry`` (której pola nie są dla nas widoczne).


Dodanie katalogu
----------------

::

    struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent);

Tworzy katalog o nazwie ``name`` w katalogu opisanym przez ``parent``
(jeśli ``parent`` jest ``NULL``, to w ``/proc``).

Są również dostępne warianty ``proc_mkdir_mode`` (podajemy jeszcze prawa
dostępu) i ``proc_mkdir_data`` (podajemy dane ``void *``).

Dodanie pliku
-------------

::

    struct proc_dir_entry *proc_create_data(const char *name, umode_t mode,
                            struct proc_dir_entry *parent,
                            const struct file_operations *fops,
                            void *data);

Tworzy plik o nazwie ``name`` w katalogu opisanym przez ``parent``. Ustawia
zestaw operacji na pliku, prawda dostępu i wskaźnik danych prywatnych na
``fops``, ``mode, ``data``.


Ustawianie metadanych pliku
---------------------------

::

    void proc_set_size(struct proc_dir_entry *, loff_t);
    void proc_set_user(struct proc_dir_entry *, kuid_t, kgid_t);

Służą do ustawiania rozmiaru (zwracanego przez ``fstat``) i właściciela pliku.


Pobieranie danych prywatnych pliku
----------------------------------

Aby dostać się do danych prywatnych naszego pliku, możemy użyć jednej z dwóch
funkcji::

    void *PDE_DATA(const struct inode *);
    void *proc_get_parent_data(const struct inode *);

``PDE_DATA`` zwraca dane prywatne podanego pliku, natomiast
``proc_get_parent_data`` zwraca dane prywatne zawierającego go katalogu (co
jest przydatne, jeżeli tworzymy katalogi odpowiadające jakimś obiektom,
a pliki w nich odpowiadają ustalonym atrybutom).

Dodanie linku symbolicznego
---------------------------

::

    struct proc_dir_entry *proc_symlink(const char *name,
                    struct proc_dir_entry *parent,
                    const char *dest);

Tworzy link symboliczny o nazwie ``name`` w katalogu opisanym przez ``parent``
wskazujący na ``dest``.

Usunięcie pozycji z procfs
--------------------------

::

    void proc_remove(struct proc_dir_entry *);

Usuwa plik z ``proc``.


seq_file
========

Opis bazuje na artykule z LWN: "Driver porting: The seq_file interface"
(http://lwn.net/Articles/22355/)

Implementacja odczytywania zawartości wirtualnych plików takich jak te stworzone
w ``procfs`` jest łatwa, gdy użytkownik dostarczy bufor zdolny pomieścić całą
zawartość pliku. W przeciwnym wypadku trzeba napisać implementację wrażliwą na
stan otwartego pliku - jego obecny offset. Na ogół jest to dość uciążliwe, bo
trudno oszacować na podstawie samego offsetu co należy przekopiować do bufora
użytkownika.

Żeby ułatwić życie programistom, Alexander Viro napisał ``seq_file``, który każe
programiście jedynie zaimplementować interfejs iteratora po danych do wypisania
za pomocą predefiniowanych funkcji. Przed użyciem go, należy włączyć nagłówek
``linux/seq_file.h``.

Interfejs składa się z czterech metod, podpinanych przez strukturę
``seq_operations``::

    struct seq_operations {
        void *start(struct seq_file *s, loff_t *pos);
        void *next(struct seq_file *s, void *v, loff_t *pos);
        void stop(struct seq_file *s, void *v);
        int show(struct seq_file *s, void *v);
    }

Konwencja przyjęta w argumentach i wartościach zwracanych jest następująca:

- wartości typu ``void *`` to iterator (czyli dowolna struktura sterownika)
- wartości typu ``seq_file`` to struktura trzymająca stan seq_file i nie
  należy jej dotykać
- wartości typu ``loff_t`` i ``loff_t *`` reprezentują indeks w wypisywanej
  kolekcji

Idea jest następująca - iterator po strukturze do wypisania przesuwany jest
przez funkcję ``seq_next``, tworzony jest przez funkcję ``seq_start``,
niszczony przez funkcję ``seq_stop``, a do faktycznego wypisania tego na co
wskazuje iterator służy ``seq_show``. Mechanizm ``seq_file`` będzie wołać te
metody tak, aby zadowolić proces czytający.

Funkcja ``seq_start`` powinna respektować parametr ``pos`` - ustawić iterator
na ``pos``-tej pozycji.

Funkcja ``seq_show`` do wypisywania powinna używać funkcji::

    int seq_putc(struct seq_file *m, char c);
    int seq_puts(struct seq_file *m, const char *s);
    int seq_escape(struct seq_file *m, const char *s, const char *esc);
    int seq_path(struct seq_file *m, struct vfsmount *mnt, 
                     struct dentry *dentry, char *esc);
    int seq_printf(struct seq_file *m, const char *f, ...);

Jedyne co programista musi później zrobić, to odpowiednio ustawić strukturę
``file_operations`` w nastepujący sposób::

    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = seq_release.

Pole ``open`` powinno zostać ustawione na funkcję, która zawoła funkcję
``seq_open``::

    int seq_open(struct file *file, struct seq_operations *op)

z odpowiednio wypełnioną strukturą ``seq_operations``. Resztą zajmie się
mechanizm ``seq_file``.
