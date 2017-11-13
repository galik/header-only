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

CXX_14_TIME_FLAGS := -std=c++14 -pthread -MMD -MP -pedantic-errors -O3 -g0
CXX_17_TIME_FLAGS := -std=c++17 -pthread -MMD -MP -pedantic-errors -O3 -g0

HEADERS := $(wildcard hol/*.h)
PKGCFGS := $(wildcard pkg-config/*.pc)

DOCS := doxy-docs/*
TEST_SRCS := $(wildcard src/test*.cpp)
TIME_SRCS := $(wildcard src/time*.cpp)
#DEPS := $(patsubst %.cpp,%.d,$(SRCS))
#TESTS := $(patsubst %.cpp,%,$(SRCS))
TESTS_11 := $(patsubst %.cpp,%-11,$(TEST_SRCS))
TESTS_14 := $(patsubst %.cpp,%-14,$(TEST_SRCS))
TESTS_17 := $(patsubst %.cpp,%-17,$(TEST_SRCS))
TESTS := $(TESTS_11) $(TESTS_14) $(TESTS_17)

TIMES_14 := $(patsubst %.cpp,%-14,$(TIME_SRCS))
TIMES := $(TIMES_11) $(TIMES_14) $(TIMES_17)

SRCS := $(TEST_SRCS) $(TIME_SRCS)
DEPS := $(patsubst %.cpp,%-14.d,$(SRCS))

#all: $(TESTS_11) $(TESTS_14) $(TESTS_17)
#all: $(TESTS_14) $(TESTS_17)
all: $(TESTS_14) $(TIMES_14)

show:
	@echo TEST_SRCS $(TEST_SRCS)
	@echo DEPS $(DEPS)
	@echo TESTS $(TESTS)
	@echo TIMES $(TIMES)

#%: %.cpp
#	@echo "C: $@"
#	@echo [triggered by changes in $?]
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	
#%-11: %.cpp
#	@echo "C: $@"
#	@echo [triggered by changes in $?]
#	$(CXX) $(CXX_11_FLAGS) $(CPPFLAGS) -o $@ $<
	
test%-14: test%.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	$(CXX) $(CXX_14_FLAGS) $(CPPFLAGS) -o $@ $<
	
test%-17: test%.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	$(CXX) $(CXX_17_FLAGS) $(CPPFLAGS) -o $@ $<
	
time%-14: time%.cpp
	@echo "C: $@"
	@echo [triggered by changes in $?]
	$(CXX) $(CXX_14_TIME_FLAGS) $(CPPFLAGS) -o $@ $<
	
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
	@$(RM) $(DEPS) $(TESTS) $(TIMES) $(DOCS)
