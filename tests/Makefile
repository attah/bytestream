LDFLAGS = -ldl -Wl,--export-dynamic
CXXFLAGS = -std=c++17 -g -pedantic -Wall -Werror -Wextra

.PHONY: test
# test.cpp listed explicitly since changing test.h would go unnoticed otherwise
test:
	$(CXX) $(CXXFLAGS) -I . -I .. ../bytestream.cpp test.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f test

analyze:
	clang++ $(CXXFLAGS) --analyze ../bytestream.cpp
