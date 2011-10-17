CHARMC = /expand/home/jessie/PPL/git/charm/mpi-linux-x86_64-smp/bin/charmc 
BINARY = random_access projections

SRCFILES = $(wildcard *.cc)
OBJFILES = $(SRCFILES:.cc=.o)
CIFILES = $(wildcard *.ci)
HFILES = $(CIFILES:.ci=.decl.h) $(CIFILES:.ci=.def.h)
CHARMCFLAGS= -g -O3 $(OPTS)

all: $(BINARY) 
random_access: $(OBJFILES)
	$(CHARMC) -language charm++ -o $@ $+

projections: $(OBJFILES)
	$(CHARMC) -language charm++ -tracemode projections -o random_access.prj $+

.SECONDARY: $(patsubst %.cc,%.decl.h,$(wildcard *.cc))
.SECONDARY: $(patsubst %.cc,%.def.h,$(wildcard *.cc))

%.def.h %.decl.h: %.ci
	$(CHARMC) $(CHARMCFLAGS) $<

%.o: %.cc

%.o: %.cc $(HFILES)
	$(CHARMC) $(CHARMCFLAGS) $<

test: $(BINARY)
	./charmrun +p1 ./random_access 21 
testprj:random_access.prj
	poe ./random_access.prj 20 1 +traceroot /u/home/ac/yanhuas/git/HPCC/charmversion_messages/projections_result -llfile jobpoelapi.cmd +gz-trace
clean:
	rm -f *.o *.decl.h *.def.h $(BINARY)* charmrun* random_access.prj
