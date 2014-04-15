.. _09-p1-seqfile:

===================
Przykład 1: seqfile
===================

Przykład bazuje na artykule z LWN: "Driver porting: The seq_file interface" (http://lwn.net/Articles/22355/)

Prosty moduł tworzący w ``procfs`` katalog, plik, oraz łącze symboliczne. Plik
jest obsługiwany przez ``seq_file`` i wypisuje wszystkie liczby do ``10000``
o wadze Hamminga 4.

Kod
---

- paczka: :download:`../p1_seqfile.tar`
- źródło: :download:`seqfile_example.c`
- Makefile: :download:`Makefile`
- KBUILD: :download:`Kbuild`
