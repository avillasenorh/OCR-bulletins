# Linux
CC =		gcc
CFLAGS =	-g -O2
FC =		gfortran
FFLAGS =	-g -O2

BINDIR =	${HOME}/bin

BINARIES =	gr2ffb

all:		$(BINARIES)

OBJS =		trim.o \
		substr.o \
		phase_code.o \
		iaddsec1.o \
		w_isc1.o \
		w_isc5.o \
		w_isc6.o \
		w_isc7.o

gr2ffb:	gr2ffb.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/gr2ffb gr2ffb.o $(OBJS) -lm

clean:
	/bin/rm -f $(BINARIES) *.o

#-------------------------------------------------------------------------------
#       program dependencies
#------------------------------------------------------------------------------- 
.c.o:
		$(CC) -c $(CFLAGS) $<

.f.o:
		$(FC) -c $(FFLAGS) $<

