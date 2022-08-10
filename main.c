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

static const layout_key_t hid_key_none   = 0;
static const layout_key_t hid_mod_lctrl  = (1 <<  8);
static const layout_key_t hid_mod_lshift = (1 <<  9);
static const layout_key_t hid_mod_lalt   = (1 << 10);
static const layout_key_t hid_mod_lgui   = (1 << 11);
static const layout_key_t hid_mod_rctrl  = (1 << 12);
static const layout_key_t hid_mod_rshift = (1 << 13);
static const layout_key_t hid_mod_ralt   = (1 << 14);
static const layout_key_t hid_mod_rgui   = (1 << 15);
static const layout_key_t hid_mod_all    = hid_mod_lctrl | hid_mod_lshift | hid_mod_lalt | hid_mod_lgui 
                                         | hid_mod_rctrl | hid_mod_rshift | hid_mod_ralt | hid_mod_rgui;

static const layout_key_t ordered_hid_codes[49] = {
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
    if (virtual_key == -1) {
        log_warning("no virtual key found for char %c\n", chr);
        return hid_key_none;
    }
    SHORT modifiers = (virtual_key & 0xff00);
    UINT  scan_code = MapVirtualKey((virtual_key & 0xff), MAPVK_VK_TO_VSC);
    if (scan_code == 0) {
        log_warning("no virtual scan code found for char %c\n", chr);
        return hid_key_none;
    }
    return (layout_key_t)(scan_code|modifiers);
}

static layout_key_t virtual_scan_code_to_hid_code(const layout_key_t vscm) {
    if (vscm == hid_key_none)
        return hid_key_none;
    
    layout_key_t vsc = (vscm & 0xff); // strip modifiers
    size_t index;
    for (index = 0; index < sizeof(vsc_to_hid)/sizeof(*vsc_to_hid); ++index) {
        if (vsc_to_hid[index][0] == vsc)
            break;
    }
    if (index < sizeof(vsc_to_hid)/sizeof(*vsc_to_hid)) {
        return vsc_to_hid[index][1];
    } else {
        log_warning("virtual scan code has no hid mapping: %04x\n", vscm);
        return hid_key_none;
    }
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
           "                map a key to a character. repeat option as necessary.\n"
           "                    <char> is any character between ascii(32) and ascii(126)\n"
           "                    <key>  is the hid key code (see -k)\n"
           "                    <mods> is any combination of none, lshift, lctrl, lalt,\n"
           "                           lgui, rshift, rctrl, ralt, rgui separated by a +\n"
           "        -k      display valid hid key codes\n" 
           "        -g      display a simplified graphical representation of the\n"
           "                resulting mapping. good for a quick check, prefer -l for\n"
           "                serious diagnostic\n"
           "        -l      list exhaustively the content of the resulting mapping\n"
           "        -e      expert mode. suppress all checks on following -m options,\n"
           "                effectively allowing to put whatever you see fit into the\n"
           "                keyboard layout. \n"
           "\n"
           "    options are applied in the following order: -r -d -m -k -g -l -w\n"
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
    char map[49*2];
    memset(map, ' ', sizeof(map));

    for (size_t num = 32; num < 127; ++num) {
        if (layout[num] != 0) {
            size_t key;
            for (key = 0; key < sizeof(ordered_hid_codes) / sizeof(*ordered_hid_codes); ++key) {
                if (ordered_hid_codes[key] == (layout[num] & 0xff)) 
                    break;
            }
            if (key < sizeof(ordered_hid_codes) / sizeof(*ordered_hid_codes)) {
                if ((layout[num] & hid_mod_all) == 0) // no modifier
                    map[key*2] = (char)num;
                else if ((layout[num] & hid_mod_all) == hid_mod_lshift) // only lshift modifier
                    map[key*2+1] = (char)num;
            }
        }
    }

    printf("layout quick overview (normal & lshift keys):\n"
           "    ---------------------------------------------------------------------- \n"
           "   | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c |  <- |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  -> | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c |    |\n"
           "    ------------------------------------------------------------------|   |\n"
           "   |   o  | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c |   |\n"
           "    ---------------------------------------------------------------------- \n"
           "   |  ^  | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c |    ^    |\n"
           "    ---------------------------------------------------------------------- \n"
           "   | ctrl | gui | alt |              %c%c                | alt | gui | ctrl |\n"
           "    ---------------------------------------------------------------------- \n",
           map[0],  map[1],  map[2],  map[3],  map[4],  map[5],  map[6],  map[7],  map[8],  map[9],  map[10], map[11], map[12], 
           map[13], map[14], map[15], map[16], map[17], map[18], map[19], map[20], map[21], map[22], map[23], map[24], map[25],
           map[26], map[27], map[28], map[29], map[30], map[31], map[32], map[33], map[34], map[35], map[36], map[37], 
           map[38], map[39], map[40], map[41], map[42], map[43], map[44], map[45], map[46], map[47], map[48], map[49], 
           map[50], map[51], map[52], map[53], map[54], map[55], map[56], map[57], map[58], map[59], map[60], map[61], 
           map[62], map[63], map[64], map[65], map[66], map[67], map[68], map[69], map[70], map[71], map[72], map[73], 
           map[74], map[75], map[76], map[77], map[78], map[79], map[80], map[81], map[82], map[83], map[84], map[85], 
           map[86], map[87], map[88], map[89], map[90], map[91], map[92], map[93], map[94], map[95], map[96], map[97]);
}

static void list_layout(const layout_t layout) {
    printf("layout is: \n"
           "   chr : code : key modifiers...\n");
    for (size_t num = 32; num < 127; ++num) {
        layout_key_t code = layout[num];
        printf("   %c   : %04x : %02x  %s %s %s %s %s %s %s %s\n", (char)num, code, code & 0xff, 
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
    bool show = false;
    bool list = false;
    bool expert = false;
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
            } else if (strcmp("-g", argv[argn]) == 0) {
                show = true;
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

                if (   (strlen(argv[argn]) != 1) 
                    || (!expert && ((int)argv[argn][0] < 32)) 
                    || (!expert && ((int)argv[argn][0] >= 127)) 
                    || (expert && ((int)argv[argn][0] >= 128))) {
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
                layout_key_t code = strtol(argv[argn], &end, 16) & 0xffff;  // limit code to 16 bits              
                if (((end-argv[argn]) != strlen(argv[argn])) || (code == 0)) {
                    log_error("modify: invalid hid key code specified\n");
                    result = -1;
                }
                size_t check;
                for (check = 0; check < sizeof(ordered_hid_codes)/sizeof(*ordered_hid_codes); ++check) {
                    if (ordered_hid_codes[check] == code)
                        break;
                }
                if (!expert && (check >= sizeof(ordered_hid_codes)/sizeof(*ordered_hid_codes))) {
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
            } else if (strcmp("-e", argv[argn]) == 0) {
                expert = true;
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
    if (show)
        show_layout(layout);
    if (list)
        list_layout(layout);
    if (write) {
        printf("writing layout file %s...\n", path);
        if (!write_layout(path, layout))
            return -1;
    }

    free(modify);
    return result;
}
