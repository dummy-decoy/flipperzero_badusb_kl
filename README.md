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

    options are applied in the following order: -r -d -m -k -g -l -w

## Requirements

### Building 

The included makefile is intended for the MingW-W64 port of GCC on Windows.

### Execution

Obviously requires a Windows computer.
