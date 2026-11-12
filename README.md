<h1 align="center">eZeus</h1>

eZeus is an open-source implementation of Zeus: Master of Olympus game

[Watch trailers](https://www.youtube.com/@eZeus-e)

Please note that eZeus is not an exact copy of the original game. There are some minor differences, most of which are accidental, as well as optional additional features (such as a new walker implementation and the ability to build two cities at once). eZeus supports the original game’s adventures, and the overall experience should be similar to the original.

## How To Play

To play eZeus you will need original game files (Zeus: Master of Olympus base game and Poseidon expansion).

## Languages

eZeus takes most of its text straight from the original game files, so the
in-game language follows the language of your Zeus and Poseidon
installation. Its own interface texts live in `Text/language.txt`.

English, German and Polish are supported. Pick the language in the settings
menu, or set it in `settings.txt`:

```
language "de"
```

Accepted values are `en`, `de` and `pl`. A translation may be incomplete -
any key missing from `Text/language_<code>.txt` falls back to the English
text in `Text/language.txt`.

The original game stores its text in the ANSI code page of its release
(Windows-1252 for the Western European versions, Windows-1250 for Polish),
which eZeus converts to UTF-8 while loading. Detection is automatic; should
a file be misread, the code page can be forced in `settings.txt`:

```
encoding "cp1252"
```

Accepted values are `auto` (default), `utf8`, `cp1252` and `cp1250`.

### Adding a language

Copy `Text/language.txt` to `Text/language_<code>.txt`, translate the values
and save the file as UTF-8. Add the language to `elanguageid.cpp`, and make
sure `Fonts/Zeus.ttf` covers its glyphs - the bundled font covers Western
and Central European Latin only.

Only windows binaries are provided. If you want to play on Linux you have to build eZeus yourself.

[You can download eZeus from GitHub](https://github.com/MaurycyLiebner/eZeus/releases)

## Changes in this fork

### Desirability follows the original model

This changes the balance compared to upstream eZeus. Upstream gave every
building a value and a range, let the value fall off linearly to the range
and halved it, while the houses needed little to evolve. A city taken over
from a saved game of the original sat several levels lower in eZeus: in one
of them 52 of 96 townhouses fell to level 2, although the original grid of
that save puts all but 3 of them above the 25 a townhouse needs.

eZeus now spreads desirability the way the original does, as read off the
desirability grid of its saved games:

- every building uses value, range, rings per step and change per step from
  `Model\Zeus_Model_*.txt` of the original game,
- the value spreads in square rings, starting next to the building, up to 6
  rings, and every tile stays between -100 and 100,
- a house reads its best tile,
- a house keeps a level above the "devolve" value and rises above the
  "evolve" value of the model, which differ per difficulty,
- the agora counts with its own value, its stalls have none.

Two values are not in the model and are estimates: god monuments spread
like commemorative monuments, and roads are drawn paved above 20. Onion and
carrot farms had swapped values before.

### Agora stalls keep less

Upstream let a stall for fleece, oil or wine keep ten loads. The stalls of
the original never hold more than four in its saved games, and with ten
the agoras next to the warehouses took a whole year's supply before the
others got any. A stall now keeps four loads
(`basic_vendor_max_resource_i`). Food stays at ten; the original keeps up
to eleven.

### Peddlers leave sooner

Upstream lets every walker building wait 5000 ticks before it sends the
next walker, and a walker roams 40 tiles. In the saved games of the
original four of five agoras have their peddler out, and a peddler turns
back after 43 tiles; two saves 19 days apart put the pause between two
trips at the time it takes to walk 12 to 17 tiles. An agora now waits
3000 ticks (`peddler_wait_time_i`) and its peddler roams 43 tiles
(`peddler_max_distance_i`). Houses run out of fleece about half as often.
The other walkers are unchanged.

### Importing saved games of the original

```
eZeus --import-sav <original .sav> [<converted .ez>]
```

starts the city of a saved game of Zeus or Poseidon on the map of its
adventure, or saves it as an eZeus game. The buildings come with their
stocks, the agoras with what their stalls hold, the storages and trade
posts with their orders, the houses with their food, fleece, oil and wine, and
the sheep and goats with their sheds. Wild animals standing where a
building goes are removed. Avenues are taken over tile by tile; a boulevard
becomes a road with an avenue on both sides, as eZeus has no boulevards.
Without them the houses, podiums and theatres behind an avenue had no road
and nobody came by. Colonies are not supported yet, nor are the
people walking about. An imported city plays on without the goals of its
episode. eZeus' sanctuaries are larger than the original ones; they go
where they cover the fewest roads and buildings nearby.

### Larger maps and a map generator

The maps of the original go up to 228 x 228 tiles, half of which lie
inside the playable diamond. eZeus keeps the whole map in a grid of 114 x
228 "dtiles" and the editor can resize a board up to 999 x 999, which is
38 times the area of the largest original map. A city of 3500 people runs
at the same speed on a board of 16 times the original area, saves in a
few hundred milliseconds and loads again, so there is no limit to raise
here, only the empty space to fill. Two things stood in the way and are
fixed: resizing wiped the desirability map until a building changed, so
the houses devolved and the people left, and the copies of the board that
the worker threads use stayed empty after a resize.

The map generator of the editor (board settings, "Generate") no longer
needs libnoise and so works on Windows too. It is built from the maps of
the original adventures: `eZeus --map-stats` reads all 80 boards of the
adventures and prints what they are made of, and the defaults of the
generator are the medians of that - 22% water, 8% forest, 3% fertile
land, 2% stones, 4% of the land raised, a river on most maps, a lake on
one in four, about three animal points and two fishing points on the
largest maps. The counts are given per 26000 tiles (the largest original
map) and scale with the area, so a map four times as large gets four
times as many rivers, lakes, ore patches, animals and fish. The generator
makes the sea along one or two sides with beaches, rivers like those of
the original maps - five to nine tiles wide, meandering from a side of
the map to the sea or right across, some with a tributary, with their
entry and exit points - lakes, meadows in large patches with a liking
for the rivers and lakes, forests, hills with slopes, stone fields with
copper, silver, marble and orichalc, animal and fishing points and the
entry and exit of the map. The whole map becomes the territory of the
chosen city. Marble at the edge of the map crashed eZeus when the
marble groups were collected; fixed.

```
eZeus --generate <width> <height> [<seed>]
```

generates a map without the editor, prints its statistics and writes a
picture of it as `generated.ppm`, which is how the generator was tuned.

### Running a city unattended

```
eZeus --load <saved game .ez> --autoplay <months>
```

runs a city at the highest speed, reports it once a month and ends. It
also works after `--import-sav`. `--load` may be followed by
`--resize <width> <height>`, which changes the size of the board, and
`--save <.ez>`, which saves the game right away.

## How To Build on macOS

You need these dependencies for x86 and ARM Builds: 
```
softwareupdate --install-rosetta
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install sdl2 sdl2_mixer sdl2_image sdl2_ttf git qt make dylibbundler fluid-synth
arch -x86_64 /usr/local/bin/brew install sdl2 sdl2_mixer sdl2_image sdl2_ttf git qt make dylibbundler fluid-synth
```
Clone the eZeus repository:
```
git clone --recurse-submodules https://github.com/MaurycyLiebner/eZeus
```
Than simply execute compile_apple.sh script. All is done automatic. At the End you should have 2 new folders. build_arm and build_x86. Follow the instructions at the end of the script.

Or you could DL ready to go Binaries straight forward here:
https://github.com/MacThings/eZeus/releases/tag/Zeus-Binaries

## How To Build on Linux

Make sure you have **git** installed on your system.

Clone the eZeus repository:
```
git clone --recurse-submodules https://github.com/MaurycyLiebner/eZeus
```

You will need to install following packages:

```
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev libnoise-dev g++
```

You will need Qt Creator (qt.io).

Open eZeus.pro in Qt Creator and use it to build eZeus.

## How To Build With CMake

The CMake build fetches SDL2 and its satellite libraries itself, so no SDL
packages have to be installed:

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target eZeus
```

CMake 4 rejects the vendored freetype that SDL_ttf 2.24 ships, because it
still asks for compatibility with CMake below 3.5. Until that dependency is
updated, add `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` to the configure step.

Before running eZeus binary make sure to setup all files as specified in **How To Play** section and the release description. Put eZeus binary in (...)/Zeus and Poseidon/eZeus/Bin directory and run it.

## Authors

**Maurycy Liebner** - 2021 - 2025 - [MaurycyLiebner](https://github.com/MaurycyLiebner)
