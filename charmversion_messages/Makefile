CHARMC = /expand/home/jessie/PPL/git/charm/net-linux-x86_64/bin/charmc -g
BINARY = random_access

SRCFILES = $(wildcard *.cc)
OBJFILES = $(SRCFILES:.cc=.o)
CIFILES = $(wildcard *.ci)
HFILES = $(CIFILES:.ci=.decl.h) $(CIFILES:.ci=.def.h)

all: $(BINARY)
$(BINARY): $(OBJFILES)
	$(CHARMC) -language charm++ -o $@ $+

.SECONDARY: $(patsubst %.cc,%.decl.h,$(wildcard *.cc))
.SECONDARY: $(patsubst %.cc,%.def.h,$(wildcard *.cc))

%.def.h %.decl.h: %.ci
	$(CHARMC) $(CHARMCFLAGS) $<

%.o: %.cc

%.o: %.cc $(HFILES)
	$(CHARMC) $(CHARMCFLAGS) $<

test: $(BINARY)
	./charmrun ./$(BINARY) $(TESTFLAGS)

clean:
	rm -f *.o *.decl.h *.def.h $(BINARY)* charmrun*
