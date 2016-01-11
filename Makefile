
RM = rm -f
CPPFLAGS = -Iinclude
CXXFLAGS = -std=c++14
SOURCES = $(wildcard src/*.cpp)
PROGS = $(patsubst %.cpp,%,$(SOURCES))

all: $(PROGS)
	
clean:
	$(RM) $(PROGS)