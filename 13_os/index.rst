========================================
Zajęcia 13-15: Piszemy system operacyjny
========================================

Daty:

- 20.05.2014 (start, obsługa ekranu)
- 22.05.2014 (GDT, obsługa przerwań, przesył danych z klawiatury)
- 27.05.2014 (przełączanie kontekstów, wielowątkowość, ?)
- 28.05.2014 (?)
- 03.06.2014 (zadanie poprawkowe, ?)
- 04.06.2014 (zadanie poprawkowe, ?)


.. toctree::
   :hidden:

   zadanie
   15_zadanie


Materiały dodatkowe
===================

- :ref:`13-zadanie`
- :ref:`15-zadanie`
- http://github.com/koriakin/zso-os/


Wprowadzenie
============

Ostatnie 6 zajęć (3 tygodnie po 2 grupy) przeznaczymy na pisanie od podstaw
własnego, prostego systemu operacyjnego. Założenia:

- kod systemu jest publicznie dostępny na githubie (użytkownik ``koriakin``,
  repozytorium ``zso-os``), a autorzy modułów dostaną dostęp do zapisu
- piszemy w całości w czasie zajęć (chyba, że ktoś bardzo chce napisać jakiś
  moduł w domu)
- obie grupy pracują nad tym samym kodem, ale na początku każdych zajęć będzie
  krótka powtórka z funkcjonalności zrealizowanej w drugiej grupie
- zajmę się głównymi częściami systemu (start, obsługa procesora, przełączanie
  kontekstów), ale proszę studentów o pomoc w napisaniu pozostałych modułów
- moduły do napisania będą ogłaszane na zajęciach, proszę o zgłaszanie się
  ochotników
- *każdy* napisany moduł liczy się jako jedno małe zadanie (czyli dodatkowy
  dzień spóźnienia)
- nie będziemy specjalnie przejmować się jakością kodu i wcale nie będziemy
  przejmować się wydajnością kodu - chodzi nam o pokazanie w jak najkrótszym
  czasie jak największej liczby elementów systemu operacyjnego
- piszemy na architekturę i386, obsługujemy starożytny sprzęt klasy PC (tzn.
  grafika VGA w trybie tekstowym, klawiatura PS/2, itp.) - niestety obsługa
  współczesnych interfejsów sprzętowych jest zbyt skomplikowana na 12 godzin
  zajęć
- głównie chodzi nam o pokazanie obsługi procesora, w mniejszym stopniu innego
  sprzętu i mechanizmów komunikacji międzyprocesowej, a w najmniejszym stopniu
  wyższych warstw systemu (np. protokoły sieciowe, systemy plików)
- na naszym systemie operacyjnym ma działać tetris

Proponowany zakres (zobaczymy, jak wyjdzie czasowo):

- szkielet obrazu w formacie multiboot do uruchomienia przez GRUB lub inny
  bootloader (zrobione - zajęcia 1)
- obsługa VGA, wypisywanie tekstu na VGA (zrobione - zajęcia 1, Jan Wróblewski)
- tablice deskryptorów, obsługa przerwań, kontroler przerwań (zrobione - zajęcia
  2)
- obsługa klawiatury (częściowo zrobione - zajęcia 2)
- parsowanie struktury multiboot i alokator pamięci (zlecone)
- tetris (planowane - zajęcia 3, poszukiwany ochotnik)
- przełączanie kontekstów + wywłaszczanie w przerwaniu zegarowym (planowane -
  zajęcia 3)
- wzajemne wykluczanie (planowane - zajęcia 3)
- blokowanie wątków, prosta komunikacja między wątkami (planowane - zajęcia 3)
- stronicowanie i alokator pamięci na stronach (planowane - zajęcia 4)
- podział na przestrzeń jądra i użytkownika (planowane - zajęcia 5)
- obsługa zegara, alarmy (planowane - zajęcia 6)
- shell (w zapasie)
- jakieś proste programy mogące działać pod naszym systemem (potrzebni
  ochotnicy)
- jakiś lekki interpreter języka skryptowego (potrzebny ochotnik)
- mysz PS/2 (w zapasie)
- dyski IDE (w zapasie)
- system plików FAT (w zapasie)
- inne pomysły?

Czego na pewno nie będziemy robić (za dużo pracy):

- wieloprocesorowość
- zarządzanie energią i dowolna inna funkcjonalność związana z ACPI
- magistrala PCI/USB i dowolny sprzęt, który by jej wymagał
- TCP/IP (ale UDP/IP może być do zrobienia, jeżeli znajdzie się ochotnik)
- tryb graficzny
- system plików w trybie do zapisu
