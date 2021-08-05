#include <ncurses.h>

#include <codecvt>
#include <fstream>
#include <iostream>
#include <pomvic.hh>

int main(int argc, char **argv) {
        if(argc != 2) {
                std::cout << "You need to pass in exactly 1 file to edit."
                          << std::endl;
                return 1;
        }

        // this helps us get ESC without weird problems
        putenv((char *)"ESCDELAY=0");

        // init ncurses
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        refresh();

        std::ifstream f;
        f.open(argv[1]);
        std::wstring_convert<std::codecvt_utf8<vichar>, vichar> c;
        std::string s((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());
        auto buffer = pomvic::init(c.from_bytes(s));
        f.close();

        endwin();
}
