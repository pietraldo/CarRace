# rendering_3d_scene


```
git clone https://github.com/pietraldo/CarRace
cd CarRace
git submodule init
git submodule update
cmake CMakeLists.txt
```


Jasne — wrzuć to do **README.md** jako sekcję „Struktura projektu”. Poniżej gotowy fragment do skopiowania:

---

# Struktura projektu

```
CarRace/
  CMakeLists.txt
  externals/
  assets/
    models/track/
    models/car/
    textures/
    shaders/
  src/
    app/
    core/
    gfx/
    game/
    physics/
    features/
      car/
      track/
    ui/
    utils/
  include/
  tests/
  README.md
```

## Co gdzie trzymamy

### `externals/`

Zewnętrzne biblioteki: **GLFW**, **GLAD**, **GLM**, **stb\_image**, **Assimp**, **ImGui** itp.

* Formy: submodule, vendored lub prebuilt (zgodnie z CMake w repo).

### `assets/`

**Tylko dane** (żadnego C++):

* `models/track/` – siatki toru (np. `.obj/.fbx/.dae`, materiały `.mtl`).
* `models/car/` – siatki auta i akcesoriów.
* `textures/` – tekstury (`.png/.jpg`), normal/specular/roughness itp.
* `shaders/` – shadery **GLSL** (`.vert`, `.frag`, ewentualnie `.geom`).
  **Uwaga:** pliki `.cpp/.h` nie trafiają do `assets/`.

### `src/`

Kod źródłowy gry (C++). Podział „tematyczny”:

* `app/` – **entrypoint i pętla gry**:

  * `main.cpp`, konfiguracja czasu (`FixedTimeStep`), wczytanie sceny, bootstrap.
* `core/` – infrastruktura **systemowa**:

  * okno (GLFW), wejście (klawiatura/mysz → `InputState`), zegar (`Timer`), loger.
* `gfx/` – **rendering i grafika**:

  * `Renderer`, `Shader`, `Mesh`, `Model` (Assimp), `Texture` (`stb_image`), `Camera`,
  * `gfx/lights/` – światła (np. `Directional`),
  * ewentualnie `gfx/primitives/` – prymitywy/debug (linia, box).
* `game/` – **stan gry**:

  * `Scene` (lista encji), `Entity`, komponenty (`Transform`, `Tag`), ewentualnie „SceneBuilder”.
* `physics/` – **ruch i kolizje**:

  * `RigidBody`, integrator (np. semi-implicit Euler), kolizje (`Collider`, `Collision`, `raycast`),
  * prosty model 2D dla toru (korytarz z segmentów).
* `features/` – **logika domenowa** (konkretne „feature’y” gry):

  * `features/car/` – sterowanie autem (`CarController`), parametry pojazdu (`VehicleParams`, wheelbase, maxSteer, siły), helpery trakcji.
  * `features/track/` – wczytywanie toru (`TrackLoader`), granice (`Boundaries`), checkpointy/okrążenia.
* `ui/` – **interfejs użytkownika**:

  * HUD (prędkość, bieg, czas/okrążenie), debug overlay (np. ImGui).
* `utils/` – **narzędzia wspólne**:

  * math helpers, I/O (np. ładowanie JSON z parametrami), małe klasy pomocnicze.

### `include/`

Nagłówki „publiczne”, jeśli kiedyś wydzielimy biblioteki lub chcemy rozdzielić interfejs od implementacji.
Na razie **opcjonalne** (większość nagłówków trzymamy w `src/`).

### `tests/`

Testy (np. GoogleTest / Catch2): testy jednostkowe helperów, prostych algorytmów kolizji itp.
Opcjonalnie „headless” testy logiki (bez okna).

---

## Konwencje

* **Assets ≠ kod**: w `assets/` tylko dane (modele/teksty/shadery).
* **Nazwy**: pliki shaderów `*.vert` / `*.frag`; modele i tekstury pogrupowane per obiekt.
* **Include’y**: z `src/` includujemy względnie, np. `#include "gfx/Camera.h"`.
* **Renderer jest read-only**: niczego nie modyfikuje – tylko czyta stan sceny.
* **Fizyka zmienia stan**: modyfikuje `Transform`/`RigidBody`, używa stałego `dt`.
* **Feature’y nie znają renderera**: komunikacja przez `Scene`/komponenty, nie bezpośrednio.
* **Debug draw**: linie/OBB/segmenty – jako osobna ścieżka renderingu (łatwiej debugować kolizje).

---

## Przepływ w czasie uruchomienia (skrót)

1. `app/main.cpp` – inicjalizacja okna, ładowanie shaderów/bibliotek, budowa sceny (tor + auto).
2. Pętla gry: `Input` → `Physics::step(dt)` → `Renderer::draw(Scene)`.
3. `features/car` czyta `InputState`, modyfikuje `RigidBody`, fizyka integruje ruch.
4. `features/track` dostarcza granice toru i checkpointy do kolizji/logiki okrążeń.
5. `ui/` rysuje HUD.

---

Jeśli chcesz, dorzucę na koniec README krótką sekcję „Build & uruchomienie” (CMake + kopiowanie `assets/` do folderu z `.exe`).
