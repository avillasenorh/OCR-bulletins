# Linux
CC =		gcc
CFLAGS =	-g -O
FC =		gfortran
FFLAGS =	-g -O

#BINDIR =	${HOME}/bin
BINDIR =	../../bin

BINARIES =	fmt57

all:		$(BINARIES)

OBJ57 = 	trim.o \
		trimh.o \
		cmdash.o \
		yrpg.o \
		linetype.o \
		get_hypo.o \
		get_depth.o \
		get_abc.o \
		get_ghk.o \
		get_se.o \
		get_stnm.o \
		get_distaz.o \
		get_phases.o \
		sindex.o \
		w_hypo.o \
		w_phases.o
 
fmt57:	fmt57.o $(OBJ57)
	$(CC) $(CFLAGS) -o $(BINDIR)/fmt57 fmt57.o $(OBJ57)

clean:
	/bin/rm -f $(BINARIES) *.o

#-------------------------------------------------------------------------------
#       program dependencies
#------------------------------------------------------------------------------- 
.c.o:
		$(CC) -c $(CFLAGS) $<

.f.o:
		$(FC) -c $(FFLAGS) $<

