**FilamentMonitor** to autorski projekt urządzenia IoT opartego na mikrokontrolerze ESP32, którego zadaniem jest monitorowanie poziomu wilgotności i temperatury wewnątrz pojemnika do przechowywania filamentu do druku 3D. 

---

## Motywacja i geneza projektu

Jako świeżo upieczony posiadacz drukarki 3D szybko przekonałem się, jak kluczowe dla jakości wydruków i drożności dyszy jest odpowiednie przechowywanie filamentu. Aby chronić go przed wilgocią, stworzyłem własne pudełko (drybox) z uszczelką i pochłaniaczem wilgoci. Fabryczne zatrzaski okazały się jednak za słabe, dlatego zaprojektowałem i wydrukowałem dedykowane zaciski (ich modele 3D znajdziesz w tym repozytorium!).

Gdy stworzyłem już mechaniczne zabezpieczenie, przyszedł czas na kontrolę warunków. Choć gotowe czujniki wilgotności można kupić za grosze, postanowiłem zbudować własne urządzenie od zera. To dla mnie idealna okazja, aby wejść w świat mikrokontrolerów ESP32 i po raz pierwszy zaprogramować ten układ.

---

## Założenia projektu

### Faza 1 (Aktualna):
* Wykonywanie cyklicznych pomiarów temperatury i wilgotności.
* Wyświetlanie aktualnych danych na ekranie OLED po naciśnięciu fizycznego przycisku (w celu oszczędzania energii).
* Nauka podstaw programowania ESP32.

### Faza 2 (Plany na przyszłość):
* Przesyłanie danych do chmury.
* Stworzenie prostej aplikacji mobilnej lub strony WWW do stałego monitoringu.
* System powiadomień na telefon, gdy wilgotność przekroczy bezpieczną granicę.

---

## Status projektu
Aktualnie zamówiłem wszystkie niezbędne komponenty elektroniczne i czekam na dostawę. Kolejnym krokiem będzie złożenie pierwszego prototypu na płytce stykowej.
