-include Makefile.local

PREFIX ?= /usr/local

STAMP := $(shell date +%Y%m%d-%H%M%S)

CP := cp -r
RM := rm -fr
INSTALL := install
DOXYGEN := doxygen
CPPFLAGS := -I. $(CPPFLAGS) $(GSL_CPPFLAGS)
#CXXFLAGS := -std=c++14 -pthread -MMD -MP -pedantic-errors $(CXXFLAGS)
CXX_11_FLAGS := -std=c++11 -pthread -MMD -MP -pedantic-errors $(CXXFLAGS)
CXX_14_FLAGS := -std=c++14 -pthread -MMD -MP -pedantic-errors $(CXXFLAGS)
CXX_17_FLAGS := -std=c++17 -pthread -MMD -MP -pedantic-errors $(CXXFLAGS)

HEADERS := $(wildcard hol/*.h)
PKGCFGS := $(wildcard pkg-config/*.pc)

DOCS := doxy-docs/*
SRCS := $(wildcard src/*.cpp)
#DEPS := $(patsubst %.cpp,%.d,$(SRCS))
#PRGS := $(patsubst %.cpp,%,$(SRCS))
PRGS_11 := $(patsubst %.cpp,%-11,$(SRCS))
PRGS_14 := $(patsubst %.cpp,%-14,$(SRCS))
PRGS_17 := $(patsubst %.cpp,%-17,$(SRCS))
PRGS := $(PRGS_11) $(PRGS_14) $(PRGS_17)
DEPS := $(patsubst %.cpp,%-14.d,$(SRCS))

#all: $(PRGS_11) $(PRGS_14) $(PRGS_17)
#all: $(PRGS_14) $(PRGS_17)
all: $(PRGS_14)

show:
	@echo SRCS $(SRCS)
	@echo DEPS $(DEPS)
	@echo PRGS $(PRGS)

#%: %.cpp
#	@echo "C: $@"
#	@echo [triggered by changes in $?]
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	
#%-11: %.cpp
#	@echo "C: $@"
#	@echo [triggered by changes in $?]
#	$(CXX) $(CXX_11_FLAGS) $(CPPFLAGS) -o $@ $<
	
%-14: %.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	$(CXX) $(CXX_14_FLAGS) $(CPPFLAGS) -o $@ $<
	
%-17: %.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	$(CXX) $(CXX_17_FLAGS) $(CPPFLAGS) -o $@ $<
	
docs: doxy-docs/index.html

doxy-docs/index.html: $(SRCS)
	$(DOXYGEN) Doxyfile 

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

.PHONY: show docs install uninstall

clean:
	@echo "Cleaning build files."
	@$(RM) $(DEPS) $(PRGS) $(DOCS)
