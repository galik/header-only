
CPPFLAGS = -Iinclude
CXXFLAGS = -std=c++14
SOURCES = $(wildcard src/*.cpp)
PROGS = $(patsubst %.cpp,%,$(SOURCES))

all: $(PROGS)
	
clean:
	rm -f *.o $(notdir $(patsubst %.cpp,%,$(SOURCES)))