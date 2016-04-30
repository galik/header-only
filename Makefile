-include Makefile.local

CP := cp -r
RM := rm -f
CPPFLAGS := -Iinclude $(CPPFLAGS)
CXXFLAGS := -std=c++14 -MMD -MP $(CXXFLAGS)

SRCS := $(wildcard src/*.cpp)
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
PRGS := $(patsubst %.cpp,%,$(SRCS))

all: $(PRGS)

%: %.cpp
	@echo "C: $@"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	
install:
	$(CP) hol $(HOME)/dev/include
	$(CP) pkg-config/*.pc $(HOME)/dev/lib/pkgconfig

-include $(DEPS)

.PHONY: install

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS)
