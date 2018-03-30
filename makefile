CROSS_COMPILE=

CPP=$(CROSS_COMPILE)g++

PROJ=mfm
PROJDIR=$(CURDIR)

INCDIR=-I$(PROJDIR)/include
SRCDIR=$(PROJDIR)/src
OBJDIR=$(PROJDIR)/obj
BINDIR=$(PROJDIR)/bin

BINARY=$(BINDIR)/$(PROJ)

LIBS=-lfltk -lXext -lX11 -lm
SLIBS=-lstdc++fs -lconfig

### linker flags
LDFLAGS=-Wl,-Map,$(BINDIR)/$(PROJ).map $(LIBS)
# enable this line for debug version
#CFLAGS=-DDEBUG_VERSION -O0 -ggdb --pipe -c
CFLAGS=-O3 --pipe -c -std=c++17

### files name preparing macroes
SRCS:= $(wildcard $(SRCDIR)/*.cc)
OBJS:= $(patsubst %.cc,%.o,$(SRCS))
OBJS:= $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(OBJS))

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CPP) $(LDFLAGS) $(OBJS) -o $@ $(SLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CPP) $(CFLAGS) $(INCDIR) -c -o $@ $<

clean:
	rm -f obj/*.o bin/*
