.PHONY: test
test:
	g++ -std=c++11 -Wl,--export-dynamic -g -pedantic -Wall -Werror -Wextra \
	-I . test.cpp bytestream.cpp -ldl -o test
