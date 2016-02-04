
RM = rm -f
CPPFLAGS = -Iinclude
CXXFLAGS = -std=c++14 -g3 -O0
#HEADERS = $(wildcard include/*.h)
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))
PROGS = $(patsubst %.cpp,%,$(SOURCES))
DEPS = $(patsubst %.cpp,%.d,$(SOURCES))

COMPILE = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

all: $(PROGS) $(DEPS)

-include $(DEPS)

%.d: %.cpp
	@echo "D: $@"
	@$(CXX) -MM $(CXXFLAGS) $(CPPFLAGS) $< | sed -re 's/([-a-zA-Z0-9]+)\.o/src\/\1/g' > $@
	@echo -e "\t@echo C: $@" | sed -re 's/\.d//g' >> $@
	@echo -e "\t@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<" | sed -re 's/\.d//g' >> $@

clean:
	rm -f $(PROGS) $(OBJECTS) $(DEPS)
