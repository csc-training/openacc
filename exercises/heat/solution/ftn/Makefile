COMP=pgi

ifeq ($(COMP),pgi)
CC=pgcc
FC=pgf90
CCFLAGS=-g -acc -O3 -ta=tesla:cc70
FCFLAGS=-g -acc -O3 -ta=tesla:cc70
# FCFLAGS=-g -Mbounds
RUN=
endif

EXE1=heat_1
OBJS1=pngwriter.o heat_1.o main_1.o

EXE2=heat_2
OBJS2=pngwriter.o heat_2.o main_2.o

all: $(EXE1) $(EXE2)

# Dependencies
main_1.o: pngwriter.o heat_1.o main_1.F90
heat_1.o: heat_1.F90

main_2.o: pngwriter.o heat_2.o main_2.F90
heat_2.o: heat_2.F90

$(EXE1): $(OBJS1) .ALWAYSEXEC
	$(FC) $(FCFLAGS) $(OBJS1) -o $@ -lpng

$(EXE2): $(OBJS2) .ALWAYSEXEC
	$(FC) $(FCFLAGS) $(OBJS2) -o $@ -lpng

pngwriter.o: pngwriter.c
	$(CC) $(CCFLAGS) -c $< -o $@

%.o: %.F90
	$(FC) $(FCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE1) $(EXE2)
	-/bin/rm -f  *.o *.mod *~ *.png

.ALWAYSEXEC:
