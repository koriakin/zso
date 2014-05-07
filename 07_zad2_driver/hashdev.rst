.. _07-hashdev:

==================
Urządzenie hashdev
==================

.. contents::


Wprowadzenie
============

Urządzenie ``hashdev`` służy do szybkiego wykonywania operacji SHA-1
na dużych blokach danych, celem odciążenia głównego procesora w komputerze.
Dane oraz polecenia są odczytywane przez bezpośredni dostęp do pamięci (DMA).
Urządzenie jest podłączane do komputera przez szynę PCI - identyfikator
producenta to ``0x1af4``, a identyfikator urządzenia to ``0x10ff``.

Urządzeniem steruje się przez rejestry wejścia/wyjścia zmapowane do pamięci
(MMIO). Ten obszar MMIO jest pierwszym i jedynym obszarem BAR używanym
przez urządzenie (BAR0). Urządzenie wykorzystuje również jedną linię
przerwania PCI.

Obszar MMIO ma wielkość 4kiB, ale tylko niektóre bajty z tego zakresu są
używane na rejestry. Wszystkie rejestry, o ile nie zaznaczono inaczej, są
32-bitowe w formacie little-endian i powinny być używane tylko przez
wyrównane 32-bitowe odczyty i zapisy.

Urządzenie składa się z czterych bloków:

- blok operacji SHA-1
- blok wczytywania danych
- blok wczytywania poleceń
- blok sterujący

Blok sterujący zajmuje się nadzorowaniem pracy całego urządzenia. Jego
rejestry to:

``BAR0 + 0x00: ENABLE``
  Rejestr kontrolujący pracę bloków wczytywania danych i poleceń. Posiada
  2 bity:

  - bit 0: ``FETCH_DATA`` - jeśli ustawiony na 1, blok wczytywania danych
    jest włączony
  - bit 1: ``FETCH_CMD`` - jeśli ustawiony na 1, blok wczytywania poleceń
    jest włączony

  Dostępny do odczytu i zapisu. Przy resecie urządzenia, rejestr zostaje
  ustawiony na 0.

``BAR0 + 0x04: STATUS``
  Rejestr statusu, tylko do odczytu. Posiada 2 bity:

  - bit 0: ``FETCH_DATA`` - jeśli ustawiony na 1, blok wczytywania danych
    jest aktywny (czyli jest włączony i ma pracę do wykonania)
  - bit 1: ``FETCH_CMD`` - jeśli ustawiony na 1, blok wczytywania poleceń
    jest aktywny (czyli jest włączony i ma polecenia do wczytania)

``BAR0 + 0x08: INTR``
  Rejestr statusu przerwań. Posiada 18 bitów, każdy odpowiadający jednemu
  przerwaniu:

  - bity 0-15: ``USER[i]`` - przerwania użytkownika. Mogą być wyzwolone przez
    wysłanie odpowiedniego polecenia.
  - bit 16: ``FETCH_DATA_COMPLETE`` - wyzwalane przy zakończeniu wczytywania
    danych przez blok wczytywania danych
  - bit 17: ``INVALID_COMMAND`` - wyzwalane przy otrzymaniu niepoprawnego
    polecenia

  Każde przerwanie może być w danej chwili aktywne bądź nie. Przy odczycie
  tego rejestru, bity odpowiadające aktywnym przerwaniom będą miały wartość
  1. Przerwanie może zostać wyzwolone (tzn. stać się aktywne) przez zajście
  odpowiedniego zdarzenia w urządzeniu. Przerwanie może zostać wyzerowane
  (tzn. stać się nieaktywne) przez zapis do rejestru INTR - przy zapisie
  zerowane są wszystkie przerwania, których odpowiednie bity sa ustawione
  na 1 w zapisanej wartości. Przykładowo, zapisanie 0x10040 spowoduje
  wyzerowanie przerwań ``FETCH_DATA_COMPLETE`` oraz ``USER[6]``.

``BAR0 + 0x0c: INTR_ENABLE``
  Rejestr włączania przerwań, dostępny do odczytu i zapisu. Posiada takie
  same bity jak ``INTR``. Jeśli dany bit jest ustawiony na 1 w tym rejestrze,
  urządzenie zgłosi przerwanie na swojej linii przerwań PCI gdy
  odpowiednie przerwanie jest aktywne. W przeciwnym wypadku, przerwanie
  jest ignorowane i nie będzie zgłoszone (choć będzie widoczne przy
  odczycie rejestru ``INTR``). Przy resecie urządzenia, rejestr zostaje
  ustawiony na 0.



