#include <cstdint>
#include <string>
#include <vector>

#ifdef POMVIC_SMALL
typedef uint32_t vipos;
typedef std::string vistring;
typedef char8_t vichar;
#else
typedef uint64_t vipos;
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
struct key {
        vichar c;
        modifier m;
};
struct vec2 {
        vipos x;
        vipos y;
};
typedef std::vector<vistring> buffer;
struct cursor {
        vec2 pos;
        vec2 visual_origin;
        cursor_mode mode;
        buffer insert_buffer;
        vistring command_buffer;
#ifdef POMVIC_INTERNAL_YANK
        buffer yankboard;
#endif
};
class vi {
        public:
        buffer b;
        cursor c;
        vistring newline;
        vipos &x, &y;
        cursor_mode &mode;
        buffer &insert_buffer;
        vistring &command_buffer;
        vec2 &pos, &visual_origin;
#ifdef POMVIC_INTERNAL_YANK
        buffer &yankboard;
#endif
        vi(buffer _b, cursor _c, vistring _nl) : b(_b), c(_c), newline(_nl), x(c.pos.x), y(c.pos.y), mode(c.mode), insert_buffer(c.insert_buffer), command_buffer(c.command_buffer), pos(c.pos), visual_origin(c.visual_origin)
#ifdef POMVIC_INTERNAL_YANK
        , yankboard(c.yankboard)
#endif
        {}
        void handle_key_command(key k);
        void update(key k);
};
buffer split_by_newlines(vistring raw, vistring *newline) {
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
        buffer v;
        size_t pos;
        while((pos = raw.find(*newline)) != std::string::npos) {
                v.push_back(raw.substr(0, pos));
                raw.erase(0, pos + newline->length());
        }
        return v;
}
vi init(vistring content) {
        vistring newline;
        auto lines = split_by_newlines(content, &newline);
        return vi(lines, cursor(), newline);
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
inline bool _cmd(vistring s, std::string x) {
        return s == vistring(x.begin(), x.end());
}
void run_ex_command(buffer &b, vistring cmd) {
        // TODO: ...
}
}

#define cmd(x) (pomvic::_cmd(s, #x))
#define mod_cmd(x, y) (cmd(x) && (k.m & y))
#define none_cmd(x) (cmd(x) && !k.m)
#define ctrl_cmd(x) mod_cmd(x, MOD_CTRL)
#define meta_cmd(x) mod_cmd(x, MOD_META)
#define cmd_cmd(x) mod_cmd(x, MOD_CMD)
#define group(x) (s[0] == (vichar)#x[0])
void pomvic::vi::handle_key_command(pomvic::key k) {
        auto &s = command_buffer += k.c;
        if(none_cmd(x)) {
                b[y].erase(x);
        } else if(none_cmd(X)) {
                // TODO: check if this can throw
                b[y].erase(x - 1);
        } else if(none_cmd(i)) {
                insert_buffer.clear();
                mode = MODE_INSERT;
        } else if(none_cmd(I)) {
                // TODO
        } else if(none_cmd(a)) {
                insert_buffer.clear();
                mode = MODE_INSERT;
                x++;
        } else if(none_cmd(A)) {
                // TODO: $a
        } else if(none_cmd(v)) {
                if(mode == MODE_VISUAL) {
                        mode = MODE_NORMAL;
                } else {
                        c.visual_origin = c.pos;
                        c.mode = MODE_VISUAL;
                }
        } else if(none_cmd(V)) {
                if(mode == MODE_VISUAL_LINE) {
                        mode = MODE_NORMAL;
                } else {
                        c.visual_origin = c.pos;
                        mode = MODE_VISUAL_LINE;
                }
        } else if(ctrl_cmd(v) || ctrl_cmd(V)) {
                if(mode == MODE_VISUAL_BLOCK) {
                        mode = MODE_NORMAL;
                } else {
                        c.visual_origin = c.pos;
                        mode = MODE_VISUAL_BLOCK;
                }
        } else if(none_cmd(w)) {
                // TODO: forward one word
        } else if(none_cmd(W)) {
                // TODO: forward one word
        } else if(none_cmd(b)) {
                // TODO: back one word
        } else if(none_cmd(B)) {
                // TODO: back one word
        } else if(none_cmd(h)) {
                if(x > 0) {
                        x--;
                }
        } else if(none_cmd(j)) {
                if(y < b.size() - 1) {
                        x++;
                }
        } else if(none_cmd(k)) {
                if(y > 0) {
                        y--;
                }
        } else if(none_cmd(l)) {
                if(x < b[y].size() - 1) {
                        x++;
                }
        } else if(none_cmd(0)) {
                x = 0;
        } else if (group(c)) {
        } else if (group(d)) {
        } else if (none_cmd(gg)) {
        }
        else {
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
#undef group
void pomvic::vi::update(pomvic::key k) {
        switch(mode) {
                case MODE_NORMAL:
                case MODE_VISUAL:
                case MODE_VISUAL_BLOCK:
                case MODE_VISUAL_LINE:
                        handle_key_command(k);
                        break;
                case MODE_INSERT:
                        if(k.m & MOD_ESC) {
                                // TODO: insert text
                        } else {
                                insert_buffer += k.c;
                        }
                        break;
                case MODE_EX:
                        if(k.m & MOD_ESC) {
                                command_buffer.clear();
                                mode = MODE_NORMAL;
                        } else if(k.m & MOD_RET) {
                                run_ex_command(b, command_buffer);
                                command_buffer.clear();
                                mode = MODE_NORMAL;
                        } else {
                                command_buffer += k.c;
                        }
                        break;
                default:
                        break;
        }
}
}
