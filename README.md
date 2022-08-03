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

## Examples

- Create a new layout file from your own current keybaord layout:

        flipperzero_badusb_kl.exe -d -w my_layout.kl

- Open an existing layout file and display its simplified view:

        flipperzero_badusb_kl.exe -r -g my_layout.kl

- Open an existing layout, set the key for $ to ctrl+shift+1, set the key for # to altgr+0, and save the modified layout:

        flipperzero_badusb_kl.exe -r -m $ 1e ctrl+shift -m # 27 ralt -w my_layout.kl

- The second argument to the -m option (1e and 27 in the example above) is the key code. See what code correspond to each key:

        flipperzero_badusb_kl.exe -k
