CXXFLAGS = -Wall -g -fsanitize=address,undefined

all: json_test

json_test: test.cpp json_value.cpp
	clang++ $(CXXFLAGS) -o json_test test.cpp json_value.cpp

.PHONY: test
test: json_test
	./json_test
