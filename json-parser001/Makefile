CXXFLAGS=-std=c++17
ifeq ($(OS),Windows_NT)
	CXX=clang++
	AR=llvm-ar
else
	CXX=g++
	AR=ar
endif

all: json.a

json.a: json.o
	$(AR) rc json.a json.o

json.o: json.cpp
	$(CXX) $(CXXFLAGS) -c -o json.o json.cpp
