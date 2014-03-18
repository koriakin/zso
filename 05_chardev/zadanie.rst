.. _05-zadanie:

===============
Małe zadanie #3
===============

Przykładowy sterownik załączony do materiałów ma kilka wad. Zadanie polega
na poprawieniu ich:

- użycie liczby zapisanych bajtów z syscalla ``write`` do ustawienia ilości
  powtórzeń jest co najmniej dziwnym interfejsem. Należy zmodyfikować
  operację ``write`` tak, aby czytała ona przekazane przez użytkownika dane
  jako liczbę w formacie dziesiętnym, i ustawiała na nią liczbę powtórzeń.

  Założenia:

  - cała liczba jest zapisana jednym wywołaniem ``write``
  - po liczbie może wystąpić (lub nie) znak nowej linii
  - liczba ma co najwyżej 32 znaki (należy zwrócić błąd jeśli użytkownik
    spróbuje wpisać zbyt długą liczbę) i jest bez znaku
  - wpisany ciąg znaków jest poprawną liczbą (tzn. pasuje do regexa
    ``[0-9]+\n?``) - to też trzeba sprawdzić

- liczba powtórzeń jest globalna dla wszystkich użytkowników, przez co
  urządzenie nie może być użyte w bezpieczny sposób na systemie z wieloma
  użytkownikami. Należy zmodyfikować sterownik tak, aby każdy użytkownik
  miał niezależnie ustawianą liczbę powtórzeń. Do przechowywania danych
  można użyć standardowej implementacji list z jądra oraz dynamicznej
  alokacji pamięci przez ``kmalloc``.
