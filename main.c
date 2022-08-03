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

static const layout_key_t hid_mod_lctrl  = (1 <<  8);
static const layout_key_t hid_mod_lshift = (1 <<  9);
static const layout_key_t hid_mod_lalt   = (1 << 10);
static const layout_key_t hid_mod_lgui   = (1 << 11);
static const layout_key_t hid_mod_rctrl  = (1 << 12);
static const layout_key_t hid_mod_rshift = (1 << 13);
static const layout_key_t hid_mod_ralt   = (1 << 14);
static const layout_key_t hid_mod_rgui   = (1 << 15);

static const layout_key_t hid_codes[49] = {
    0x35,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x2d,0x2e,
    0x14,0x1a,0x08,0x15,0x17,0x1c,0x18,0x0c,0x12,0x13,0x2f,0x30,
    0x04,0x16,0x07,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x33,0x34,0x31,
    0x64,0x1d,0x1b,0x06,0x19,0x05,0x11,0x10,0x36,0x37,0x38,0x2c
};
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

static bool write_layout(const char* path, const layout_t layout) {
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

static bool patch_layout(layout_t layout, const char chr, const layout_key_t hid_code) {
    layout[(size_t)chr] = hid_code;
    return true;
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
    
    mods |= (vsc & 0x01 ? hid_mod_lshift : 0); // shift
    mods |= (vsc & 0x02 ? hid_mod_lctrl  : 0); // ctrl
    mods |= (vsc & 0x04 ? hid_mod_lalt   : 0); // alt

    // treat ctrl + alt as ralt 
    if ((mods & hid_mod_lctrl) && (mods & hid_mod_lalt)) 
        mods = mods & ~(hid_mod_lctrl|hid_mod_lalt) | hid_mod_ralt;
    
    return mods;
}

static layout_key_t make_hid_code(const layout_key_t code, const layout_key_t modifiers) {
    return (code & 0xff) | (modifiers & 0xff00);
}

static layout_key_t char_to_hid_code(const char chr) {
    layout_key_t vscm = char_to_virtual_scan_code(chr);
    return make_hid_code(virtual_scan_code_to_hid_code(vscm), virtual_scan_code_to_hid_modifiers(vscm));
}

static void usage() {
    printf("usage: \n"
           "    flipperzero_badusb_kl.exe <options> [<file>]\n"
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
           "                           lgui, rshift, rctrl, ralt, rgui separated by a +\n"
           "        -k      displays valid hid key codes\n" 
           "        -l      lists the resulting mapping\n"
    );
}

static void show_keys() {
    printf("hid key codes:\n"
           "    ---------------------------------------------------------------------- \n"
           "   | 35 | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 2d | 2e |  <- |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  -> | 14 | 1a | 08 | 15 | 17 | 1c | 18 | 0c | 12 | 13 | 2f | 30 |    |\n"
           "    ------------------------------------------------------------------|   |\n"
           "   |   o  | 04 | 16 | 07 | 09 | 0a | 0b | 0d | 0e | 0f | 33 | 34 | 31 |   |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  ^  | 64 | 1d | 1b | 06 | 19 | 05 | 11 | 10 | 36 | 37 | 38 |    ^    |\n"
           "    ---------------------------------------------------------------------- \n"
           "   | ctrl | gui | alt |              2c                | alt | gui | ctrl |\n"
           "    ---------------------------------------------------------------------- \n");
}

static void show_layout(const layout_t layout) {
    printf("layout is:  (char : code : hid_key modifiers...)\n");
    for (size_t num = 32; num < 127; ++num) {
        layout_key_t code = layout[num];
        printf("   %c: %04x : %02x %s %s %s %s %s %s %s %s\n", (char)num, code, code & 0xff, 
               code & hid_mod_lctrl  ? "lctrl"  : "     ", 
               code & hid_mod_lshift ? "lshift" : "      ", 
               code & hid_mod_lalt   ? "lalt"   : "    ", 
               code & hid_mod_lgui   ? "lgui"   : "    ", 
               code & hid_mod_rctrl  ? "rctrl"  : "     ", 
               code & hid_mod_rshift ? "rshift" : "      ", 
               code & hid_mod_ralt   ? "ralt"   : "    ",
               code & hid_mod_rgui   ? "rgui"   : "    "
        );
    }
}

int main(int argc, char *argv[]) {
    bool help = false;
    bool read = false;
    bool write = false;
    bool dump = false;
    bool keys = false;
    bool list = false;
    bool file = false;
    char *path = NULL;
    size_t modify_size = 0;
    struct {
        char chr; layout_key_t code;
    } *modify = NULL;
    int result = 0;

    modify = malloc(sizeof(*modify) * (modify_size+1));
    memset(modify, 0, sizeof(*modify) * (modify_size+1));

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
            } else if (strcmp("-l", argv[argn]) == 0) {
                list = true;
                argn++;
            } else if (strcmp("-m", argv[argn]) == 0) {
                argn++;
                if (argn >= argc) {
                    log_error("modify: missing parameters\n");
                    result = -1;
                    break;
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
                    break;
                }

                char *end;
                layout_key_t code = strtol(argv[argn], &end, 16);
                if (((end-argv[argn]) != strlen(argv[argn])) || (code == 0)) {
                    log_error("modify: invalid hid key code specified\n");
                    result = -1;
                }
                size_t check;
                for (check = 0; check < sizeof(hid_codes)/sizeof(*hid_codes); ++check) {
                    if (hid_codes[check] == code)
                        break;
                }
                if (check >= sizeof(hid_codes)/sizeof(*hid_codes)) {
                    log_error("modify: hid key code out of range\n");
                    result = -1;
                }
                argn++;
                if (argn >= argc) {
                    log_error("modify: missing parameters\n");
                    result = -1;
                    break;
                }
                
                size_t pos = 0;
                while (pos < strlen(argv[argn])) {
                    if (strncmp(argv[argn]+pos, "none", 4) == 0) { 
                        pos += 4;                       
                    } else if (strncmp(argv[argn]+pos, "lshift", 6) == 0) { 
                        code |= hid_mod_lshift;
                        pos += 6;
                    } else if (strncmp(argv[argn]+pos, "lctrl", 5) == 0) { 
                        code |= hid_mod_lctrl;
                        pos += 5;
                    } else if (strncmp(argv[argn]+pos, "lalt", 4) == 0) { 
                        code |= hid_mod_lalt;
                        pos += 4;
                    } else if (strncmp(argv[argn]+pos, "lgui", 4) == 0) { 
                        code |= hid_mod_lgui;
                        pos += 4;
                    } else if (strncmp(argv[argn]+pos, "rshift", 6) == 0) { 
                        code |= hid_mod_rshift;
                        pos += 6;
                    } else if (strncmp(argv[argn]+pos, "rctrl", 5) == 0) { 
                        code |= hid_mod_rctrl;
                        pos += 5;
                    } else if (strncmp(argv[argn]+pos, "ralt", 4) == 0) { 
                        code |= hid_mod_ralt;
                        pos += 4;
                    } else if (strncmp(argv[argn]+pos, "rgui", 4) == 0) { 
                        code |= hid_mod_rgui;
                        pos += 4;
                    } else {
                        log_error("modify: invalid modifier specified\n");
                        result = -1;
                        break;
                    }

                    if (pos < strlen(argv[argn])) {
                        if (argv[argn][pos] == '+') {
                            pos++;
                        } else {
                            log_error("modify: multiple modifiers shall be separated by +");
                            result = -1;
                            break;
                        }
                    }
                }
                modify_size += 1;
                modify = realloc(modify, sizeof(*modify) * (modify_size+1));
                modify[modify_size-1].chr = chr;
                modify[modify_size-1].code = code;
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
    for (size_t index = 0; index < modify_size; ++index) {
        char chr = modify[index].chr;
        layout_key_t code = modify[index].code;
        printf("patching layout: set char %c to 0x%04x\n", chr, code);
        patch_layout(layout, chr, code);
    }
    if (keys)
        show_keys();
    if (list)
        show_layout(layout);
    if (write) {
        printf("writing layout file %s...\n", path);
        if (!write_layout(path, layout))
            return -1;
    }

    free(modify);
    return result;
}
