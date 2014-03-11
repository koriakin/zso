.. _04-p4-kmalloc:

============================
Przykład 4: Alokacja pamięci
============================

:download:`../p4_kmalloc.tar`

Dynamiczny przydział pamięci dla jądra

Jądro może przydzielać dynamicznie pamięć na własny użytek. Służa do tego
funkcje::

    void *kmalloc(unsigned int size, int priority);
    void kfree(void * obj);

Funkcja kmalloc() pozwala na przydział spójnego obszaru pamięci fizycznej
wielkości maks. 32 stron pamięci (daje to dla x86 niecałe 128kb pamięci; 
cześć pamięci jądro  rezerwuje na nagłówek bloku). Przydział pamięci
realizowany jest szybko (algorytm Buddy). Parametr priority określa
rodzaj pamięci (stałe GFP_* zdef. w pliku linux/mm.h). Definicje
znajdują się w pliku linux/malloc.h.

- GFP_KERNEL - najczęściej używana, może być blokująca, więc wołać ją można
  tylko z kontektu procesu lub we własnym wątku.

- GFP_ATOMIC - nie blokuje, może byæ wołana z procedur obsługi przerwań.

Nieco informacji na temat kmalloc można znaleźć w Projekcie LabLinux,
rozdział Zarządzanie pamięcią, podrozdział 4.11 Pamięć dla jądra.

::

    void *vmalloc(unsigned long size);
    void vfree(void *addr);

Przy pomocy vmalloc() można przydzielić obszar dowolnie duży (pod
warunkiem, ze jest odp. dużo wolnej pamieci fizycznej), ale już
niespójny (pamięć ta przechodzi przez translację adresów). Definicje
znajdują się w pliku linux/mm.h.

::

    unsigned long __get_free_pages(int gfp_mask, unsigned long order)

Przydziela 2^order całych stron, parametr gfp_mask określa, jak przydzielać
strony (jak w kmalloc).

::

    void __free_pages(struct page *page, unsigned long order)
     Zwalnia strony.

SLAB allocator
--------------

Efektywnie przydziela pamięć na obiekty o stałej długości.
Przydziela całą stronę i dzieli na fragmenty o zadanej długości, układając
je dodatkowo tak, by maksymalnie wykorzystać cache procesora.
Przed przydzielaniem pamięci trzeba utworzyć slab cache za pomocą funkcji
kmem_cache_create(), a następnie przydzielać/zwalniać obiekty za pomocą
kmem_cache_alloc()/void __kmem_cache_free().
Po zakończeniu używania slab cache należy je zniszczyć za pomocą
kmem_cache_destroy().
Przydzielaniem pamięci na kolejne obiekty i zwalnianiem (leniwym) 
nieużywanej pamięci zajmuje się slab allocator. Pamięć na nowy obiekt jest
inicjalizowana za pomocą konstruktora podanego przy tworzeniu cache,
a przed zwolnieniem jest wołany destruktor.

::

     kmem_cache_t * kmem_cache_create (
         char *name, size_t size, size_t offset,
         unsigned long flags, 
         void (*ctor)(void*, kmem_cache_t *, unsigned long),
         void (*dtor)(void*, kmem_cache_t *, unsigned long));
         
     int kmem_cache_destroy (kmem_cache_t * cachep);

     void * kmem_cache_alloc (kmem_cache_t *cachep, int flags);
     void kmem_cache_free (kmem_cache_t *cachep, void* objp);

