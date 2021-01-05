# Pixla 2 Virtual Analog Synth Music Tracker

## Tracker commands

* `1xx` Portamento up, parameter value specified as 1/4 halfnote per row
* `2xx` Portamento down, paremeter value specified as 1/4 halfnote per row
* 'Fxx' Set fast song BPM, 256 + value (256-511). Overrides previous Fxx/Txx.
* 'Txx' Set song BPM (1-255, 0 has no effect). Overrides previous Fxx/Txx.

## PX2 file format

The `.px2` file format is a simple format consisting of 1 or
more 32-bit words. The contents of some words are context sensitive. The overall
file structure is: 

```
Header word
Word1 
Word2
... 
```
### Header word 
A `.px2` file starts with the header word. It is the word `0x324C5850` in little endian format,
or the four characters `PXL2`.

### Track data

When bit 31 is zero, the word contains track data on the following format:

`0ppppppp rrrrrrrr vvvvvvvv iiiiiiii`

* `ppppppp` MIDI note pitch 2-127, or note off (1). 0 is not used.
* `rrrrrrrr` Row of current track, 0-255.
* `vvvvvvvv` Velocity of note, 0-255.
* `iiiiiiii` Instrument number of note, 0-255.

Note that current track, current pattern or extended note modifiers are defined
using dedicated words.

### Metadata

When bit 31 is one, the words contains metadata. Bit 24-30 defines
the type of metadata. 

#### Synth data (metadata id=0)

Metadata id 0 defines synth settings on the following format:

`10000000 iiiiiiii pppppppp vvvvvvvv`

* `iiiiiiii` The instrument number (0-255)
* `pppppppp` The parameter number (0-255)
* `vvvvvvvv` The parameter value (0-255)

Parameter numbers _TBD_.  

#### Sampler data (metadata id=1)

Metadata id 1 defines sampler settings on the following format:

`10000001 iiiiiiii pppppppp vvvvvvvv`

* `iiiiiiii` The instrument number (0-255)
* `pppppppp` The parameter number (0-255)
* `vvvvvvvv` The parameter value (0-255)

Parameter numbers _TBD_.  

#### Mixer data (metadata id=2)

Metadata id 2 defines mixer settings on the following format.

`10000010 00000000 pppppppp vvvvvvvv`

* `pppppppp` The parameter number (0-255)
* `vvvvvvvv` The parameter value (0-255)

#### Pattern context (metadata id=3)

Metadata id 3 specifies the current pattern context. Settings related to
pattern will be applied to this pattern.

`10000011 00000000 pppppppp pppppppp`

* `pppppppppppppppp`  The pattern number to use (0-65535)

#### Arrangement data (metadata id=4)

Metadata id 4 specifies one song position arrangement data.
Song length and arrangement position increases after each entry. If the file contains no arrangement
data, the reader should assume a song of length 1 with pattern 0 at position 0.

`10000100 00000000 pppppppp pppppppp`

* `pppppppppppppppp` The pattern number at the current position (0-65535).

#### Track context (metadata=5)

Metadata id 5 specifies the current track context. Settings related to 
track will be applied to this track.

`10000101 00000000 00000000 tttttttt`
* `tttttttt`  The track number to use (0-255)

#### Command data (metadata=6)

Metadata id 6 specifies a command for a specific row in the current track/pattern.

The format is:

`10000110 rrrrrrrr cccccccc pppppppp`

* `rrrrrrrr` The row number, 0-255.
* `cccccccc` The command number, 0-255.
* `pppppppp` The parameter value, 0-255. 
