.. _10-zadanie:

===============
Małe zadanie #5
===============

Proszę pobawić się gołym obrazem systemu plików, czyli:

- stworzyć plik na system plików (wypełniony zerami)
- stworzyć w nim system plików (dla uproszczenia revision 0, bez żadnych
  dodatkowych usprawnień (argumenty do ``mke2fs``:  ``-r 0 -O ""``)
- zrobić kopię obrazu
- zamontować go i utworzyć plik z rozpoznawalnymi danymi, np literami 'a'
- odmontować
- porównać obraz sprzed utworzenia pliku i po i zidentyfikować odpowiednie
  obszary:

  - superblok
  - deskryptor grupy
  - i-węzeł
  - dane katalogu
  - dane pliku

- zmodyfikować datę utworzenia pliku
- dopisać do pliku trochę danych, nie zmieniając liczby bloków:

  - zmienić rozmiar pliku w i-węźle
  - dopisać rozpoznawalne dane

- dopisać do pliku trochę danych dodając nowy blok, czyli:

  - zmienić bitmapę bloków
  - dopisać do i-węzła numer nowego bloku
  - zmienić rozmiar w i-węźle
  - ustawić dane w bloku

- dopisać nowy plik do katalogu, czyli:

  - zaalokować nowy inode, uwzględniając go w bitmapie i licznikach wolnych
    inodów
  - wypełnić odpowiednio inode
  - stworzyć wpis katalogowy wskazujący na ten inode
  - podpiąć jakieś bloki z danymi pod inode (jak wyżej)

Aby zaliczyć zadanie, należy przesłać (skompresowane) wersje tego samego
obrazu przed i po stworzeniu nowego pliku (system plików nie powinien być
w międzyczasie montowany do zapisu - proszę testować montując z ``-o ro``).
Obrazy powinny przechodzić przez ``fsck`` bez żadnych ostrzeżeń.
