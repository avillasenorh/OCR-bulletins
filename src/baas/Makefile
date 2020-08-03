# Linux
CC =		gcc
CFLAGS =	-g -O2
FC =		gfortran
FFLAGS =	-g -O2

#BINDIR =	${HOME}/bin
BINDIR =	../../bin

BINARIES =	fmt13 \
		fmt14

all:		$(BINARIES)

OBJS =		trim.o \
		substr.o \
		phase_code.o \
		w_isc1.o \
		w_isc5.o \
		w_isc6.o \
		w_isc7.o

fmt13:	fmt13.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/fmt13 fmt13.o $(OBJS) -lm

fmt14:	fmt14.o $(OBJS)
	$(CC) $(CFLAGS) -o $(BINDIR)/fmt14 fmt14.o $(OBJS) -lm

clean:
	/bin/rm -f $(BINARIES) *.o

#-------------------------------------------------------------------------------
#       program dependencies
#------------------------------------------------------------------------------- 
.c.o:
		$(CC) -c $(CFLAGS) $<

.f.o:
		$(FC) -c $(FFLAGS) $<

