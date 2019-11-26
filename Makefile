src = bytestream.cpp
obj = $(src:.cpp=.o)

LDFLAGS = -ldl
CPPFLAGS = -std=c++11 -Wl,--export-dynamic -g -pedantic -Wall -Werror -Wextra

.PHONY: test
# test.cpp listed explicitly since chaning test.h would go unnoticed otherwise
test: $(obj)
	$(CXX) $(CPPFLAGS) -I . $^ test.cpp $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(obj) test
