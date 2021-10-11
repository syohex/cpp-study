CXXFLAGS=-Wall -g -fsanitize=address,undefined

all: ranking_cli

ranking_cli: main.cpp ranking.o
	clang++ $(CXXFLAGS) -o ranking_cli main.cpp ranking.o

ranking.o: ranking.cpp
	clang++ $(CXXFLAGS) -o ranking.o -c ranking.cpp

unit_test: test.cpp ranking.o
	clang++ $(CXXFLAGS) -o unit_test test.cpp ranking.o

.PHONY: test
test: unit_test
	./unit_test

.PHONY: clean
clean:
	-rm -f *.o *.pdb *.lib *.exp ranking_cli unit_test
