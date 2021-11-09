CXXFLAGS = -Wall -g -fsanitize=address,undefined

all: json_test

json_test: test.cpp json_value.cpp json_parser.h input_source.h
	clang++ -std=c++17 $(CXXFLAGS) -o json_test test.cpp json_value.cpp

.PHONY: test
test: json_test
	./json_test

.PHONY: clean
clean:
	-rm -f json_test

.PHONY: format
format:
	-clang-format -i *.cpp *.h
