CXXFLAGS=-Wall -g -std=c++17 -fsanitize=address,undefined

all: test

test: test.cpp basex.o
	$(CXX) $(CXXFLAGS) -o test test.cpp basex.o

basex.o: basex.cpp
	$(CXX) $(CXXFLAGS) -c -o basex.o basex.cpp
