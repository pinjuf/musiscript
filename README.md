# musiscript

A musical scripting language.

## Getting started

1) Compile the program by issuing `make`
2) Write your music scripts and store them in `data` with the extension `*.vc` ("voice")
3) Execute `musiscript`
4) The output file can be found at `data/out.wav`

## The syntax

`musiscript` attempts to follow a simple and persistent syntax. The basic pattern is as follows:
```
<command> <args>
```

### Commands

- `pan <0.0-1.0>`: set the stereo pan, 0.0 being left
- `volume <float>`: set the volume
- `speed <float>`: set the speed, this does not affect pitch and should be used only once at the beginning of a script.
- `sound <int>`: select the sound, as listed in `src/sounds.h` and defined in `src/sounds.cpp`
- ´transpose <semitones>`: begin transposing
- `n <pitches> <durations>`: play a sound, multiple pitches can be given as a comma (no spaces) separated list,
the duration can either be 1 value or 2 (separated by a `+`), where the first dictates the base duration and the second a pause directly after the sound (for repeated, distinct notes)
- `p <duration>`: pause for the specified duration
- `end`: mark the end of a song
- `effect`: usage as `effect a <index> <settings>` adds an effect to the effect stack by its index and initializes it with the given setting, usage as `effect c` clears the effect stack

### Pitches

Pitches are given using the following pattern:
```
<base pitch> <octaves> <accidentals>
```
The base pitch is simply a letter representing a key (think standard piano): `c` `d` `e` `f` `g` `a` `b`
Upper- and lowercase letters are treated as the same.

The octaves are self-explanatory, as they define the octave in which a pitch should be played. Here, `A4` is at 440 hz.

For accidentals, a `+` or a `#` represents a sharp tone, a `-` or a `b` a flat one.
