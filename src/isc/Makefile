# Linux
CC =		gcc
CFLAGS =	-g -O2
FC =		gfortran
FFLAGS =	-g -O2

LIBS = -lm

BINDIR =	${HOME}/bin

BINARIES =	iss2isc \
		fixisc \
		iss2dat \
		dat2isc \
		dat2sta \
		datphases

OBJECTS =	trim.o \
		sindex.o \
		ltype.o \
		r_iss_epic.o \
		w_heq.o \
		w_isc1.o \
		w_isc5.o \
		w_isc6.o \
		w_isc7.o \
		get_isc_cards.o \
		r_iss_phase.o \
		phase_code.o \
		iaddsec1.c

all:		$(BINARIES)

iss2isc:	iss2isc.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/iss2isc iss2isc.o $(OBJECTS) $(LIBS)

fixisc:	fixisc.o
	$(CC) $(CFLAGS) -o $(BINDIR)/fixisc fixisc.o

iss2dat:	iss2dat.o trim.o
	$(CC) $(CFLAGS) -o $(BINDIR)/iss2dat iss2dat.o trim.o

dat2isc:	dat2isc.o get_hypo.o get_phases.o asterisk.o \
		phase_code2.o iaddsec1.o trim.o
	$(CC) $(CFLAGS) -o $(BINDIR)/dat2isc dat2isc.o \
		get_hypo.o get_phases.o asterisk.o phase_code2.o \
		iaddsec1.o trim.o -lm

dat2sta:	dat2sta.o trim.o
	$(CC) $(CFLAGS) -o $(BINDIR)/dat2sta dat2sta.o trim.o

datphases:	datphases.o
	$(FC) $(FFLAGS) -o $(BINDIR)/datphases datphases.o


clean:
	/bin/rm -f $(BINARIES) *.o

#-------------------------------------------------------------------------------
#       program dependencies
#-------------------------------------------------------------------------------
.c.o:
		$(CC) -c $(CFLAGS) $<

.f.o:
		$(FC) -c $(FFLAGS) $<

