#include <cstdint>
#include <string>

// TODO: POMVIC_INTERNAL_YANK:
//       defined => like vim uses internal yank, external with "+
//       ndefned => like evil uses external always

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
enum mode {
        MODE_NORMAL = 0,
        MODE_INSERT,
        MODE_VISUAL,
        MODE_VISUAL_BLOCK,
        MODE_VISUAL_LINE,
        MODE_COMMAND,
};
struct cursor {
        vipos x;
        vipos y;
        mode m;
};
struct buffer {
        cursor cursor;
        vistring content;
        vistring _buffer;
        vipos under_cursor() {
                // TODO: ...
                return 0;
        }
};
buffer init(vistring content) { return {.content = content}; }
void handle_key_command(buffer &b, vichar c, bool visual) {
        auto &s = b._buffer += c;
        if(s == U"x") {
                s.erase(b.under_cursor());
        } else if(s == U"v") {
                b.cursor.m = MODE_VISUAL;
        } else if(s == U"V") {
                b.cursor.m = MODE_VISUAL_LINE;
        } else if(s == U"w") {
                // TODO: forward one word
        } else if(s == U"W") {
                // TODO: forward one word
        } else if(s == U"b") {
                // TODO: back one word
        } else if(s == U"B") {
                // TODO: back one word
        } else {
                return;
        }
        s.clear();
}
void update(buffer &b, vichar c) {
        switch(b.cursor.m) {
                case MODE_NORMAL:
                        handle_key_command(b, c, false);
                        break;
                case MODE_INSERT:
                        // TODO: check for escape
                        b._buffer += c;
                        break;
                case MODE_VISUAL:
                case MODE_VISUAL_BLOCK:
                case MODE_VISUAL_LINE:
                        handle_key_command(b, c, true);
                        break;
                case MODE_COMMAND:
                        // TODO: check for escape or return
                        b._buffer += c;
                        break;
                default:
                        break;
        }
}
}
}
