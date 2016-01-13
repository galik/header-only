
RM = rm -f
CPPFLAGS = -Iinclude
CXXFLAGS = -std=c++14 -g3 -O0
HEADERS = $(wildcard include/*.h)
SOURCES = $(wildcard src/*.cpp)
PROGS = $(patsubst %.cpp,%,$(SOURCES))

all: $(PROGS) 
	
clean:
	$(RM) $(PROGS)