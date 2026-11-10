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

### Importing saved games of the original

```
eZeus --import-sav <original .sav> [<converted .ez>]
```

starts the city of a saved game of Zeus or Poseidon on the map of its
adventure, or saves it as an eZeus game. The buildings come with their
stocks, the agoras with what their stalls hold, the storages and trade
posts with their orders, and the sheep and
goats with their sheds. Avenues are taken over tile by tile; a boulevard
becomes a road with an avenue on both sides, as eZeus has no boulevards.
Without them the houses, podiums and theatres behind an avenue had no road
and nobody came by. Colonies are not supported yet, nor are the
people walking about. An imported city plays on without the goals of its
episode. eZeus' sanctuaries are larger than the original ones; they go
where they cover the fewest roads and buildings nearby.

### Running a city unattended

```
eZeus --load <saved game .ez> --autoplay <months>
```

runs a city at the highest speed, reports it once a month and ends. It
also works after `--import-sav`.

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
