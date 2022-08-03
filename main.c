#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>


#define log(stream, ...) fprintf(stream, __VA_ARGS__)
#define log_error(...) log(stderr, "error: "__VA_ARGS__)
#define log_warning(...) log(stderr, "warning: "__VA_ARGS__)


typedef uint16_t layout_key_t;
typedef layout_key_t layout_t[128];

static const layout_key_t vsc_to_hid[49][2] = {
    {0x29,0x35},{0x02,0x1e},{0x03,0x1f},{0x04,0x20},{0x05,0x21},{0x06,0x22},{0x07,0x23},{0x08,0x24},{0x09,0x25},{0x0a,0x26},{0x0b,0x27},{0x0c,0x2d},{0x0d,0x2e},
    {0x10,0x14},{0x11,0x1a},{0x12,0x08},{0x13,0x15},{0x14,0x17},{0x15,0x1c},{0x16,0x18},{0x17,0x0c},{0x18,0x12},{0x19,0x13},{0x1a,0x2f},{0x1b,0x30},
    {0x1e,0x04},{0x1f,0x16},{0x20,0x07},{0x21,0x09},{0x22,0x0a},{0x23,0x0b},{0x24,0x0d},{0x25,0x0e},{0x26,0x0f},{0x27,0x33},{0x28,0x34},{0x2b,0x31},
    {0x56,0x64},{0x2c,0x1d},{0x2d,0x1b},{0x2e,0x06},{0x2f,0x19},{0x30,0x05},{0x31,0x11},{0x32,0x10},{0x33,0x36},{0x34,0x37},{0x35,0x38},{0x39,0x2c}
};


static bool read_layout(const char* path, layout_t layout) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        log_error("cannot open layout file for reading\n");
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    if (ftell(file) != sizeof(layout_t)) {
        log_error("input layout file has the wrong size\n");
        return false;
    }
    fseek(file, 0, SEEK_SET);

    if (fread(layout, 1, sizeof(layout_t), file) != sizeof(layout_t)) {
        log_error("input layout file read failed\n");
        return false;
    }

    fclose(file);
    return true;
}

static bool write_layout(const char* path, layout_t layout) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        log_error("cannot open layout file for writing\n");
        return false;
    }

    if (fwrite(layout, 1, sizeof(layout_t), file) != sizeof(layout_t)) {
        log_error("output layout file write failed\n");
        return false;
    }

    fclose(file);
    return true;
}

static void set_default_layout(layout_t layout) {
    layout_t default_layout = {
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x002A,0x002B,
        0x0028,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
    };
    memcpy(layout, default_layout, sizeof(layout_t));
}

static layout_key_t char_to_virtual_scan_code(const char chr) {
    SHORT virtual_key = VkKeyScan(chr);
    SHORT modifiers = (virtual_key & 0xff00);
    UINT  scan_code = MapVirtualKey((virtual_key & 0xff), MAPVK_VK_TO_VSC);
    return (layout_key_t)(scan_code|modifiers);
}

static layout_key_t virtual_scan_code_to_hid_code(const layout_key_t vscm) {
    layout_key_t vsc = (vscm & 0xff); // strip modifiers
    size_t index;
    for (index = 0; index < sizeof(vsc_to_hid)/sizeof(*vsc_to_hid); ++index) {
        if (vsc_to_hid[index][0] == vsc)
            break;
    }
    if (index < sizeof(vsc_to_hid)/sizeof(*vsc_to_hid))
        return vsc_to_hid[index][1];
    return 0;
}

static layout_key_t virtual_scan_code_to_hid_modifiers(const layout_key_t vscm) {
    layout_key_t vsc = (vscm & 0xff00) >> 8; // strip code
    layout_key_t mods = 0;
    
    mods |= (vsc & 0x01 ? (1 << 9) : 0); // shift
    mods |= (vsc & 0x02 ? (1 << 8) : 0); // ctrl
    mods |= (vsc & 0x04 ? (1 << 10): 0); // alt

    // treat ctrl + alt as ralt 
    if ((mods & (1<<8)) && (mods & (1<<10))) 
        mods = mods & ~((1<<8)|(1<<10)) | (1<<14);
    
    return mods;
}

static layout_key_t make_hid_code(const layout_key_t code, const layout_key_t modifiers) {
    return (code & 0xff) | (modifiers & 0xff00);
}

static layout_key_t char_to_hid_code(char chr) {
    layout_key_t vscm = char_to_virtual_scan_code(chr);
    return make_hid_code(virtual_scan_code_to_hid_code(vscm), virtual_scan_code_to_hid_modifiers(vscm));
}

