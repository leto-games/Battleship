# Battleship
Classic strategy game for the LETO gaming console

## Building on Linux

The game is built against the LETO core libraries, which must be cloned next to each
other in one folder. `LETO_PATH` points at that folder; the trailing slash is required.

```sh
export LETO_PATH="$HOME/leto/"
```

Build the core in order: every step is built against the artifacts of the previous one.

```sh
cd "$LETO_PATH/LetoAPI"  && cmake --preset linux-debug && cmake --build --preset linux-debug
cd "$LETO_PATH/LetoCore" && cmake --preset linux-debug && cmake --build --preset linux-debug
cd "$LETO_PATH/LetoOS"   && cmake --preset linux-debug && cmake --build --preset linux-debug
```

Then the game itself:

```sh
cd "$LETO_PATH/Battleship" && cmake --preset linux-debug && cmake --build --preset linux-debug
```

On Linux the game is a shared library (`libBattleship.so`). The build copies it to
`$LETO_PATH/Apps/linux-debug/Battleship/` — the layout the console reads games from.

The game links `libLetoAPI.a` and `libLetoCore.a` statically and packs them into the `.so`.
Those libraries are compiled as position-independent code (`CMAKE_POSITION_INDEPENDENT_CODE`
in the `linux-base` preset), because non-PIC objects cannot be linked into a shared object.
`LetoOS` is not built with PIC on purpose: a game links only `LetoAPI` and `LetoCore`, and
the console itself is an executable.
