CXX ?= c++
CXXFLAGS += -O3 -Wall -Wextra -pedantic -std=c++11 -I.
LDFLAGS += -lncurses

all: format compile_commands.json

clean:
	rm -f vi demo/main.o compile_commands.json

vi: demo/main.o
	$(CXX) $(LDFLAGS) -o vi demo/main.o
	strip vi

demo/main.o: demo/main.cc pomvic.hh
	$(CXX) $(CXXFLAGS) -c -o demo/main.o demo/main.cc

compile_commands.json: Makefile demo/main.cc pomvic.hh
	bear -- make vi

format:
	clang-format -Werror -i --style=file --verbose pomvic.hh demo/main.cc

.PHONY: all clean format
