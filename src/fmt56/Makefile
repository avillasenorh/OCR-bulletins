# Linux
CC =		gcc
CFLAGS =	-g -O
FC =		gfortran
FFLAGS =	-g -O

#BINDIR =	${HOME}/bin
BINDIR =	../../bin

BINARIES =	rmdash \
		fmt56

all:		$(BINARIES)

OBJ56 =		trim.o \
		trimh.o \
		mdash.o \
		int2eng.o \
		sindex.o \
		g56_ltype.o \
		g56_epic.o \
		w56_epic.o \
		g56_stnm.o \
		g56_dist.o \
		g56_az.o \
		g56_phase.o \
		fix_errors.o \
		valid_phase.o \
		g56_l.o \
		w56_comment.o

rmdash:	rmdash.o
	$(CC) $(CFLAGS) -o $(BINDIR)/rmdash rmdash.o

fmt56:	fmt56.o $(OBJ56)
	$(CC) $(CFLAGS) -o $(BINDIR)/fmt56 fmt56.o $(OBJ56) -lm

clean:
	/bin/rm -f $(BINARIES) *.o

#-------------------------------------------------------------------------------
#       program dependencies
#------------------------------------------------------------------------------- 
.c.o:
		$(CC) -c $(CFLAGS) $<

.f.o:
		$(FC) -c $(FFLAGS) $<

