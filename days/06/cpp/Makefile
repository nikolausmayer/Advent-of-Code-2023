
CXX ?= g++-12
CXXFLAGS ?= -W -Wall -Wextra -Wpedantic -O3 -std=c++23
.PHONY: default clean

default: part-1 part-2

clean:
	-rm part-1 part-2

%: src/%.o Makefile
	$(CXX) $< -o $@

src/%.o: src/%.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