Suma SHA-1
==========

SHA-1 jest powszechnie używaną kryptograficzną funkcją skrótu. Jej wejście
to dane o dowolnej długości (w bitach), a jej wyjście to 160-bitowa liczba. Jej
ogólny schemat jest następujący:

1. Ustaw 5 32-bitowych zmiennych tymczasowych, h[0..4], na wartości początkowe:

   - h[0] = ``0x67452301``
   - h[1] = ``0xEFCDAB89``
   - h[2] = ``0x98BADCFE``
   - h[3] = ``0x10325476``
   - h[4] = ``0xC3D2E1F0``

2. Uzupełnij dane:

   - Na koniec danych dopisz bit 1
   - Dopóki rozmiar danych (w bitach) nie przystaje do 448 modulo 512, dopisuj
     bit 0 na koniec danych
   - Na koniec danych dopisz oryginalny (sprzed procesu uzupełniania) rozmiar
     danych w *bitach*, w formie 64-bitowej liczby big-endian

3. Podziel dane na bloki po 512 bitów. Dla każdego bloku wykonaj jeden krok
   SHA-1. Wejściem do kroku SHA-1 są wartości h[0..4] i blok danych, a wyjściem
   są nowe wartości h[0..4]. Kroku SHA-1 nie będziemy tu dokładnie opisywać.

4. Wynikiem SHA-1 jest konkatenacja ostatecznych wartości zmiennych tymczasowych
   h[0], h[1], h[2], h[3], h[4].

Wszystkie dane w obliczeniach traktowane są jako big-endian (tzn. za pierwszy
bit w bajcie uznaje się bit najwyższy).

Urządzenie wykonuje tylko operację kroku SHA-1 - za inicjację zmiennych
tymczasowych oraz uzupełnienie danych odpowiedzialny jest sterownik.


Blok operacji SHA-1
===================

Blok operacji SHA-1 zajmuje się wykonywaniem operacji kroku SHA-1. Posiada
następujące rejestry:

``BAR0 + 0x20 + i * 4, i < 5: SHA1_H[i]``
  Wartość zmiennej tymczasowej h[i]. Dostępne do odczytu i do zapisu.

``BAR0 + 0x40 + i * 4, i < 15: SHA1_DATA[i]``
  Blok danych, na którym należy wykonać krok, dostępne tylko do zapisu.
  Indeks i odpowiada bitom i*32..i*32+31 bloku. Zapisanie wartości do
  SHA1_DATA[15] kończy blok i powoduje wykonanie kroku SHA-1.

Krok SHA-1 wykonuje się natychmiastowo z punktu widzenia sterownika (tzn.
urządzenie blokuje dostępy aż wyliczanie kroku się skończy) - nie jest
konieczne jawne oczekiwanie na to.

Należy zauważyć, że PCI jest szyną little-endian, a algorytm SHA-1 opiera
się na obliczeniach big-endian - z tego powodu, konieczne może okazać się
zamienienie porządku bajtów przy komunikacji z urządzeniem.


Blok wczytywania danych
=======================

Blok wczytywania danych zajmuje się czytaniem danych do kroków SHA-1 przez DMA
i przekazywaniem ich blokowi operacji SHA-1. Ma on dwa rejestry:

``BAR0 + 0x10: SHA1_DATA_PTR``
  32-bitowy wskaźnik na dane do wczytania. Zawsze musi być wyrównany do
  wielokrotności 64 bajtów.

``BAR0 + 0x14: SHA1_DATA_COUNT``
  32-bitowa liczba bajtów danych do wczytania. Zawsze musi być wielokrotnością
  64.

Blok wczytywania danych włączany jest w bloku sterującym. Kiedy tylko blok ten
jest włączony, a ``SHA1_DATA_COUNT`` ma niezerową wartość, będzie on wczytywał
dane w następujący sposób:

1. Wczytaj blok danych (64 bajty) z adresu ``SHA1_DATA_PTR``
2. Wyślij te dane do bloku operacji SHA-1 (tak, jakby zostały ręcznie wpisane
   do rejestrów ``SHA1_DATA[i]``)
