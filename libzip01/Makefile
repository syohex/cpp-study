all: zipinfo

zipinfo: main.cpp
	$(CXX) -Wall -o zipinfo main.cpp -lzip

.PHONY: clean
clean:
	-rm -f zipinfo *.o
