# musiscript

A musical scripting language.

## Getting started

1) Compile the program by issuing `make`
2) Write your music scripts and store them in `data` with the extension `*.msc` ("voice")
3) Execute `musiscript`
4) The output file can be found at `data/out.wav`

## The syntax

`musiscript` attempts to follow a simple and persistent syntax. The basic pattern is as follows:
```
<command> <args>
```
Indentation does not matter.

> Musiscript is "forgiving" by default. This means that it will always try to output something, even if the input is faulty (but it will still warn the user). This may lead to strange behaviour, so always check the logs!

### Commands

- `pan <0.0-1.0>`: set the stereo pan, 0.0 being left
- `volume <float>`: set the volume
- `speed <float>`: set the speed, this does not affect pitch and should be used only once at the beginning of a script.
- `sound <int>`: select the sound, as listed in `src/sounds.h` and defined in `src/sounds.cpp`
- `transpose <semitones>`: begin transposing
- `n <pitches> <durations>`: play a sound, multiple pitches can be given as a comma (no spaces) separated list,
the duration can either be 1 value or 2 (separated by a `,`), where the first dictates the base duration and the second a pause directly after the sound (for repeated, distinct notes)
- `p <duration>`: pause for the specified duration
- `end`: mark the end of a song
- `effect <effect command>`: see below
- `w <timestamp>`: waits until a certain (absolute) timestamp is reached, can be used to sync voices
- `def <varname> <value>`: defines a variable and initializes it with a value, see below
- `sub <subname>`: defines a subroutine, all commands following it are part of it until an `endsub` is reached, nested subs are possible
- `endsub`: ends a subroutine definition, see above
- `call <subname>`: calls a subroutine
- `rep <n>`: repeats the following block n times
- `endrep`: ends a `rep` block
- `echo <text>`: Log a text, useful for debugging
- `#<comment>`: a comment, ignored by parsing

### Pitches

Pitches are given using the following pattern:
```
<base pitch> <octaves> <accidentals>
```
The base pitch is simply a letter representing a key (think standard piano): `c` `d` `e` `f` `g` `a` `b`
Upper- and lowercase letters are treated as the same, and `h` is treated as `b.

The octaves are self-explanatory, as they define the octave in which a pitch should be played. Here, `A4` is at 440 hz.

For accidentals, a `+` or a `#` represents a sharp tone, a `-` or a `b` a flat one. As an example: `b4-`

Alternatively, you may pass a frequency as a floating point number with an `r` prefix, for example: `r440'

### RPN

Musiscript uses the Reverse Polish Notation (RPN) as its main way of interpreting user inputs. You can enter RPN mode by using square brackets. Anything inbetween said brackets will be interpreted as RPN. Nested RPNs are not allowed, nor are they necessary. Here's an example:

```
# Define the ration between two adjacent notes
def halftonestep [2 1 12 / ^]
echo $halftonestep
```

### Variables

Variables can be declared as seen above. Internally speaking, they all are strings (incl. spaces etc.)! But as they are essentially replaced by the line preprocessor, they can still be used as numbers and other types. They can also be redefined, allowing for some funny sounds:


```
sound TRIANGLE
speed 48

def halftonestep [2 1 12 / ^]

sub octave
    rep 12
        n r$currfreq 1
        def currfreq [$currfreq $halftonestep *]
    endrep
endsub

rep 4
    def currfreq 440
    call octave
endrep
```
 
### Effects
 
Effects are pieces of code that can change the frequency, amplitude, etc. of a sound. There are 4 effect commands:
 - `a <effect name/index> <settings>`: Add an effect by its name or index (deprecated), followed by multiple floating point numbers that serve as the settings of the effect
 - `r <effect name/index>`: Remove the first effect by its name or index
 - `ra <effect name/index>`: Remove all effects with a specific name or index
 - `c`: Clear the effect stacl
Effects are applied in the same order that they were added.
