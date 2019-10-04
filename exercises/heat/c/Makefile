COMP=pgi

ifeq ($(COMP),pgi)
CC=pgcc
CCFLAGS=-g -O3 -acc -ta=tesla,kepler+
RUN=
endif

EXE=heat
OBJS=pngwriter.o heat.o main.o

all: $(EXE)

# Dependencies
main.o: pngwriter.o heat.o main.c
heat.o: heat.c
pngwriter.o: pngwriter.c

$(EXE): $(OBJS) .ALWAYSEXEC
	$(CC) $(CCFLAGS) $(OBJS) -o $@ -lpng



%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	-/bin/rm -f $(EXE)
	-/bin/rm -f a.out *.o *~ *.png

.ALWAYSEXEC:
