CHARMC=~/work/charm/bin/charmc

OBJS = MeshStreamer.o

OPTS = -g

all: MeshStreamer.o MeshStreamerTest.o

MeshStreamer.o: MeshStreamer.C MeshStreamer.decl.h
	$(CHARMC) -g -c MeshStreamer.C

MeshStreamer.decl.h: MeshStreamer.ci
	$(CHARMC) MeshStreamer.ci

MeshStreamerTest.o: MeshStreamerTest.C MeshStreamerTest.decl.h MeshStreamer.o
	$(CHARMC) -g -o MeshStreamerTest MeshStreamerTest.C MeshStreamer.o 

MeshStreamerTest.decl.h: MeshStreamerTest.ci
	$(CHARMC) MeshStreamerTest.ci

clean:
	rm -f *.decl.h *.def.h *.o charmrun MeshStreamerTest *~

