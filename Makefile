
RM := rm -f
CPPFLAGS := -Iinclude
CXXFLAGS := -std=c++14 -g3 -O0

SRCS := $(wildcard src/*.cpp)
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
PRGS := $(patsubst %.cpp,%,$(SRCS))

all: $(PRGS) $(DEPS)

-include $(DEPS)

%.d: %.cpp
	@echo "D: $@"
	@$(CXX) -MM $(CXXFLAGS) $(CPPFLAGS) $< | sed -re 's/([-a-zA-Z0-9]+)\.o/src\/\1/g' > $@
	@echo -e "\t@echo C: $@" | sed -re 's/\.d//g' >> $@
	@echo -e "\t@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<" | sed -re 's/\.d//g' >> $@

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS)
