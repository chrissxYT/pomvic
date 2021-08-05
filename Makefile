CXX ?= c++
CXXFLAGS += -O3 -Wall -Wextra -pedantic -std=c++11 -I.
LDFLAGS += -lncurses

all: vi

clean:
	rm -f vi demo/main.o

vi: demo/main.o
	$(CXX) $(LDFLAGS) -o vi demo/main.o
	strip vi

demo/main.o: demo/main.cc pomvic.hh
	$(CXX) $(CXXFLAGS) -c -o demo/main.o demo/main.cc

format:
	clang-format -Werror -i --style=file --verbose pomvic.hh demo/main.cc

.PHONY: all clean format
