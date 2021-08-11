CXX ?= c++
CXXFLAGS += -O3 -Wall -Wextra -pedantic -std=c++11 -I.
LDFLAGS += -lncurses

all: format compile_commands.json

clean:
	rm -f vi vi.o compile_commands.json

vi: vi.o
	$(CXX) $(LDFLAGS) -o vi vi.o
	strip vi

vi.o: vi.cc pomvic.hh
	$(CXX) $(CXXFLAGS) -c -o vi.o vi.cc

format:
	clang-format -Werror -i --style=file --verbose pomvic.hh vi.cc

.PHONY: all clean format
