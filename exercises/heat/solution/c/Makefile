COMP=pgi

ifeq ($(COMP),pgi)
CC=pgcc
CCFLAGS=-g -O3 -acc -ta=tesla,cc70
RUN=
endif

EXE1=heat_1
OBJS1=pngwriter.o heat_1.o main_1.o

EXE2=heat_2
OBJS2=pngwriter.o heat_2.o main_2.o

all: $(EXE1) $(EXE2)

# Dependencies
main_1.o: pngwriter.o heat_1.o main_1.c
main_2.o: pngwriter.o heat_2.o main_2.c
heat_1.o: heat_1.c
heat_2.o: heat_2.c

pngwriter.o: pngwriter.c

$(EXE1): $(OBJS1) .ALWAYSEXEC
	$(CC) $(CCFLAGS) $(OBJS1) -o $@ -lpng

$(EXE2): $(OBJS2) .ALWAYSEXEC
	$(CC) $(CCFLAGS) $(OBJS2) -o $@ -lpng



%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE1) $(EXE2)
	-/bin/rm -f a.out *.o *~ *.png

.ALWAYSEXEC:
