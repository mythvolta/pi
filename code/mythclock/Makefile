
# Completely build the socket listener

# Global variables
STD        := -lstdc++ -std=c++14 -pthread
CC         := gcc $(STD)
CXX        := g++ $(STD)
WARN       := -Wall -Wextra -Wfatal-errors
CFLAGS     := -fPIC $(WARN) -O1 -g
RM         := rm -f

# Binaries to build
BINARIES   := mythclock

# Where our library resides. You mostly only need to change the
# RGB_LIB_DISTRIBUTION, this is where the library is checked out.
RGB_LIB_DISTRIBUTION := /home/pi/code/rpi-rgb-led-matrix
RGB_INCDIR           := $(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR           := $(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME     := rgbmatrix
RGB_LIBRARY          := $(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

.PHONY: all 
all : $(BINARIES)

$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR)

mythclock : $(RGB_LIBRARY) mythclock.o socket_listener.o

# All the binaries that have the same name as the object file
% : %.o
	$(CXX) -o $@ $(LDFLAGS) $^
	$(CXX) $(CFLAGS) -o $@ $^

# Since the C example uses the C++ library underneath, which depends on C++
# runtime stuff, you still have to also link -lstdc++

# Rules to build C/C++ objects
%.o : %.cpp
	$(CXX) $^ -I$(RGB_INCDIR) $(CFLAGS) -c -o $@
%.o : %.cc
	$(CXX) $^ -I$(RGB_INCDIR) $(CFLAGS) -c -o $@
%.o : %.c
	$(CC) $^ -I$(RGB_INCDIR) $(CFLAGS) -c -o $@

.PHONY: clean
clean:
	$(RM) *.o $(BINARIES)

FORCE:
.PHONY: FORCE