static void usage() {
    printf("usage: \n"
           "    flipper_kl <options> [<file>]\n"
           "\n"
           "    options are:\n"
           "        -h      display this help\n"
           "        -r      read layout file <file> and work from here\n"
           "        -w      write layout file <file> when done\n"
           "        -d      dump current keyboard layout\n"
           "        -m <char> <key> <mods>\n"
           "                maps a key to a character. repeat option as necessary.\n"
           "                    <char> is any character between ascii(32) and ascii(126)\n"
           "                    <key>  is the hid key code (see -k)\n"
           "                    <mods> is any combination of none, lshift, lctrl, lalt,\n"
           "                           rshift, rctrl, ralt separated by a +\n"
           "        -k      displays valid hid key codes\n" 
           "        -s      shows the resulting mapping\n"
    );
}

static void show_keys() {
    printf("hid key codes:\n"
           "    ---------------------------------------------------------------------- \n"
           "   | 35 | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 2D | 2e |  <- |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  -> | 14 | 1a | 08 | 15 | 17 | 1c | 18 | 0c | 12 | 13 | 2f | 30 |    |\n"
           "    ------------------------------------------------------------------|   |\n"
           "   |   o  | 04 | 16 | 07 | 09 | 0a | 0b | 0d | 0e | 0f | 33 | 34 | 31 |   |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  ^  | 64 | 1d | 1b | 06 | 19 | 05 | 11 | 10 | 36 | 37 | 38 |    ^    |\n"
           "    ---------------------------------------------------------------------- \n"
           "   | ctrl | alt |                  2c                        | alt | ctrl |\n"
           "    ---------------------------------------------------------------------- \n");
}

static void show_layout(layout_t layout) {
    printf("layout is:  (char : code : hid_key modifiers...)\n");
    for (size_t num = 32; num < 127; ++num) {
        layout_key_t code = layout[num];
        printf("   %c: %04x : %02x %s %s %s %s %s %s\n", (char)num, code, code & 0xff, 
               code&(1<<8)?  "lctrl" :"     ", 
               code&(1<<9)?  "lshift":"      ", 
               code&(1<<10)? "lalt"  :"    ", 
               code&(1<<12)? "rctrl" :"     ", 
               code&(1<<13)? "rshift":"      ", 
               code&(1<<14)? "ralt"  :"    "
        );
    }
}

int main(int argc, char *argv[]) {
    bool help = false;
    bool read = false;
    bool write = false;
    bool dump = false;
    bool keys = false;
    bool show = false;
    bool file = false;
    char *path = argv[1];

    int result = 0;

    if (argc == 1) {
        log_error("missing argument\n");
        result = -1;
        help = true;
    } else {
        size_t argn = 1;
        while (argn < argc) {
            if (strcmp("-h", argv[argn]) == 0) {
                help = true;
                argn++;
            } else if (strcmp("-r", argv[argn]) == 0) {
                read = true;
                argn++;
            } else if (strcmp("-w", argv[argn]) == 0) {
                write = true;
                argn++;
            } else if (strcmp("-d", argv[argn]) == 0) {
                dump = true;
                argn++;
            } else if (strcmp("-k", argv[argn]) == 0) {
                keys = true;
                argn++;
            } else if (strcmp("-s", argv[argn]) == 0) {
                show = true;
                argn++;
            } else if (strcmp("-m", argv[argn]) == 0) {
                log_warning("option -m is not yet implemented\n");
                argn++;
                if (argn >= argc) {
                    log_error("modify: missing parameters\n");
                    result = -1;
                }

                if ((strlen(argv[argn]) != 1) || ((int)argv[argn][0] < 32) || ((int)argv[argn][0] >= 127)) {
                    log_error("modify: invalid character specified\n");
                    result = -1;
                }
                char chr = argv[argn][0];
                argn++;
                if (argn >= argc) {
                    log_error("modify: missing parameters\n");
                    result = -1;
                }

                char *end;
                layout_key_t code = strtol(argv[argn], &end, 16);
                if (((end-argv[argn]) != strlen(argv[argn])) || (code == 0)) {
                    log_error("modify: invalid hid key code specified\n");
                    result = -1;
                }
                argn++;
                if (argn >= argc) {
                    log_error("modify: missing parameters\n");
                    result = -1;
                }

                // parse modifiers
                layout_key_t mods;
                argn++;
            } else if (!file) {
                path = argv[argn];
                file = true;
                argn++;
            } else {
                log_error("unrecognized argument\n");
                result = -1;
                help = true;
                break;
            }
        }
    }

    if (help || (result != 0)) {
        usage();
        return result;
    }

    layout_t layout;
    set_default_layout(layout);

    if (read) {
        printf("reading layout file %s...\n", path);
        if (!read_layout(path, layout))
            return -1;
    }
    if (dump) {
        printf("dumping current keyboard layout...\n");
        for (uint8_t num = 32; num < 127; ++num) {
            layout[num] = char_to_hid_code((char)num);
        }
    }
    if (keys)
        show_keys();
    if (show)
        show_layout(layout);
    if (write) {
        printf("writing layout file %s...\n", path);
        if (!write_layout(path, layout))
            return -1;
    }

    return result;
}
