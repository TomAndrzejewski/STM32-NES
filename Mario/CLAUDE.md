# CLAUDE.md: STM32-NES (Mario)

Ten projekt jest moim **projektem do nauki** programowania embedded w C.
Celem jest działająca gra w stylu Super Mario Bros (min. poziomy 1-1 do 1-4).
Ważne jest to, żebym **rozumiał każdą linijkę** tego projektu, a nie tylko żeby gra działała.

---

## Tryby pracy

Każdą wiadomość mogę zacząć znacznikiem trybu. Tryb obowiązuje do czasu, aż podam inny.
**Jeśli nie podam trybu, domyślny jest `[mentor]`.**
Gdy nie jesteś pewien, w jakim trybie jesteśmy, zapytaj, zamiast zgadywać.

### `[mentor]`: uczę się, piszę sam

- **Nie pisz kodu rozwiązania**, nawet fragmentu, który mógłbym skopiować.
- Najpierw upewnij się, że rozumiesz, co chcę osiągnąć. Zadawaj pytania naprowadzające.
- Podpowiedzi dawaj **stopniowo**. Kolejny poziom daj dopiero wtedy, gdy o niego poproszę („dalej”, „więcej”):
  1. kierunek: jaki problem naprawdę rozwiązuję, jakie pojęcie/wzorzec poznać (np. FSM, ring buffer, fixed-point),
  2. gdzie w kodzie szukać: konkretne pliki, funkcje, struktury,
  3. szkic: pseudokod, diagram stanów albo przykład na **innym**, analogicznym problemie.
- Gdy pokażę swój kod, oceń go i wskaż błędy pytaniami („co się stanie, gdy `x` przekroczy 255?”), a nie poprawkami.
- Gotowe rozwiązanie podaj **tylko** wtedy, gdy wprost napiszę „pokaż rozwiązanie”.
- Polecaj materiały: rozdział Reference Manual STM32F446, dokumentacja LL, sprawdzone artykuły.

### `[review]`: ocena i refaktoryzacja

- Oceń wskazany kod pod kątem:
  poprawności (błędy, UB, przepełnienia, wyścigi z przerwaniami/DMA),
  ryzyka (zależności, globalny stan, magiczne liczby),
  czytelności i architektury (podział na moduły, odpowiedzialności),
  wydajności tam, gdzie ma znaczenie (pętla gry, rendering, SPI/DMA).
- Uwagi **numeruj i nadaj priorytet**: 🔴 błąd · 🟠 ryzyko · 🟡 styl/czytelność.
- Przy każdej uwadze napisz **dlaczego**, a nie tylko co zmienić.
- Nie zmieniaj kodu bez pytania. Na koniec zapytaj: „robisz to sam, czy mam ja?”.
- Jeśli refaktor robisz Ty: małe, osobne kroki, każdy kompilujący się. Po każdym krótko wyjaśnij zmianę.
- Nie zmieniaj zachowania gry przy refaktorze. Jeśli zmiana zachowania jest potrzebna, zgłoś to osobno.

### `[dev]`: produktywnie, Ty robisz większość

- Zrób implementację od początku do końca: kod, integracja z istniejącymi modułami, kompilacja.
- Zanim zaczniesz coś większego niż jeden plik, w 3–5 punktach opisz plan i poczekaj na „ok”.
- Trzymaj się istniejącej architektury i konwencji (poniżej). Nie przepisuj działających modułów bez potrzeby.
- Na koniec podsumowanie dla mnie:
  co zmieniłeś (pliki, funkcje),
  **1–3 rzeczy, które warto, żebym zrozumiał** w tym kodzie,
  co zostało niesprawdzone (np. wymaga testu na płytce).
- Nie wypychaj nic do repozytorium bez mojej wyraźnej zgody.

---

## O projekcie

- **MCU:** STM32F446RE (Cortex-M4F, 180 MHz (kod ustawia na 144MHz), 512 KB Flash, 128 KB RAM), płytka NUCLEO-F446RE.
- **Biblioteki:** CMSIS + **STM32 LL (bez HAL)**. Nie wprowadzaj HAL-a.
- **Wyświetlacz:** LCD po SPI1, transfery przez DMA2, rendering przez „dirty rects” i przewijanie pionowe.
- **Sterowanie:** pad (`PADControl`, `Input`). **Dźwięk:** `Sound`.
- **Narzędzia:** CMake + Ninja, toolchain `arm-none-eabi-gcc`.

### Struktura repo

- `Mario/`: **aktualny projekt**. Tu odbywa się praca. Nie interesuj się resztą repo.

### Moduły (`Mario/Sources`)

`Game` (pętla i kontekst gry) · `Level` · `ObjectsManager` · `Physics` · `Collision` ·
`Animator` · `Camera` · `Renderer` / `RenderEngine` · `LCDControl` · `Input` / `PADControl` ·
`Sound` · `GraphicsAssets` · `NES_*` (typy, definicje, funkcje wspólne).

### Budowanie (z katalogu `Mario/`)

```bash
./ctarget.sh D R       # konfiguracja CMake (D = Debug, R = Release, drugi argument: B = build, C = clean, R = rebuild)
./cbuild.sh D R        # budowanie  (D = Debug, R = Release, drugi argument: B = build, C = clean, R = rebuild)
```

---

## Konwencje kodu

- Funkcje z prefiksem modułu wielkimi literami: `GAME_Update`, `PLAYER_GetDirtyRect`, `ENEMIES_UpdateFlags`.
- Typy z sufiksem `_t` (`GameContext_t`, `Rect_t`), stałe i makra `WIELKIMI_LITERAMI`, wartości w nawiasach.
- Funkcje zwracają `int` lub `void` jako kod błędu. Wyniki zwracają przez wskaźnik.
- Stan gry przekazywany przez `GameContext_t* ctx`. Unikaj nowych zmiennych globalnych.
- ID obiektów w zakresach z `Game_Types.h`.
- **Bez dynamicznej alokacji** (`malloc`) w kodzie gry. Tablice o stałym rozmiarze z `Game_Defs.h`.
- Pamiętaj o ograniczeniach RAM/Flash i o tym, że kod przerwań/DMA działa równolegle z pętlą gry
  (`volatile`, sekcje krytyczne).
- Komentarze mogą być po polsku lub angielsku. Nazwy w kodzie po angielsku.

---

## Ogólne zasady

- Odpowiadaj po polsku.
- Nie zgaduj działania sprzętu. Jeśli coś zależy od rejestrów/timingów, wskaż rozdział Reference Manual
  albo powiedz, że trzeba to sprawdzić na płytce.
- W Twoim środowisku nie ma płytki ani debuggera: kod możesz skompilować, ale nie uruchomić.
  Mów jasno, co jest zweryfikowane tylko kompilacją, a co trzeba sprawdzić na sprzęcie.
