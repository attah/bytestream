.PHONY: test
test:
	g++ -std=c++11 -Wl,--export-dynamic -g -I . test.cpp bytestream.cpp -ldl -o test
