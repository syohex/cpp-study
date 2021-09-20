CXXFLAGS=-Wall -std=c++17
ifeq ($(OS),Windows_NT)
	CXX=clang++
	AR=llvm-ar
	TEST_OUTPUT=json_test.exe
else
	CXX=g++
	AR=ar
	TEST_OUTPUT=json_test
endif

all: json.a

json.a: json.o
	$(AR) rc json.a json.o

json.o: json.cpp
	$(CXX) $(CXXFLAGS) -c -o json.o json.cpp

json_test: json_test.cpp json.a
	$(CXX) $(CXXFLAGS) -o $(TEST_OUTPUT) json_test.cpp json.a

.PHONY: test

test: json_test
	./json_test

.PHONY: clean
clean:
	rm -f json_test* *.o