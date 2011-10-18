CHARMC=~/work/charm/bin/charmc

OBJS = MeshStreamer.o

all: MeshStreamer.o

MeshStreamer.o: MeshStreamer.C MeshStreamer.decl.h
	$(CHARMC) -c MeshStreamer.C

MeshStreamer.decl.h: MeshStreamer.ci
	$(CHARMC) MeshStreamer.ci

clean:
	rm -f *.decl.h *.def.h *.o 

