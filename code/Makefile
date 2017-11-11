# Global variables
CC         := gcc
CXX        := g++
SRCDIR     := src
INCDIR     := include
BINDIR     := bin
CFLAGS     := -O2 -g
CPPFLAGS   := -std=c++14 -I$(INCDIR)
LDFLAGS    := -lwiringPi -lpthread
RM         := rm -f
BIN        := $(BINDIR)/elder_control

# We will auto-generate dependencies
SRCS := $(SRCDIR)/elder_control.cc
OBJS  = $(SRCS:.cc=.o)
DEPS  = $(SRCS:.cc=.d)

.PHONY: all
all: debug $(BIN)

# Actually build the binary
$(BIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Auto-generate dependencies for all .cc files
%.d: %.cc
	@echo "Generating dependency $@ from $<"
	set -e; rm -f $@; \
    $(CXX) -I$(INCDIR) -MM -MT $< $< > $@.$$$$; \
    sed 's|\($*\)\.o[ :]*|\1.o $@ : |g' < $@.$$$$ > $@; \
    ls -al $@.$$$$
	#rm -f $@.$$$$

.PHONY: clean
clean:
	-${RM} $(BIN) $(OBJS) $(DEPS)

.PHONY: rules
rules:
	@echo "Target name   : [$@]"
	@echo "First prereq  : [$<]"
	@echo "All prereqs   : [$^]"
	@echo "Newer prereqs : [$?]"
	@echo "Stem          : [$*]"

.PHONY: debug
debug:
	@echo "BIN  = $(BIN)"
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
	@echo "DEPS = $(DEPS)"
