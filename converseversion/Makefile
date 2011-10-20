CHARMC = $(HOME)/git/charm/net-linux-x86_64/bin/charmc 
BINARY = random_access 

SRCFILES = $(wildcard *.cc)
OBJFILES = $(SRCFILES:.cc=.o)
CIFILES = $(wildcard *.ci)
HFILES = $(CIFILES:.ci=.decl.h) $(CIFILES:.ci=.def.h)
CHARMCFLAGS= -g -O3 $(OPTS)

all: $(BINARY) 
random_access: $(OBJFILES)
	$(CHARMC) $(CHARMCFLAGS) -language converse++ -o $@ $+


.SECONDARY: $(patsubst %.cc,%.decl.h,$(wildcard *.cc))
.SECONDARY: $(patsubst %.cc,%.def.h,$(wildcard *.cc))

%.o: %.cc

%.o: %.cc
	$(CHARMC) $(CHARMCFLAGS) $<

test: $(BINARY)
	./charmrun +p1 ./random_access 21 
testprj:random_access.prj
	poe ./random_access.prj 20 1 +traceroot /u/home/ac/yanhuas/git/HPCC/charmversion_messages/projections_result -llfile jobpoelapi.cmd +gz-trace

clean:
	rm -f *.o *.decl.h *.def.h $(BINARY) charmrun* random_access.prj
