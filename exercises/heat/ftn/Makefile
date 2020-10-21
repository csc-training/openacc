COMP=pgi

ifeq ($(COMP),pgi)
CC=pgcc
FC=pgf90
CCFLAGS=-g -acc -O3 -ta=tesla:cc70
FCFLAGS=-g -acc -O3 -ta=tesla:cc70
# FCFLAGS=-g -Mbounds
RUN=
endif

EXE=heat
OBJS=pngwriter.o heat.o main.o

all: $(EXE)

# Dependencies
main.o: pngwriter.o heat.o main.F90
heat.o: heat.F90

$(EXE): $(OBJS) .ALWAYSEXEC
	$(FC) $(FCFLAGS) $(OBJS) -o $@ -lpng

pngwriter.o: pngwriter.c
	$(CC) $(CCFLAGS) -c $< -o $@

%.o: %.F90
	$(FC) $(FCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE)
	-/bin/rm -f  *.o *.mod *~ *.png

.ALWAYSEXEC:
