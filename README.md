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
