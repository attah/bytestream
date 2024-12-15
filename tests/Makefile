LDFLAGS = -Wl,--export-dynamic
CXXFLAGS = -std=c++17 -g -pedantic -Wall -Werror -Wextra

CLANG_TIDY ?= clang-tidy

ifeq ($(shell uname -s),Linux)
	LDFLAGS += -ldl
endif

.PHONY: test
# test.cpp listed explicitly since changing test.h would go unnoticed otherwise
test:
	$(CXX) $(CXXFLAGS) -I . -I .. ../bytestream.cpp test.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f test

tidy:
	$(CLANG_TIDY) ../bytestream.cpp -- $(CXXFLAGS)
