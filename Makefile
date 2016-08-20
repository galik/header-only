-include Makefile.local

PREFIX ?= /usr/local

STAMP := $(shell date +%Y%m%d-%H%M%S)

CP := cp -r
RM := rm -f
INSTALL := install
CPPFLAGS := -I. $(CPPFLAGS)
CXXFLAGS := -std=c++14 -pthread -MMD -MP $(CXXFLAGS)

HEADERS := $(wildcard hol/*.h)
PKGCFGS := $(wildcard pkg-config/*.pc)

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
	@echo Creating header folders
	@$(INSTALL) -d $(PREFIX)/include/hol
	@echo Installing headers
	@for file in $(HEADERS); \
	do \
		$(INSTALL) -v "$$file" "$(PREFIX)/include/hol"; \
	done
	@echo Creating pkg-config folders
	@$(INSTALL) -d $(PREFIX)/lib/pkgconfig
	@echo Installing pkg-config files
	@for file in $(PKGCFGS); \
	do \
		$(INSTALL) -v "$$file" "$(PREFIX)/lib/pkgconfig"; \
	done

uninstall:
	@echo Uninstalling headers
	@for file in $(patsubst hol/%.h,%.h,$(HEADERS)); \
	do \
		$(RM) $(PREFIX)/include/hol/$$file; \
	done
	@echo Uninstalling pkg-config files
	@for file in $(patsubst pkg-config/%.pc,%.pc,$(PKGCFGS)); \
	do \
		$(RM) $(PREFIX)/lib/pkgconfig/$$file; \
	done
	
-include $(DEPS)

.PHONY: show install uninstall

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS)
