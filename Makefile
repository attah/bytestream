src = bytestream.cpp
obj = $(src:.cpp=.o)

LDFLAGS = -ldl
CXXFLAGS = -std=c++11 -g -pedantic -Wall -Werror -Wextra

.PHONY: test
# test.cpp listed explicitly since changing test.h would go unnoticed otherwise
# -Wl,--export-dynamic given here as clang (rightfully) finds it unused
# in .o steps
test: $(obj)
	$(CXX) $(CXXFLAGS) -I . -Wl,--export-dynamic $^ test.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(obj) test
