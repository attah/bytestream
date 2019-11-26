src = bytestream.cpp
obj = $(src:.cpp=.o)

LDFLAGS = -ldl -Wl,--export-dynamic
CXXFLAGS = -std=c++11 -g -pedantic -Wall -Werror -Wextra

.PHONY: test
# test.cpp listed explicitly since changing test.h would go unnoticed otherwise
test: $(obj)
	$(CXX) $(CXXFLAGS) -I . $^ test.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(obj) test
