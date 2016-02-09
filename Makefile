-include Makefile.local

RM := rm -f
CPPFLAGS := -MMD -MP -I. $(CPPFLAGS)
CXXFLAGS := -std=c++14 $(CXXFLAGS)

SRCS := $(wildcard src/*.cpp)
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
PRGS := $(patsubst %.cpp,%,$(SRCS))

all: $(PRGS)

-include $(DEPS)

%: %.cpp
	@echo "C: $@"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS)
