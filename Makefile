-include Makefile.local

PREFIX ?= /usr/local

CP := cp -r
RM := rm -f
CPPFLAGS := -Iinclude $(CPPFLAGS)
CXXFLAGS := -std=c++14 -pthread -MMD -MP $(CXXFLAGS)

SRCS := $(wildcard src/*.cpp)
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
PRGS := $(patsubst %.cpp,%,$(SRCS))

all: $(PRGS)

show:
	@echo SRCS $(SRCS)
	@echo DEPS $(DEPS)
	@echo PRGS $(PRGS)

%: %.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	
install:
	$(CP) hol $(PREFIX)/include
	$(CP) pkg-config/*.pc $(PREFIX)/lib/pkgconfig

-include $(DEPS)

.PHONY: show install

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS)
