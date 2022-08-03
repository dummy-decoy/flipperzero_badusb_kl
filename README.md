# Keyboard layout file generator for the flipperzero badusb application

## Usage

    flipperzero_badusb_kl.exe <options> [<file>]

    options are:
        -h      display this help
        -r      read layout file <file> and work from here
        -w      write layout file <file> when done
        -d      dump current keyboard layout
        -m <char> <key> <mods>
                maps a key to a character. repeat option as necessary.
                    <char> is any character between ascii(32) and ascii(126)
                    <key>  is the hid key code (see -k)
                    <mods> is any combination of none, lshift, lctrl, lalt,
                           lgui, rshift, rctrl, ralt, rgui separated by a +
        -k      displays valid hid key codes
        -l      lists the resulting mapping

## Requirements

### Building 

The included makefile is intended for the MingW-W64 port of GCC on Windows.

### Execution

Obviously requires a Windows computer.
