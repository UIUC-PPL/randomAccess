
CHARMC = $(HOME)/src/charm/bin/charmc
BINARY = random_access

SRCFILES = $(wildcard *.cc)
OBJFILES = $(SRCFILES:.cc=.o)
CIFILES = $(wildcard *.ci)

all: $(BINARY)
$(BINARY): $(OBJFILES)
	$(CHARMC) -language charm++ -o $@ $+

.SECONDARY: $(patsubst %.cc,%.decl.h,$(wildcard *.cc))
.SECONDARY: $(patsubst %.cc,%.def.h,$(wildcard *.cc))

%.o: %.cc %.decl.h %.def.h
	$(CHARMC) $(CHARMCFLAGS) $<

%.decl.h %.def.h: %.ci
	$(CHARMC) $(CHARMCFLAGS) $<

test: $(BINARY)
	./charmrun ./$(BINARY) $(TESTFLAGS)

clean:
	rm -f *.o *.decl.h *.def.h $(BINARY)* charmrun*
