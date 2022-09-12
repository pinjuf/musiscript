# musiscript

A musical scripting language.

## Getting started

1) Compile the program by issuing `make`
2) Write your music scripts and store them in `data` with the extension `*.msc` ("voice")
> Note that if you are using the proprocessor, make sure your macro files DON'T end with `.msc` or they will be played as an individual voice as well. The convention is to call these files `.mscm`.
4) Execute `musiscript`
5) The output file can be found at `data/out.wav`

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
- `undef <varname>`: deletes a variable
- `sub <subname>`: defines a subroutine, all commands following it are part of it until an `endsub` is reached, nested subs are possible
- `endsub`: ends a subroutine definition, see above
- `call <subname>`: calls a subroutine
- `rep <n>`: repeats the following block n times
- `endrep`: ends a `rep` block
- `echo <text>`: Log a text, useful for debugging
- `jump <label>`: immediatly jump the the specified `label`
- `label <title>`: sets a label, see above
- `if <statement>`: see below
- `elif <statement>`: see below
- `else`: see below
-  `endif`: ends an `if
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

### Infix notation

Musiscript is capable of reading infix notation. To enter infix notation mode, put an expression in square braces.
Valid operators are `^+-*/`. Other functions are defined in the reverse polish notation parser.
> For the interested: To eval an infix statement, it is first converted to RPN, and then passed to an RPN parser.

### Variables

Variables can be declared as seen above. Internally speaking, they all are strings (incl. spaces etc.)! But as they are essentially replaced by the line preprocessor, they can still be used as numbers and other types. They can also be redefined, allowing for some funny sounds:


```
sound TRIANGLE
speed 48

def halftonestep [2*(1/12)]

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
Example:

```
# Smooth the rather harsh SAW sound a bit
sound SAW

# Normal sound
n a4 1,1

# Add SMOOTH effect over 3 samples
effect a SMOOTH 3
n a4 1,1

# Add another SMOOTH effect over 3 samples
effect a SMOOTH 3
n a4 1,1
```

### Codepointers

Codepointers are statements that let you call pieces of code (as defined in `src/codepts.cpp`) and use their return values. They can be very useful for checking conditions.
Example:

```
echo {numeq $counter $thres}
```

This will echo `true` if `$counter == $thres`, else it will output `false`.

### `if`-statements/blocks

`if`-statements engage if the following statement evaluates to true. For the evaulation, you can use codepointers in combination with `or`, `and`, `not` etc and parentheses.

`elif`-statements engage if all previous evaluations in the current `if`-block have evaluated to false and the following statement evaluates to true.

`else`-statements engage if all previous evaluations evaluated to `false`.

`endif` ends an `if`-block.

Example:

```
def value {randint 0 16}

if {numeq 0 [mod($value, 2)]}
    echo $value is even.
else
    echo $value is uneven.
endif
```

## The preprocessor

Musiscript has a preprocessor that allows you to include macros in your files.
Before reading any file, Musiscript preprocesses it.
The final product it actually parses will have the suffix `.mscpp`.
They may be deleted, depending on your `src/config.h`.

### `%include <filename>`

This allows you to include a complete file.
Note that said file will be proprocessed too, regardless of its suffix.
