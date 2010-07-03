CHARMC = charmc
BINARY = random_access

SRCFILES = $(wildcard *.cc) $(wildcard *.c)
OBJFILES = $(SRCFILES:.cc=.o) $(SRCFILES:.c=.o)
CIFILES = $(wildcard *.ci)
HFILES = $(CIFILES:.ci=.decl.h) $(CIFILES:.ci=.def.h)

all: $(BINARY)
$(BINARY): $(OBJFILES)
	$(CHARMC) -language charm++ -o $@ $+

.SECONDARY: $(patsubst %.cc,%.decl.h,$(wildcard *.cc))
.SECONDARY: $(patsubst %.cc,%.def.h,$(wildcard *.cc))

%.decl.h %.def.h: %.ci
	$(CHARMC) $(CHARMCFLAGS) $<

%.o: %.cc $(HFILES)
	$(CHARMC) $(CHARMCFLAGS) $<

test: $(BINARY)
	./charmrun ./$(BINARY) $(TESTFLAGS)

clean:
	rm -f *.o *.decl.h *.def.h $(BINARY)* charmrun*