3. Zwiększ ``SHA1_DATA_PTR`` o 64
4. Zmniejsz ``SHA1_DATA_COUNT`` o 64

Używanie rejestrów bloku operacji SHA-1 czy bloku wczytywania danych jest złym
pomysłem, gdy jest on aktywny. W zasadzie jedyną bezpieczną operacją jest
odczyt ``SHA1_DATA_COUNT`` i porównanie do 0 (żeby stwierdzić, czy wczytywanie
już się skończyło).

Gdy wczytywanie zakończy się (tzn. ``SHA1_DATA_COUNT`` spadnie do 0 przez
operację wczytywania bloku), wyzwolone zostanie przerwanie zakończenia
wczytywania danych.


Blok wczytywania poleceń
========================

Blok wczytywania poleceń pozwala na efektywne sterowanie urządzeniem (bez
konieczności stałego monitorowania jego stanu przez procesor). Każde polecenie
jest pojedynczym 32-bitowym słowem wczytywanym przez DMA. Rejestry bloku
wczytywania poleceń sa następujące:

``BAR0 + 0x18: SHA1_CMD_PTR``
  32-bitowy wskaźnik na następne polecenie do wczytania. Zawsze musi być
  wyrównany do wielokrotności 4 bajtów.

``BAR0 + 0x1c: SHA1_CMD_END``
  32-bitowy wskaźnik na miejsce, gdzie procesor ma zapisać następne polecenie.
  Jeśli ``SHA1_CMD_PTR == SHA1_CMD_END``, blok wczytywania poleceń zostanie
  zatrzymany, aż procesor załaduje więcej poleceń i przesunie ``SHA1_CMD_END``.
  Zawsze musi być wyrównany do wielokrotności 4 bajtów.

Blok wczytywania poleceń będzie wczytywał polecenia zawsze, gdy zachodzą
wszystkie z następujących warunków:

- blok wczytywania poleceń jest włączony w bloku sterującym
- są jakieś nieprzetworzone polecenia, tzn. ``SHA1_CMD_PTR != SHA1_CMD_END``
- blok wczytywania danych nie jest aktywny (tzn. ``SHA1_DATA_COUNT == 0``)

Polecenie wczytywane jest z adresu ``SHA1_CMD_PTR``, jest on zwiększany o 4,
po czym polecenie jest wykonywane.

Polecenia są 32-bitowymi słowami i rozpoznawane są po najniższych bitach:

- bity 0-1 == 0: ``SHA1_JUMP`` - skok pod podany adres (tzn. ustawienie
  SHA1_CMD_PTR na podaną wartość) - następne polecenie zostanie wczytane
  z nowego adresu. Parametr to bity 2-31 polecenia, dopełnione od dołu
  dwoma bitami zerowymi.
- bity 0-1 == 1: ``SHA1_H_SAVE`` - zapis stanu ``SHA1_H`` do pamięci. Wartości
  SHA1_H zostaną zapisane po kolei pod adres podany jako parametr polecenia.
  Parametr jak w ``SHA1_JUMP``.
- bity 0-1 == 2: ``SHA1_H_LOAD`` - odczyt stanu ``SHA1_H`` z pamięci. 20 bajtów
  zostanie odczytwane z adresu podanego jako parametr polecenia i zapisane
  do ``SHA1_H``. Parametr jak w ``SHA1_JUMP``.
- bity 0-1 == 3:

  - bity 2-5 == 0: ``SHA1_DATA_PTR`` - ustawienie rejestru ``SHA1_DATA_PTR`` na
    parametr polecenia. Parametr to bity 6-31 polecenia, dopełnione od dołu
    sześcioma bitami zerowymi.
  - bity 2-5 == 1: ``SHA1_DATA_COUNT`` - ustawienie rejestru ``SHA1_DATA_COUNT``
    na parametr polecenia (parametr jak wyżej).
  - bity 2-5 == 2: ``SHA1_INTR`` - wyzwolenie wybranego przerwania użytkownika.
    Indeks wyzwolonego przerwania przechowywany jest w bitach 6-9 polecenia.
  - bity 2-5 >= 3: niepoprawne polecenie. Następuje wyzwolenie przerwania
    niepoprawnego polecenia.
