# Keyboard layout file generator for the flipperzero badusb application

## Usage

    flipperzero_badusb_kl.exe <options> [<file>]

    options are:
        -h      display this help
        -r      read layout file <file> and work from here
        -w      write layout file <file> when done
        -d      dump current keyboard layout
        -m <char> <key> <mods>
                map a key to a character. repeat option as necessary.
                    <char> is any character between ascii(32) and ascii(126)
                    <key>  is the hid key code (see -k)
                    <mods> is any combination of none, lshift, lctrl, lalt,
                           lgui, rshift, rctrl, ralt, rgui separated by a +
        -k      display valid hid key codes\
        -g      display a simplified graphical representation of the
                resulting mapping. good for a quick check, prefer -l for
                serious diagnostic
        -l      list exhaustively the content of the resulting mapping
        -e      expert mode. suppress all checks on following -m options,
                effectively allowing to put whatever you see fit into the
                keyboard layout. 

    options are applied in the following order: -r -d -m -k -g -l -w

## Requirements

### Building 

The included makefile is intended for the MingW-W64 port of GCC on Windows.

### Execution

Obviously requires a Windows computer.

## Examples

- Create a new layout file from your own current keyboard layout:

        flipperzero_badusb_kl.exe -d -w my_layout.kl

- Open an existing layout file and display its simplified view:

        flipperzero_badusb_kl.exe -r -g my_layout.kl

- Open an existing layout, set the key for $ to ctrl+shift+1, set the key for # to altgr+0, and save the modified layout:

        flipperzero_badusb_kl.exe -r -m $ 1e ctrl+shift -m # 27 ralt -w my_layout.kl

- The second argument to the -m option (1e and 27 in the example above) is the key code. See what code correspond to each key:

        flipperzero_badusb_kl.exe -k

- Just display a simplified view of the current keyboard layout, without saving anything:

        flipperzero_badusb_kl.exe -d -g

## .kl file format

A .kl file is a binary file format which stores, for each ASCII code point in order from *ASCII(0)* to *ASCII(127)*, a 16 bit little endian value composed of a *USB HID code* in the low order byte and a *bit set of modifier keys* in the high order byte. 

*USB HID codes* for each key of a standard 102/105 key keyboard are (as displayed passing option `-k` to the software):


         ---------------------------------------------------------------------- 
        | 35 | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 2d | 2e |  <- |
         ---------------------------------------------------------------------- 
        |  -> | 14 | 1a | 08 | 15 | 17 | 1c | 18 | 0c | 12 | 13 | 2f | 30 |    |
         ------------------------------------------------------------------|   |
        |   o  | 04 | 16 | 07 | 09 | 0a | 0b | 0d | 0e | 0f | 33 | 34 | 31 |   |
         ---------------------------------------------------------------------- 
        |  ^  | 64 | 1d | 1b | 06 | 19 | 05 | 11 | 10 | 36 | 37 | 38 |    ^    |
         ---------------------------------------------------------------------- 
        | ctrl | gui | alt |              2c                | alt | gui | ctrl |
         ---------------------------------------------------------------------- 

*Modifier key bits* are listed in order, starting from bit 1: *lctrl*, *lshift*, *lalt*, *lgui*, *rctrl*, *rshift*, *ralt*, *rgui*. 
