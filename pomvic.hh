#include <cstdint>
#include <string>
#include <vector>

#ifdef POMVIC_SMALL
typedef int32_t vipos;
typedef std::string vistring;
typedef char8_t vichar;
#else
typedef int64_t vipos;
typedef std::u32string vistring;
typedef char32_t vichar;
#endif

namespace {
namespace pomvic {
enum cursor_mode {
        MODE_NORMAL = 0,
        MODE_INSERT,
        MODE_VISUAL,
        MODE_VISUAL_BLOCK,
        MODE_VISUAL_LINE,
        MODE_EX,
};
enum modifier {
        MOD_NONE = 0,
        MOD_CTRL = 1,
        // Alt on PC keyboards, Option on Macs
        MOD_META = 2,
        // Only available on Macs, could be Super on PCs,
        // but that would be weird, maybe use Control+Shift
        MOD_CMD = 4,
        MOD_ESC = 8,
        MOD_RET = 0x10,
};
struct vec2 {
        vipos x;
        vipos y;
};
struct cursor_t {
        vec2 pos;
        vec2 visual_origin;
        cursor_mode mode;
};
struct buffer {
        cursor_t cursor;
        std::vector<vistring> content;
        vistring _buffer;
        vistring newline;
#ifdef POMVIC_INTERNAL_YANK
        vistring yankboard;
#endif
        vipos under_cursor() {
                // TODO: ...
                return 0;
        }
};
std::vector<vistring> split_by_newlines(vistring raw, vistring *newline) {
        vistring nl;
        if(!newline) newline = &nl;
        bool b = false;
        for(vichar c : raw) {
                if(c == '\n' || c == '\r') {
                        *newline += c;
                        if(b) break;
                        b = true;
                }
                if(b) break;
        }
        if(*newline == U"") *newline = U"\n";
        std::vector<vistring> v;
        size_t pos;
        while((pos = raw.find(*newline)) != std::string::npos) {
                v.push_back(raw.substr(0, pos));
                raw.erase(0, pos + newline->length());
        }
        return v;
}
buffer init(vistring content) {
        vistring newline;
        auto lines = split_by_newlines(content, &newline);
        return {.content = lines, .newline = newline};
}
void yank(buffer &b, vistring s, bool e) {
#ifdef POMVIC_INTERNAL_YANK
        if(!e) {
                b.yankboard = s;
        } else {
#endif
                // TODO: how do i make external yank
#ifdef POMVIC_INTERNAL_YANK
        }
#endif
}
#define cmd(x) (s == x)
#define mod_cmd(x, y) cmd(x && (m & y))
#define none_cmd(x) cmd(x && !m)
#define ctrl_cmd(x) mod_cmd(x, MOD_CTRL)
#define meta_cmd(x) mod_cmd(x, MOD_META)
#define cmd_cmd(x) mod_cmd(x, MOD_CMD)
void handle_key_command(buffer &b, vichar c, modifier m) {
        auto &s = b._buffer += c;
        if(none_cmd(U"x")) {
                s.erase(b.under_cursor());
        } else if(none_cmd(U"X")) {
                s.erase(b.under_cursor() - 1);
        } else if(none_cmd(U"v")) {
                if(b.cursor.mode == MODE_VISUAL) {
                        b.cursor.mode = MODE_NORMAL;
                } else {
                        b.cursor.visual_origin = b.cursor.pos;
                        b.cursor.mode = MODE_VISUAL;
                }
        } else if(none_cmd(U"V")) {
                if(b.cursor.mode == MODE_VISUAL_LINE) {
                        b.cursor.mode = MODE_NORMAL;
                } else {
                        b.cursor.visual_origin = b.cursor.pos;
                        b.cursor.mode = MODE_VISUAL_LINE;
                }
        } else if(ctrl_cmd(U"v") || ctrl_cmd(U"V")) {
                if(b.cursor.mode == MODE_VISUAL_BLOCK) {
                        b.cursor.mode = MODE_NORMAL;
                } else {
                        b.cursor.visual_origin = b.cursor.pos;
                        b.cursor.mode = MODE_VISUAL_BLOCK;
                }
        } else if(none_cmd(U"w")) {
                // TODO: forward one word
        } else if(none_cmd(U"W")) {
                // TODO: forward one word
        } else if(none_cmd(U"b")) {
                // TODO: back one word
        } else if(none_cmd(U"B")) {
                // TODO: back one word
        } else {
                return;
        }
        s.clear();
}
#undef cmd
#undef mod_cmd
#undef none_cmd
#undef ctrl_cmd
#undef meta_cmd
#undef cmd_cmd
void update(buffer &b, vichar c, modifier m) {
        switch(b.cursor.mode) {
                case MODE_NORMAL:
                case MODE_VISUAL:
                case MODE_VISUAL_BLOCK:
                case MODE_VISUAL_LINE:
                        handle_key_command(b, c, m);
                        break;
                case MODE_INSERT:
                        if(m & MOD_ESC) {
                                // TODO: insert text
                        } else {
                                b._buffer += c;
                        }
                        break;
                case MODE_EX:
                        if(m & MOD_ESC) {
                                // TODO:close
                        } else if(m & MOD_RET) {
                                // TODO: run command
                        } else {
                                b._buffer += c;
                        }
                        break;
                default:
                        break;
        }
}
}
}
